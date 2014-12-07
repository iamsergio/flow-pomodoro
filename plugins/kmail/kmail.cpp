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

#include "kmail.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

KMailPlugin::KMailPlugin() : QObject(), PluginInterface()
  , m_enabled(false)
{
}

void KMailPlugin::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        update(enabled);
    }
}

bool KMailPlugin::enabled() const
{
    return m_enabled;
}

void KMailPlugin::setSystrayIconsNotificationsEnabled(bool enable)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.kmail2", "/KMail", "", "setSystrayIconNotificationsEnabled");
    message << enable;

    const bool queued = QDBusConnection::sessionBus().send(message);
    if (!queued) {
        setLastError(tr("Could not queue D-Bus message"));
    }
}

void KMailPlugin::setNewMailAgentEnabled(bool enable)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.Akonadi.NewMailNotifierAgent", "/NewMailNotifierAgent", "", "setEnableAgent");
    message << enable;

    const bool queued = QDBusConnection::sessionBus().send(message);
    if (!queued) {
        setLastError(tr("Could not queue D-Bus message for new mail agent"));
    }
}

void KMailPlugin::update(bool enable)
{
    setLastError("");
    setSystrayIconsNotificationsEnabled(enable);
    setNewMailAgentEnabled(enable);
}

void KMailPlugin::setTaskStatus(TaskStatus status)
{
    if (m_enabled) {
        update(status != TaskStarted);
    }
}

QString KMailPlugin::text() const
{
    return QStringLiteral("KMail Notifications");
}

QString KMailPlugin::helpText() const
{
    return tr("Disables KMail systray notifications and notifier agent popups.");
}

QObject *KMailPlugin::controller()
{
    return this;
}

void KMailPlugin::setQmlEngine(QQmlEngine *)
{

}

QQuickItem *KMailPlugin::configureItem() const
{
    return 0;
}

void KMailPlugin::setLastError(const QString &lastError)
{
    if (!lastError.isEmpty())
        qWarning() << "KMailPlugin:" << lastError;
    if (lastError != m_lastError) {
        m_lastError = lastError;
        emit lastErrorChanged();
    }
}

QString KMailPlugin::lastError() const
{
    return m_lastError;
}
