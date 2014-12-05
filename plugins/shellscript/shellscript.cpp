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

#include "shellscript.h"

#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>

class RunScriptTask : public QRunnable
{
public:
    RunScriptTask(const QString &filename, const QStringList &args, QMutex *mutex)
        : m_filename(filename)
        , m_args(args)
        , m_mutex(mutex)
    {
    }

private:
    void run()
    {
        QMutexLocker locker(m_mutex);
        QProcess *process = new QProcess();
        process->start(m_filename, m_args);
        process->waitForFinished();
        delete process;
    }

    QString m_filename;
    QStringList m_args;
    QMutex *m_mutex;
};

ShellScriptPlugin::ShellScriptPlugin() : QObject(), PluginInterface()
  , m_enabled(false)
  , m_allowingDistractions(true)
{
    m_scriptName = "shell_script_plugin";
#ifdef Q_OS_WIN
    m_scriptName += ".bat";
#else
    m_scriptName += ".sh";
#endif

    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    m_scriptName = directory + "/" + m_scriptName;
}

void ShellScriptPlugin::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        update(m_enabled);
    }
}

bool ShellScriptPlugin::enabled() const
{
    return m_enabled;
}

void ShellScriptPlugin::update(bool allowDistractions)
{
    if (allowDistractions == m_allowingDistractions)
        return;

    QFile file(m_scriptName);
    if (!file.exists()) {
        setLastError(tr("File doesn't exist: %1").arg(m_scriptName));
        return;
    }

    QStringList args;
    args << (allowDistractions ? "allow" : "disallow");

    RunScriptTask *task = new RunScriptTask(m_scriptName, args, &m_mutex);
    QThreadPool::globalInstance()->start(task);
}

void ShellScriptPlugin::setTaskStatus(TaskStatus status)
{
    if (m_enabled) {
        update(status != TaskStarted);
    }
}

QString ShellScriptPlugin::text() const
{
    return QStringLiteral("Shell script");
}

QString ShellScriptPlugin::helpText() const
{
    return tr("Executes a shell script to enable/disable distractions.\nYou must create or edit <b>%1</b>. The first argument passed to the script will be <b>allow</b> or <b>disallow</b>.").arg(m_scriptName);
}

QObject *ShellScriptPlugin::controller()
{
    return this;
}

void ShellScriptPlugin::setLastError(const QString &lastError)
{
    qWarning() << Q_FUNC_INFO << lastError;
    if (lastError != m_lastError) {
        m_lastError = lastError;
        emit lastErrorChanged();
    }
}

QString ShellScriptPlugin::lastError() const
{
    return m_lastError;
}
