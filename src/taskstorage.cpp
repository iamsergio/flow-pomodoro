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

#include "taskstorage.h"
#include "taskstorageqsettings.h"
#include "taskfilterproxymodel.h"
#include "tagstorage.h"

TaskStorage::TaskStorage(QObject *parent)
    : QObject(parent)
    , m_taskFilterModel(new TaskFilterProxyModel(this))
    , m_stagedTasksModel(new TaskFilterProxyModel(this))
    , m_untaggedTasksModel(new TaskFilterProxyModel(this))
    , m_archivedTasksModel(new TaskFilterProxyModel(this))
    , m_tagStorage(TagStorage::instance())
    , m_savingDisabled(false)
{
    m_tagStorage->loadTags();
    connect(m_tagStorage, &TagStorage::tagAboutToBeRemoved,
            this, &TaskStorage::onTagAboutToBeRemoved);

    m_taskFilterModel->setSourceModel(m_tasks);
    m_stagedTasksModel->setSourceModel(m_tasks);
    m_untaggedTasksModel->setSourceModel(m_tasks);
    m_archivedTasksModel->setSourceModel(m_tasks);
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);

    connect(&m_scheduleTimer, &QTimer::timeout, this, &TaskStorage::saveTasks);
    connect(m_tasks, &QAbstractListModel::dataChanged, this, &TaskStorage::scheduleSaveTasks);
    connect(m_tasks, &QAbstractListModel::rowsInserted, this, &TaskStorage::scheduleSaveTasks);
    connect(m_tasks, &QAbstractListModel::rowsRemoved, this, &TaskStorage::scheduleSaveTasks);
    connect(m_tasks, &QAbstractListModel::modelReset, this, &TaskStorage::scheduleSaveTasks);

    m_tasks.insertRole("task", [&](int i) { return QVariant::fromValue<Task*>(m_tasks.at(i).data()); }, TaskRole);
    m_tasks.insertRole("taskPtr", [&](int i) { return QVariant::fromValue<Task::Ptr>(m_tasks.at(i)); }, TaskPtrRole);
    m_stagedTasksModel->setFilterStaged(true);
    m_untaggedTasksModel->setFilterArchived(true);
    m_untaggedTasksModel->setFilterUntagged(true);
    m_archivedTasksModel->setFilterArchived(true);
}

TaskStorage *TaskStorage::instance()
{
    static TaskStorage *storage = new TaskStorageQSettings(qApp);
    return storage;
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

TaskFilterProxyModel *TaskStorage::archivedTasksModel() const
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
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_archivedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
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
    m_savingDisabled = disable;
}

void TaskStorage::dumpDebugInfo()
{
    qDebug() << Q_FUNC_INFO << "task count:" << m_tasks.count();
    for (int i = 0; i < m_tasks.count(); ++i)
        qDebug() << Q_FUNC_INFO << i << m_tasks.at(i)->summary();
}

void TaskStorage::loadTasks()
{
    m_savingDisabled = true;
    loadTasks_impl();
    m_savingDisabled = false;
}

void TaskStorage::onTagAboutToBeRemoved(const QString &tagName)
{
    for (int i = 0; i < m_tasks.count(); ++i)
        m_tasks.at(i)->removeTag(tagName);
}

void TaskStorage::scheduleSaveTasks()
{
    if (!m_savingDisabled)
        m_scheduleTimer.start();
}

void TaskStorage::saveTasks()
{
    m_savingDisabled = true;
    for (int i = 0; i < m_tasks.count(); ++i) {
        Task::Ptr task = m_tasks.at(i);
        if (task->summary().isEmpty()) {
            task->setSummary(tr("New Task"));
        }
    }
    m_savingDisabled = false;

    saveTasks_impl();
}

TaskFilterProxyModel *TaskStorage::stagedTasksModel() const
{
    return m_stagedTasksModel;
}

int TaskStorage::proxyRowToSource(int proxyRow) const
{
    QModelIndex proxyIndex = m_taskFilterModel->index(proxyRow, 0);
    QModelIndex index = m_taskFilterModel->mapToSource(proxyIndex);

    return index.isValid() ? index.row() : -1;
}
