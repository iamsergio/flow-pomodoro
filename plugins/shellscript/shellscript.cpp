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

#include <QDBusConnection>
#include <QDBusMessage>
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

    QString scriptName = "shell_script_plugin";
#ifdef Q_OS_WIN
    scriptName += ".bat";
#else
    scriptName += ".sh";
#endif

    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    scriptName = directory + "/" + scriptName;

    QFile file(scriptName);
    if (!file.exists()) {
        qDebug() << "ShellScriptPlugin: file doesn't exist" << scriptName;
        return;
    }

    QStringList args;
    args << (allowDistractions ? "allow" : "disallow");

    RunScriptTask *task = new RunScriptTask(scriptName, args, &m_mutex);
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
