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

#include <QObject>

#ifndef NO_WEBDAV

class QWebdav;
class QStateMachine;
class AcquireLockState;
class DownloadDataState;
class CleanupState;
class Storage;
class Controller;

class WebDAVSyncer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool syncInProgress READ syncInProgress NOTIFY syncInProgressChanged)
public:
    explicit WebDAVSyncer(Storage *storage, Controller *controller);
    bool syncInProgress() const;

public Q_SLOTS:
    void sync();

Q_SIGNALS:
    void startSync();
    void syncFinished(bool success, const QString &errorMessage);
    void webdavAlreadyLocked();
    void lockAcquired();
    void downloadError();
    void uploadError();
    void uploadFinished();
    void cleanupFinished();
    void syncInProgressChanged();

private:
    QWebdav *m_webdav;
    QStateMachine *m_stateMachine;
    Storage *m_storage;
    Controller *m_controller;
    bool m_syncInProgress;

    friend class InitialState;
    friend class AcquireLockState;
    friend class DownloadDataState;
    friend class CleanupState;
};

#endif // NO_WEBDAV

#endif
