/*
  This file is part of Flow.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "taskstorageqsettings.h"
#include "settings.h"

static const char* GROUP = "tasks";
static const char* TASK = "tasks";

TaskStorageQSettings::TaskStorageQSettings(QObject *parent)
    : TaskStorage(parent)
    , m_settings(Settings::instance())
{
}

TaskStorageQSettings::~TaskStorageQSettings()
{
    saveTasks();
}

void TaskStorageQSettings::loadTasks_impl()
{
    m_settings->beginGroup(GROUP);
    QVariant v = m_settings->value(TASK);
    m_settings->endGroup();
    setTasks(v.value<Task::List>());
}

void TaskStorageQSettings::saveTasks_impl()
{
    // qDebug() << Q_FUNC_INFO << m_tasks.count();
    QVariant v = QVariant::fromValue(m_tasks);
    m_settings->beginGroup(GROUP);
    m_settings->setValue(TASK, v);
    m_settings->sync();
    m_settings->endGroup();
}
