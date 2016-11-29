/*
  This file is part of Flow.

  Copyright (C) 2016 Sérgio Martins <iamsergio@gmail.com>

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

#include "gitupdater.h"
#include "gitutils.h"
#include "kernel.h"

#include <QThread>
#include <QDebug>

enum {
    PushInterval = 1000 * 60, // 1 minute
    RetryInterval = 1000 * 5 // 5 seconds
};

class WorkerThread : public QThread
{
public:
    explicit WorkerThread(const QString &flowDir)
        : m_flowDir(flowDir)
        , m_gitRepo(flowDir)
    {
    }

    QString errorMsg() const
    {
        return m_errorMsg;
    }

protected:
    void run() override
    {
        bool errorOccurred = false;
        if (!m_gitRepo.hasModifications(m_flowDir, errorOccurred)) {
            if (errorOccurred)
                m_errorMsg = QStringLiteral("git: Error checking for changes");

            return;
        }

        if (!m_gitRepo.stage(m_flowDir)) {
            m_errorMsg = QStringLiteral("git: Error staging changes");
            return;
        }

        if (!m_gitRepo.commit(QStringLiteral("flow auto update"))) {
            m_errorMsg = QStringLiteral("git: Error commiting changes");
            return;
        }

        if (!m_gitRepo.push()) {
            m_errorMsg = QStringLiteral("git: Error pushing changes");
            return;
        }
    }
private:
    QString m_errorMsg;
    QString m_flowDir;
    GitUtils::GitRepo m_gitRepo;
};

GitUpdater::GitUpdater(Kernel *kernel, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
{
    m_pushTimer.connect(&m_pushTimer, &QTimer::timeout, this, &GitUpdater::push);
    m_pushTimer.setSingleShot(true);
}

void GitUpdater::schedulePush()
{
    if (m_isPushing) {
        // Schedule later, we're already busy pushing
        QTimer::singleShot(RetryInterval, this, &GitUpdater::schedulePush);
    } else {
        m_pushTimer.start(PushInterval);
    }
}

void GitUpdater::push()
{
    setIsPushing(true);
    auto workerThread = new WorkerThread(m_kernel->runtimeConfiguration().flowDir());
    connect(workerThread, &QThread::finished, [this, workerThread] {
        const QString errorMsg = workerThread->errorMsg();
        if (!errorMsg.isEmpty())
            qWarning() << errorMsg;

        workerThread->deleteLater();
        setIsPushing(false);
    });

    workerThread->start();
}

bool GitUpdater::isPushing() const
{
    return m_isPushing;
}

void GitUpdater::setIsPushing(bool pushing)
{
    if (pushing != m_isPushing) {
        m_isPushing = pushing;
        emit pushingChanged();
    }
}
