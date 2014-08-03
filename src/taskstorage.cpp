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

#include "storage.h"
#include "taskstorage.h"
#include "archivedtasksfiltermodel.h"
#include "taskfilterproxymodel.h"
#include "tagstorage.h"

TaskStorage::TaskStorage(QObject *parent)
    : QObject(parent)
    , m_taskFilterModel(new TaskFilterProxyModel(this))
    , m_untaggedTasksModel(new TaskFilterProxyModel(this))
    , m_stagedTasksModel(new ArchivedTasksFilterModel(m_tasks, this))
    , m_archivedTasksModel(new ArchivedTasksFilterModel(m_tasks, this))
    , m_tagStorage(Storage::instance()->tagStorage())
    , m_savingDisabled(0)
{
    connect(m_tagStorage, &TagStorage::tagAboutToBeRemoved,
            this, &TaskStorage::onTagAboutToBeRemoved);

    m_taskFilterModel->setSourceModel(m_tasks);
    m_untaggedTasksModel->setSourceModel(m_archivedTasksModel);
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);

    connect(&m_scheduleTimer, &QTimer::timeout, this, &TaskStorage::saveTasks);
    connect(m_tasks, &QAbstractListModel::dataChanged, this, &TaskStorage::scheduleSaveTasks);
    connect(m_tasks, &QAbstractListModel::rowsInserted, this, &TaskStorage::scheduleSaveTasks);
    connect(m_tasks, &QAbstractListModel::rowsRemoved, this, &TaskStorage::scheduleSaveTasks);
    connect(m_tasks, &QAbstractListModel::modelReset, this, &TaskStorage::scheduleSaveTasks);

    m_tasks.insertRole("task", [&](int i) { return QVariant::fromValue<Task*>(m_tasks.at(i).data()); }, TaskRole);
    m_tasks.insertRole("taskPtr", [&](int i) { return QVariant::fromValue<Task::Ptr>(m_tasks.at(i)); }, TaskPtrRole);
    m_stagedTasksModel->setAcceptArchived(false);
    m_archivedTasksModel->setAcceptArchived(true);
    m_untaggedTasksModel->setFilterUntagged(true);
    m_untaggedTasksModel->setObjectName("Untagged and archived tasks model");
}

void TaskStorage::setTasks(const Task::List &tasks)
{
    m_tasks.clear();
    for (int i = 0; i < tasks.count(); ++i) {
        addTask(tasks.at(i));
    }
}

TaskFilterProxyModel *TaskStorage::taskFilterModel() const
{
    return m_taskFilterModel;
}

TaskFilterProxyModel *TaskStorage::untaggedTasksModel() const
{
    return m_untaggedTasksModel;
}

ArchivedTasksFilterModel *TaskStorage::stagedTasksModel() const
{
    return m_stagedTasksModel;
}

ArchivedTasksFilterModel *TaskStorage::archivedTasksModel() const
{
    return m_archivedTasksModel;
}

Task::Ptr TaskStorage::at(int proxyIndex) const
{
    return m_tasks.value(proxyRowToSource(proxyIndex));
}

Task::Ptr TaskStorage::addTask(const QString &taskText)
{
    Task::Ptr task = Task::createTask(taskText);
    return addTask(task);
}

Task::Ptr TaskStorage::addTask(const Task::Ptr &task)
{
    connect(task.data(), &Task::changed, this,
            &TaskStorage::scheduleSaveTasks, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_stagedTasksModel,
            &ArchivedTasksFilterModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_archivedTasksModel,
            &ArchivedTasksFilterModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::tagsChanged, m_untaggedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);

    m_tasks << task;

    return task;
}

void TaskStorage::removeTask(int proxyIndex)
{
    m_tasks.removeAt(proxyRowToSource(proxyIndex));
}

int TaskStorage::indexOf(const Task::Ptr &task) const
{
    for (int i = 0; i < m_tasks.count(); ++i) {
        if (m_tasks.at(i) == task)
            return i;
    }

    return -1;
}

void TaskStorage::setDisableSaving(bool disable)
{
    m_savingDisabled += (disable ? 1 : -1);
}

GenericListModel<Task::Ptr> TaskStorage::tasks() const
{
    return m_tasks;
}

void TaskStorage::dumpDebugInfo()
{
    qDebug() << Q_FUNC_INFO << "task count:" << m_tasks.count();
    for (int i = 0; i < m_tasks.count(); ++i)
        qDebug() << Q_FUNC_INFO << i << m_tasks.at(i)->summary();
}

void TaskStorage::loadTasks()
{
    m_savingDisabled += 1;
    loadTasks_impl();
    m_savingDisabled -= 1;
}

void TaskStorage::onTagAboutToBeRemoved(const QString &tagName)
{
    for (int i = 0; i < m_tasks.count(); ++i)
        m_tasks.at(i)->removeTag(tagName);
}

void TaskStorage::scheduleSaveTasks()
{
    if (m_savingDisabled == 0)
        m_scheduleTimer.start();
}

void TaskStorage::saveTasks()
{
    qDebug() << Q_FUNC_INFO;
    m_savingDisabled += 1;
    for (int i = 0; i < m_tasks.count(); ++i) {
        Task::Ptr task = m_tasks.at(i);
        if (task->summary().isEmpty()) {
            task->setSummary(tr("New Task"));
        }
    }
    m_savingDisabled -= 1;

    saveTasks_impl();
}

int TaskStorage::proxyRowToSource(int proxyRow) const
{
    QModelIndex proxyIndex = m_taskFilterModel->index(proxyRow, 0);
    QModelIndex index = m_taskFilterModel->mapToSource(proxyIndex);

    return index.isValid() ? index.row() : -1;
}
