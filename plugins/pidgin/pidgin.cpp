/*
  This file is part of Flow.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

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

#include "pidgin.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

PidginPlugin::PidginPlugin() : QObject(), PluginInterface()
  , m_enabled(false)
{
}

void PidginPlugin::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        update(m_enabled);
    }
}

bool PidginPlugin::enabled() const
{
    return m_enabled;
}

void PidginPlugin::update(bool enable)
{
    setLastError("");
    QDBusMessage message = QDBusMessage::createMethodCall("im.pidgin.purple.PurpleService", "/im/pidgin/purple/PurpleObject", "", "PurplePrefsSetBool");
    message << "/pidgin/docklet/change_icon_on_unread" << (enable ? 1 : 0);
    const bool queued = QDBusConnection::sessionBus().send(message);
    if (!queued) {
        setLastError(tr("Could not queue D-Bus message for pidgin"));
    }
}

void PidginPlugin::setTaskStatus(TaskStatus status)
{
    if (m_enabled) {
        update(status != TaskStarted);
    }
}

QString PidginPlugin::text() const
{
    return QStringLiteral("Pidgin Notifications");
}

QString PidginPlugin::helpText() const
{
    return tr("Disables pidgin systray notifications. You need a patched pidgin, see FAQ.");
}

QObject *PidginPlugin::controller()
{
    return this;
}

void PidginPlugin::setQmlEngine(QQmlEngine *)
{

}

QQuickItem *PidginPlugin::configureItem() const
{
    return 0;
}

void PidginPlugin::setLastError(const QString &lastError)
{
    if (!lastError.isEmpty())
        qWarning() << "PidginPlugin:" << lastError;
    if (lastError != m_lastError) {
        m_lastError = lastError;
        emit lastErrorChanged();
    }
}

QString PidginPlugin::lastError() const
{
    return m_lastError;
}
