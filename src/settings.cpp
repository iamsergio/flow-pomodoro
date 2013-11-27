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

#include "settings.h"

#include <QMetaType>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>

static Settings *s_instance = 0;

Settings::Settings(QObject *parent) : QObject(parent)
{
    qRegisterMetaTypeStreamOperators<Task>("Task");
    qRegisterMetaTypeStreamOperators<Task::List>("Task::List");
    m_settings = new QSettings(QLatin1String("KDAB"), QLatin1String("flow-pomodoro"), this);
}


Settings *Settings::instance()
{
    if (!s_instance) {
        s_instance = new Settings(qApp);
    }

    return s_instance;
}

Task::List Settings::tasks() const
{
    QVariant v = m_settings->value("tasks");
    Task::List tasks = v.value<Task::List>();
    return tasks;
}

void Settings::saveTasks(const Task::List &tasks)
{
    QVariant v = QVariant::fromValue(tasks);
    m_settings->setValue("tasks", v);
    m_settings->sync();
    /*
    foreach (const Task &task, tasks) {
        qDebug() << "Saving task: " << task.text;
    }*/
}

QDataStream &operator<<(QDataStream &out, const Task &task)
{
    out << task.text;
    return out;
}

QDataStream &operator>>(QDataStream &in, Task &task)
{
    in >> task.text;
    return in;
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    m_settings->setValue(key, value);
    m_settings->sync();
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings->value(key, defaultValue);
}
