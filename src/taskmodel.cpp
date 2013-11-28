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

#include "taskmodel.h"
#include "settings.h"

#include <QDebug>

TaskModel::TaskModel(QObject *parent) : QAbstractListModel(parent)
{
    populate();
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_tasks.count();
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(TextRole, "name");

    return roles;
}

QVariant TaskModel::data(const QModelIndex &index, int ) const
{
    if (index.row() < 0 || index.row() >= m_tasks.count()) {
        return QVariant();
    }

    const Task task = m_tasks[index.row()];
    return task.text;
}

int TaskModel::count() const
{
    return m_tasks.count();
}

Task TaskModel::at(int index) const
{
    return m_tasks.at(index);
}

void TaskModel::addTask(const QString &taskText)
{
    //qDebug() << "TaskModel::addTask()";
    beginInsertRows(QModelIndex(), m_tasks.count(), m_tasks.count());
    Task task;
    task.text = taskText;
    m_tasks.append(task);
    Settings::instance()->saveTasks(m_tasks);
    endInsertRows();
}

void TaskModel::removeTask(int index)
{
    //qDebug() << "TaskModel::removeTask()";
    beginRemoveRows(QModelIndex(), index, index);
    m_tasks.removeAt(index);
    Settings::instance()->saveTasks(m_tasks);
    endRemoveRows();
}

void TaskModel::updateTask(int i, const QString &newText)
{

    if (!(i>=0 && i<m_tasks.count())) {
        qDebug() << i;
        Q_ASSERT(false);
        return;
    }

    Task task;
    task.text = newText.isEmpty() ? tr("New Task") : newText;
    m_tasks[i] = task;
    emit dataChanged(index(i, 0), index(i, 0));
    Settings::instance()->saveTasks(m_tasks);
}

void TaskModel::populate()
{
    beginResetModel();
    m_tasks = Settings::instance()->tasks();
    /*foreach (const Task &task, m_tasks) {
        qDebug() << "Loaded task: " << task.text;
    }*/

    endResetModel();
}
