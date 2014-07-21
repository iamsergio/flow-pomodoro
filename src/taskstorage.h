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

#ifndef TASKSTORAGE_H
#define TASKSTORAGE_H

#include "task.h"
#include "genericlistmodel.h"

#include <QObject>
#include <QTimer>

class TaskFilterProxyModel;
class TagStorage;

class TaskStorage : public QObject
{
    Q_PROPERTY(TaskFilterProxyModel* stagedTasksModel READ stagedTasksModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* taskFilterModel READ taskFilterModel CONSTANT)
    Q_OBJECT
public:
    enum ModelRole {
        TaskRole = Qt::UserRole + 1, // for qml
        TaskPtrRole
    };

    static TaskStorage *instance();

    void setTasks(const Task::List &);
    void saveTasks();

    TaskFilterProxyModel* stagedTasksModel() const;
    TaskFilterProxyModel* taskFilterModel() const;

    Task::Ptr at(int proxyIndex) const;
    void addTask(const QString &taskText);
    void removeTask(int proxyIndex);
    int indexOf(const Task*) const;

    // Temporary disable saving. For performance purposes
    void setDisableSaving(bool);

public Q_SLOTS:
    void dumpDebugInfo();

Q_SIGNALS:
    void taskRemoved();
    void taskAdded();
    void taskChanged();

protected:
    explicit TaskStorage(QObject *parent = 0);
    GenericListModel<Task::Ptr> m_tasks;
    void loadTasks();
    virtual void saveTasks_impl() = 0;
    virtual void loadTasks_impl() = 0;

private Q_SLOTS:
    void onTagAboutToBeRemoved(const QString &tagName);
    void scheduleSaveTasks();

private:
    int proxyRowToSource(int proxyIndex) const;
    TaskFilterProxyModel *m_taskFilterModel;
    TaskFilterProxyModel *m_stagedTasksModel;
    TagStorage *m_tagStorage;
    QTimer m_scheduleTimer;
    bool m_savingDisabled;
};

#endif
