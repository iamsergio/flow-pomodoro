/*
  This file is part of Flow.

  Copyright (C) 2014 Sérgio Martins <iamsergio@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "webdavsyncer.h"
#include "jsonstorage.h"
#include "kernel.h"
#include "controller.h"
#ifndef NO_WEBDAV
#include "qwebdav.h"
#endif
#include <QState>
#include <QStateMachine>
#include <QSignalTransition>
#include <QNetworkReply>
#include <QBuffer>

class SyncState : public QState
{
public:
    SyncState(WebDAVSyncer *syncer)
        : QState()
        , m_syncer(syncer) { }

    QString lockFileName() const
    {
        return "/" + m_syncer->m_config.webDAVFileName() + ".lock";
    }

protected:
    WebDAVSyncer *m_syncer;
};

class InitialState : public SyncState
{
public:
    InitialState(WebDAVSyncer *syncer) : SyncState(syncer)
    {
        setObjectName("InitialState");
    }

    void onEntry(QEvent *event)
    {
        SyncState::onEntry(event);
        // qDebug() << "Entered InitialState, where we idle.";
        m_syncer->m_syncInProgress = false;
        emit m_syncer->syncInProgressChanged();
    }

    void onExit(QEvent *event)
    {
        SyncState::onExit(event);
        m_syncer->m_syncInProgress = true;
        emit m_syncer->syncInProgressChanged();
    }
};

class AcquireLockState : public SyncState
{
public:
    AcquireLockState(WebDAVSyncer *syncer) : SyncState(syncer)
    {
        setObjectName("AcquireLockState");
    }

    void onEntry(QEvent *event)
    {
        SyncState::onEntry(event);
        qDebug() << "Entered AcquireLockState";
        m_buffer = new QBuffer(&m_lockFileContents);
        m_buffer->open(QIODevice::WriteOnly);
        QNetworkReply *reply = m_syncer->m_webdav->get(lockFileName(), m_buffer);
        connect(reply, &QNetworkReply::finished, this, &AcquireLockState::onDownloadLockFileFinished);
    }
private:
    void onDownloadLockFileFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        QByteArray ourInstanceId = m_syncer->m_storage->instanceId();

        if (reply->error() == QNetworkReply::ContentNotFoundError) {
            QNetworkReply *reply2 = m_syncer->m_webdav->put(lockFileName(), ourInstanceId);
            connect(reply2, &QNetworkReply::finished, this, &AcquireLockState::onLockAcquired);
        } else if (reply->error() == 0) {
            // Lock file already present. Check if it's ours:
            if (m_lockFileContents == ourInstanceId) {
                // It's locked by us, for some reason lock wasn't removed. ( crash, network error..), so reuse it.
                qDebug() << Q_FUNC_INFO << "Reusing lock";
                emit m_syncer->lockAcquired();
            } else {
                qDebug() << Q_FUNC_INFO << "Lock already present, bailing out";
                emit m_syncer->webdavAlreadyLocked();
            }
        } else {
            qWarning() << Q_FUNC_INFO << reply->error() << reply->errorString();
            emit m_syncer->webdavAlreadyLocked();
        }

        reply->deleteLater();
        m_buffer->deleteLater();
        m_buffer = 0;
    }

    void onLockAcquired()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        if (reply->error() == 0) {
            emit m_syncer->lockAcquired();
        } else {
            qWarning() << Q_FUNC_INFO << reply->error() << reply->errorString();
            emit m_syncer->webdavAlreadyLocked();
        }

        reply->deleteLater();
    }
private:
    QByteArray m_lockFileContents;
    QBuffer *m_buffer;
};

class DownloadDataState : public SyncState
{
public:
    DownloadDataState(WebDAVSyncer *syncer)
        : SyncState(syncer)
        , m_buffer(0)
    {
        setObjectName("DownloadDataState");
    }

    void onEntry(QEvent *event)
    {
        SyncState::onEntry(event);
        qDebug() << "Entered DownloadDataState";
        m_data.clear();
        m_buffer = new QBuffer(&m_data);
        m_buffer->open(QIODevice::WriteOnly);
        QNetworkReply *reply = m_syncer->m_webdav->get("/" + m_syncer->m_config.webDAVFileName(), m_buffer);
        connect(reply, &QNetworkReply::finished, this, &DownloadDataState::onDataDownloadFinished);
    }

private:
    template <typename T>
    static GenericListModel<T> merge(Kernel *kernel,
                                     GenericListModel<T> &localList,
                                     GenericListModel<T> &serverList)
    {
        Storage *storage = kernel->storage();
        GenericListModel<T> finalList;

#ifdef UNIT_TEST_RUN
        qDebug();
        foreach (const T &t, localList) {
            qDebug() << "Local: " << t;
        }

        foreach (const T &t, serverList) {
            qDebug() << "Remote: " << t;
        }
        qDebug();
#endif

        // Case 1: Present locally, not present on server
        const GenericListModel<T> localTasksCopy = localList; // GenericListModel doesn't let use use iterators
        for (int i = 0; i < localTasksCopy.count(); ++i) {
            T localItem = localTasksCopy.at(i);
            if (localItem->revisionOnWebDAVServer() == -1) {
                // This is a new local task/tag that should be created on server
                int index = Storage::indexOfItem(serverList, localItem);
                if (index != -1) {
                    T serverItem = serverList.at(index);
                    if (serverItem->uuid() != localItem->uuid()) {
                        // In this situation two clients create the same tag concurrently, they have diff uid
                        // so use uuid from the server
                        localItem->setUuid(serverItem->uuid());
                    }
                    serverList.removeAt(index); // Shouldn't be necessary, but just in case
                }

                finalList << localItem;
                localList.removeAll(localItem);
            } else if (!Storage::itemListContains(serverList, localItem)) {
                // This task/tag was deleted on server, should be deleted here.
                // It has revisionOnWebDAVServer != -1, so it was known by the server at some point
                localList.removeAll(localItem);
            } else {
                // In this case task/tag is present in both server and local
                int index = Storage::indexOfItem(serverList, localItem);
                Q_ASSERT(index != -1);
                int localRevision = localItem->revision();
                T serverItem = serverList.at(index);
                int serverRevision = serverItem->revision();
                if (localRevision == serverRevision) {
                    finalList << serverItem;
                } else if (localRevision > serverRevision) {
                    finalList << localItem;
                } else {
                    finalList << serverItem;
                }

                localList.removeAll(localItem);
                while (index != -1) {
                    // Server can have duplicates
                    serverList.removeAt(index);
                    index = Storage::indexOfItem(serverList, localItem);
                }
            }
        }

        Q_ASSERT(localList.isEmpty());

        // New tasks/tags on server, not locally
        for (int i = 0; i < serverList.count(); ++i) {
            T serverItem = serverList.at(i);
            if (storage->data().deletedItemUids.contains(serverItem->uuid())) {
                storage->data().deletedItemUids.removeAll(serverItem->uuid());
            } else if (!Storage::itemListContains(finalList, serverItem)) {
                finalList << serverItem;
            }
        }

        return finalList;
    }

    void onDataDownloadFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        reply->deleteLater();
        m_buffer->deleteLater();
        m_buffer = 0;

        if (reply->error() != 0 && reply->error() != QNetworkReply::ContentNotFoundError) {
            qWarning() << Q_FUNC_INFO << "Download error" << reply->error() << reply->errorString();
            emit m_syncer->downloadError();
        }

        Storage::Data serverData;
        if (reply->error() == 0) {
            QString errorMsg;
            serverData = JsonStorage::deserializeJsonData(m_data, errorMsg, 0);
            if (!errorMsg.isEmpty()) {
                emit m_syncer->downloadError();
                return;
            }
        }

        Storage::Data finalData;

        Storage *storage = m_syncer->m_storage;
        TagList localTags = storage->tags();
        TaskList localTasks = storage->tasks();
        qDebug() << Q_FUNC_INFO << "Merging" << m_syncer->m_kernel->storage()->objectName();
        finalData.tags = merge<Tag::Ptr>(m_syncer->m_kernel, localTags, serverData.tags);
        finalData.tasks = merge<Task::Ptr>(m_syncer->m_kernel, localTasks, serverData.tasks);
        finalData.instanceId = storage->data().instanceId;

        storage->setData(finalData);
        QByteArray newData = JsonStorage::serializeToJsonData(storage->data()); // should be the same as finalData
        QNetworkReply *reply2 = m_syncer->m_webdav->put("/" + m_syncer->m_config.webDAVFileName(), newData);
        connect(reply2, &QNetworkReply::finished, this, &DownloadDataState::onUploadFinished);
    }

    void onUploadFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        reply->deleteLater();
        qDebug() << Q_FUNC_INFO;
        if (reply->error() == 0) {
            foreach (const Task::Ptr &task, m_syncer->m_storage->tasks()) {
                task->setRevisionOnWebDAVServer(task->revision());
            }

            foreach (const Tag::Ptr &tag, m_syncer->m_storage->tags()) {
                tag->setRevisionOnWebDAVServer(tag->revision());
            }

            m_syncer->m_storage->scheduleSave();
            emit m_syncer->uploadFinished();
        } else {
            qWarning() << Q_FUNC_INFO << "Upload error" << reply->error() << reply->errorString();
            emit m_syncer->uploadError();
        }
    }

private:
    QByteArray m_data;
    QBuffer *m_buffer;
};

class CleanupState : public SyncState
{
public:
    CleanupState(WebDAVSyncer *syncer) : SyncState(syncer)
    {
        setObjectName("CleanupState");
    }

    void onEntry(QEvent *event)
    {
        SyncState::onEntry(event);
        qDebug() << "Entered CleanupState";
        QNetworkReply *reply = m_syncer->m_webdav->remove(lockFileName());
        connect(reply, &QNetworkReply::finished, this, &CleanupState::onLockRemoved);
    }

    void onLockRemoved()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        reply->deleteLater();
        qDebug() << Q_FUNC_INFO;
        emit m_syncer->cleanupFinished();
        emit m_syncer->syncFinished(true, QString());
    }
};

class TestSettingsState : public SyncState
{
    // Tests if we can write to dav server with these settings
public:
    TestSettingsState(WebDAVSyncer *syncer) : SyncState(syncer)
    {
        setObjectName("TestSettingsState");
    }

    void onEntry(QEvent *event)
    {
        SyncState::onEntry(event);
        qDebug() << "Entered TestSettingsState";
        QNetworkReply *reply = m_syncer->m_webdav->put("/testingSettings", "");
        connect(reply, &QNetworkReply::finished, this, &TestSettingsState::onUploadFinished);
    }

    void onUploadFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        qDebug() << Q_FUNC_INFO;
        if (reply->error() == 0) {
            QNetworkReply *reply2 = m_syncer->m_webdav->remove("/testingSettings");
            connect(reply2, &QNetworkReply::finished, this, &TestSettingsState::onCleanupFinished);
        } else {
            emit m_syncer->testSettingsFinished(false, reply->errorString());
        }
        reply->deleteLater();
    }

    void onCleanupFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        qDebug() << Q_FUNC_INFO;
        if (reply->error() == 0)
            emit m_syncer->testSettingsFinished(true, QString()); // reply->errorString is "Unknown error" if success
        else
            emit m_syncer->testSettingsFinished(false, reply->errorString());
        reply->deleteLater();
    }
};


WebDAVSyncer::WebDAVSyncer(Kernel *kernel)
    : QObject(kernel)
    , m_webdav(0)
    , m_stateMachine(new QStateMachine(this))
    , m_storage(kernel->storage())
    , m_syncInProgress(false)
    , m_config(kernel->runtimeConfiguration())
    , m_kernel(kernel)
{
    m_initialState = new InitialState(this);
    QState *acquireLockState = new AcquireLockState(this);
    QState *downloadState = new DownloadDataState(this);
    QState *cleanupState = new CleanupState(this);
    QState *testSettingsState = new TestSettingsState(this);
    m_stateMachine->addState(m_initialState);
    m_stateMachine->addState(acquireLockState);
    m_stateMachine->addState(downloadState);
    m_stateMachine->addState(cleanupState);
    m_stateMachine->addState(testSettingsState);

    m_initialState->addTransition(this, SIGNAL(startSync()), acquireLockState); // Try to acquire the lock
    m_initialState->addTransition(this, SIGNAL(testSettingsStarted()), testSettingsState);
    acquireLockState->addTransition(this, SIGNAL(webdavAlreadyLocked()), m_initialState); // We can't lock, go back to beginning
    acquireLockState->addTransition(this, SIGNAL(lockAcquired()), downloadState);
    downloadState->addTransition(this, SIGNAL(downloadError()), cleanupState);
    downloadState->addTransition(this, SIGNAL(uploadError()), cleanupState);
    downloadState->addTransition(this, SIGNAL(uploadFinished()), cleanupState);
    cleanupState->addTransition(this, SIGNAL(cleanupFinished()), m_initialState);
    testSettingsState->addTransition(this, SIGNAL(testSettingsFinished(bool,QString)), m_initialState);

    m_stateMachine->setInitialState(m_initialState);
    m_stateMachine->setObjectName("State Machine");
    m_stateMachine->start();
}

bool WebDAVSyncer::syncInProgress() const
{
    return m_syncInProgress;
}

void WebDAVSyncer::setConnectionSettings(bool https, int port,
                                         const QString &host,
                                         const QString &path,
                                         const QString &user,
                                         const QString &password)
{
    // qDebug() << Q_FUNC_INFO;
    if (m_webdav)
        m_webdav->deleteLater();
    m_webdav = new QWebdav(this);
    m_webdav->setConnectionSettings(https ? QWebdav::HTTPS : QWebdav::HTTP, host,
                                    path, user, password, port);
}

void WebDAVSyncer::upload(const QString &filename, const QByteArray &contents)
{
    Q_ASSERT(m_stateMachine->configuration().contains(m_initialState));
    QNetworkReply *reply = m_webdav->put("/" + filename, contents);
    connect(reply, &QNetworkReply::finished, this, &WebDAVSyncer::onUploadFinished);
}

void WebDAVSyncer::remove(const QString &filename)
{
    Q_ASSERT(m_stateMachine->configuration().contains(m_initialState));
    Q_ASSERT(m_webdav);
    QNetworkReply *reply = m_webdav->remove("/" + filename);
    connect(reply, &QNetworkReply::finished, this, &WebDAVSyncer::onRemoveFinished);
}

void WebDAVSyncer::onUploadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << Q_FUNC_INFO;
    if (reply->error() == 0) {
        emit uploadFinished(true, QString());
    } else {
        emit uploadFinished(false, reply->errorString());
    }
    reply->deleteLater();
}

void WebDAVSyncer::onRemoveFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << Q_FUNC_INFO;
    if (reply->error() == 0) {
        emit removeFinished(true, QString());
    } else {
        emit removeFinished(false, reply->errorString());
    }
    reply->deleteLater();
}

void WebDAVSyncer::sync()
{
    if (!m_webdav) {
        qWarning() << "m_webdav is null";
        Q_ASSERT(false);
        return;
    }

    if (!m_storage->savingInProgress() && !m_storage->loadingInProgress()) {
        emit startSync();
    } else {
        qWarning() << "Will not sync while saving or loading";
    }
}

void WebDAVSyncer::testSettings()
{
    if (!m_webdav) {
        qWarning() << "m_webdav is null";
        Q_ASSERT(false);
        return;
    }

    if (syncInProgress()) {
        qWarning() << Q_FUNC_INFO << "Sync is in progress";
        return;
    }

    // Delayed, because on unit-tests the transition wouldn't be triggered, since we need one event loop processing to get in InitialState
    QMetaObject::invokeMethod(this, "testSettingsStarted", Qt::QueuedConnection);
}
