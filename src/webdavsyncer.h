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

#ifndef WEBDAVSYNCER_H
#define WEBDAVSYNCER_H

#include "runtimeconfiguration.h"
#include <QObject>

#ifndef NO_WEBDAV

class QWebdav;
class QStateMachine;
class QState;
class AcquireLockState;
class DownloadDataState;
class CleanupState;
class Storage;
class Controller;
class Kernel;

class WebDAVSyncer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool syncInProgress READ syncInProgress NOTIFY syncInProgressChanged)
public:
    explicit WebDAVSyncer(Kernel *kernel);
    bool syncInProgress() const;
    void setConnectionSettings(bool https, int port, const QString &host, const QString &path,
                               const QString &user, const QString &password);

    // Used by unit-tests:
    void upload(const QString &filename, const QByteArray &contents);
    void remove(const QString &filename);

public Q_SLOTS:
    void sync();
    void testSettings();

Q_SIGNALS:
    void startSync();
    void testSettingsStarted();
    void syncFinished(bool success, const QString &errorMessage);
    void webdavAlreadyLocked();
    void lockAcquired();
    void downloadError();
    void uploadError();
    void uploadFinished();
    void cleanupFinished();
    void syncInProgressChanged();
    void testSettingsFinished(bool success, const QString &errorMessage);
    void uploadFinished(bool success, const QString &errorMessage);
    void removeFinished(bool success, const QString &errorMessage);

private Q_SLOTS:
    void onUploadFinished();
    void onRemoveFinished();

private:
    QWebdav *m_webdav;
    QStateMachine *m_stateMachine;
    Storage *m_storage;
    Controller *m_controller;
    bool m_syncInProgress;
    RuntimeConfiguration m_config;
    QState *m_initialState;

    friend class InitialState;
    friend class AcquireLockState;
    friend class DownloadDataState;
    friend class CleanupState;
    friend class TestSettingsState;
    friend class SyncState;
};

#endif // NO_WEBDAV

#endif
