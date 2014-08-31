/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "settings.h"
#include "task.h"
#include <QCoreApplication>
#include <QMetaType>

Settings::Settings(QObject *parent)
    : QSettings("KDAB", "flow-pomodoro", parent)
    , m_syncScheduled(false)
    , m_needsSync(false)
{
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    QSettings::setValue(key, value);
    m_needsSync = true;
}

void Settings::scheduleSync()
{
    if (!m_syncScheduled) {
        qDebug() << Q_FUNC_INFO;
        m_syncScheduled = true;
        m_needsSync = false;
        QMetaObject::invokeMethod(this, "doSync", Qt::QueuedConnection);
    }
}

bool Settings::needsSync() const
{
    return m_needsSync;
}

void Settings::doSync()
{
    qDebug() << Q_FUNC_INFO;
    m_syncScheduled = false;
    QSettings::sync();
}
