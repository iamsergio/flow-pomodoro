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

#ifndef SHELLSCRIPT_PLUGIN_H
#define SHELLSCRIPT_PLUGIN_H

#include "plugininterface.h"
#include "task.h"
#include <QObject>
#include <QProcess>
#include <QMutex>

class ShellScriptPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PLUGIN_METADATA(IID "com.kdab.flow.PluginInterface")
    Q_INTERFACES(PluginInterface)

public:
    ShellScriptPlugin();

    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    bool enabled() const Q_DECL_OVERRIDE;

    void setTaskStatus(TaskStatus status) Q_DECL_OVERRIDE;
    QString text() const Q_DECL_OVERRIDE;
    QString helpText() const Q_DECL_OVERRIDE;
    QObject *controller() Q_DECL_OVERRIDE;
    void setQmlEngine(QQmlEngine *) Q_DECL_OVERRIDE;
    QQuickItem *configureItem() const Q_DECL_OVERRIDE;
    void setSettings(QSettings *) Q_DECL_OVERRIDE;

    QString lastError() const;

public Q_SLOTS:
    void editScript();

Q_SIGNALS:
    void lastErrorChanged();

private:
    bool checkSanity();
    void setLastError(const QString &);
    void update(bool blockDistractions);
    void startProcess(const QString &filename, const QStringList &arguments);
    bool m_enabled;
    QMutex m_mutex;
    bool m_allowingDistractions;
    QString m_scriptName;
    QString m_lastError;
    QQmlEngine *m_qmlEngine;
    QQuickItem *m_configItem;
};

#endif
