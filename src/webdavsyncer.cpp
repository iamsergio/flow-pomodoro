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
        qDebug() << "Entered InitialState, where we idle.";
        m_syncer->m_syncInProgress = false;
        m_syncer->m_storage->setCreateNonExistentTags(false);
        emit m_syncer->syncInProgressChanged();
    }

    void onExit(QEvent *event)
    {
        SyncState::onExit(event);
        m_syncer->m_syncInProgress = true;
        m_syncer->m_storage->setCreateNonExistentTags(true);
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
        QNetworkReply *reply = m_syncer->m_webdav->get("/flow.lock");
        connect(reply, &QNetworkReply::finished, this, &AcquireLockState::onDownloadLockFileFinished);
    }
private:
    void onDownloadLockFileFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

        if (reply->error() == QNetworkReply::ContentNotFoundError) {
            QNetworkReply *reply2 = m_syncer->m_webdav->put("/flow.lock", QByteArray("empty"));
            connect(reply2, &QNetworkReply::finished, this, &AcquireLockState::onLockAcquired);
        } else if (reply->error() == 0) {
            qDebug() << Q_FUNC_INFO << "Lock already present, bailing out";
            emit m_syncer->webdavAlreadyLocked();
        } else {
            // TODO Change state
            qWarning() << Q_FUNC_INFO << reply->error() << reply->errorString();
            emit m_syncer->webdavAlreadyLocked();
        }

        reply->deleteLater();
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
        QNetworkReply *reply = m_syncer->m_webdav->get("/flow.dat", m_buffer);
        connect(reply, &QNetworkReply::finished, this, &DownloadDataState::onDataDownloadFinished);
    }

private:
    static bool taskListContains(const TaskList &tasks, const Task::Ptr &task)
    {
        return indexOfTask(tasks, task) != -1;
    }

    static bool tagListContains(const TagList &tags, const Tag::Ptr &tag)
    {
        foreach (const Tag::Ptr &t, tags) {
            if (t->name() == tag->name())
                return true;
        }

        return false;
    }

    static int indexOfTask(const TaskList &tasks, const Task::Ptr &task)
    {
        for (int i = 0; i < tasks.count(); i++)
            if (tasks.at(i)->uuid() == task->uuid())
                return i;

        return -1;
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
            serverData = JsonStorage::deserializeJsonData(m_data, errorMsg);
            if (!errorMsg.isEmpty()) {
                emit m_syncer->downloadError();
                return;
            }
        }

        qDebug() << Q_FUNC_INFO << "Merging";
        Storage::Data finalData;
        Storage *storage = m_syncer->m_storage;
        TaskList localTasks = storage->tasks();
        TagList localTags = storage->tags();

        // TODO: Sync tags. For now copy all
        finalData.tags = localTags;
        for (int i = 0; i < serverData.tags.count(); ++i) {
            Tag::Ptr serverTag = serverData.tags.at(i);
            if (!finalData.tags.contains(serverTag))
                finalData.tags << serverTag;
        }

        // Case 1: Present locally, not present on server
        const TaskList localTasksCopy = localTasks; // GenericListModel doesn't let use use iterators
        for (int i = 0; i < localTasksCopy.count(); ++i) {
            Task::Ptr localTask = localTasksCopy.at(i);
            if (localTask->revisionOnWebDAVServer() == -1) {
                // This is a new local task that should be created on server
                finalData.tasks << localTask;
                localTasks.removeAll(localTask);
                int index = indexOfTask(serverData.tasks, localTask);
                if (index != -1)
                    serverData.tasks.removeAt(index); // Shouldn't be necessary, but just in case
            } else if (!taskListContains(serverData.tasks, localTask)) {
                // This task was deleted on server, should be deleted here.
                // It has revisionOnWebDAVServer != -1, so it was known by the server at some point
                localTasks.removeAll(localTask);
            } else {
                // In this case task is present in both server and local
                int index = indexOfTask(serverData.tasks, localTask);
                Q_ASSERT(index != -1);
                int localRevision = localTask->revision();
                Task::Ptr serverTask = serverData.tasks.at(index);
                int serverRevision = serverTask->revision();

                if (localRevision == serverRevision) {
                    finalData.tasks << localTask;
                } else if (localRevision > serverRevision) {
                    finalData.tasks << localTask;
                } else {
                    finalData.tasks << serverTask;
                }

                localTasks.removeAll(localTask);
                while (index != -1) {
                    // Server can have duplicates
                    serverData.tasks.removeAt(index);
                    index = indexOfTask(serverData.tasks, localTask);
                }
            }
        }

        Q_ASSERT(localTasks.isEmpty());

        // New tasks on server, not locally
        for (int i = 0; i < serverData.tasks.count(); ++i) {
            Task::Ptr serverTask = serverData.tasks.at(i);
            if (storage->data().deletedTasksUids.contains(serverTask->uuid())) {
                storage->data().deletedTasksUids.removeAll(serverTask->uuid());
            } else {
                finalData.tasks << serverTask;
            }
        }

        storage->setData(finalData);
        QByteArray newData = JsonStorage::serializeToJsonData(storage->data()); // should be the same as finalData
        QNetworkReply *reply2 = m_syncer->m_webdav->put("/flow.dat", newData);
        connect(reply2, &QNetworkReply::finished, this, &DownloadDataState::onUploadFinished);
    }

    void onUploadFinished()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        reply->deleteLater();
        qDebug() << Q_FUNC_INFO;
        if (reply->error() == 0) {

            TaskList tasks = m_syncer->m_storage->tasks();
            for (int i = 0; i < tasks.count(); ++i) {
                Task::Ptr task = tasks.at(i);
                task->setRevisionOnWebDAVServer(task->revision());
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
        QNetworkReply *reply = m_syncer->m_webdav->remove("/flow.lock");
        connect(reply, &QNetworkReply::finished, this, &CleanupState::onLockRemoved);
    }

    void onLockRemoved()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        reply->deleteLater();
        qDebug() << Q_FUNC_INFO;
        // TODO: Error removing lock ? and lock with our id
        emit m_syncer->cleanupFinished();
    }
};


WebDAVSyncer::WebDAVSyncer(Storage *parent)
    : QObject(parent)
    , m_webdav(new QWebdav(this))
    , m_stateMachine(new QStateMachine(this))
    , m_storage(parent)
    , m_syncInProgress(false)
{
    // TODO: Setup connection

    QState *initialState = new InitialState(this);
    QState *acquireLockState = new AcquireLockState(this);
    QState *downloadState = new DownloadDataState(this);
    QState *cleanupState = new CleanupState(this);
    m_stateMachine->addState(initialState);
    m_stateMachine->addState(acquireLockState);
    m_stateMachine->addState(downloadState);
    m_stateMachine->addState(cleanupState);

    initialState->addTransition(this, SIGNAL(startSync()), acquireLockState); // Try to acquire the lock
    acquireLockState->addTransition(this, SIGNAL(webdavAlreadyLocked()), initialState); // We can't lock, go back to beginning
    acquireLockState->addTransition(this, SIGNAL(lockAcquired()), downloadState);
    downloadState->addTransition(this, SIGNAL(downloadError()), cleanupState);
    downloadState->addTransition(this, SIGNAL(uploadError()), cleanupState);
    downloadState->addTransition(this, SIGNAL(uploadFinished()), cleanupState);
    cleanupState->addTransition(this, SIGNAL(cleanupFinished()), initialState);

    m_stateMachine->setInitialState(initialState);
    m_stateMachine->setObjectName("State Machine");
    m_stateMachine->start();
}

bool WebDAVSyncer::syncInProgress() const
{
    return m_syncInProgress;
}

void WebDAVSyncer::sync()
{
    if (!m_storage->savingInProgress() && !m_storage->loadingInProgress()) {
        emit startSync();
    } else {
        qWarning() << "Will not sync while saving or loading";
    }
}
