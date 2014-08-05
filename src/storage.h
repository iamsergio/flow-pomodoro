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

#ifndef FLOW_STORAGE_H
#define FLOW_STORAGE_H

#include "task.h"
#include "tag.h"
#include "genericlistmodel.h"

#include <QTimer>
#include <QObject>

class SortedTagsModel;
class ArchivedTasksFilterModel;
class TaskFilterProxyModel;

typedef GenericListModel<Tag::Ptr> TagList;
typedef GenericListModel<Task::Ptr> TaskList;

class Storage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* tagsModel READ tagsModel CONSTANT)
    Q_PROPERTY(ArchivedTasksFilterModel* stagedTasksModel READ stagedTasksModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* taskFilterModel READ taskFilterModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* untaggedTasksModel READ untaggedTasksModel CONSTANT)
public:
    enum TagModelRole {
        TagRole = Qt::UserRole + 1,
        TagPtrRole,
        LastRole
    };

    enum TaskModelRole {
        TaskRole = Qt::UserRole + 1,
        TaskPtrRole
    };

    static Storage *instance();

    TagList tags() const;
    TaskList tasks() const;

    void load();
    void save();

    void scheduleSave();
    // Temporary disable saving. For performance purposes
    void setDisableSaving(bool);
//------------------------------------------------------------------------------
//Stuff fot tasks
    TaskFilterProxyModel* taskFilterModel() const;
    TaskFilterProxyModel* untaggedTasksModel() const;
    ArchivedTasksFilterModel* stagedTasksModel() const;
    ArchivedTasksFilterModel* archivedTasksModel() const;
    Task::Ptr taskAt(int proxyIndex) const;
    Task::Ptr addTask(const QString &taskText);
    void removeTask(int proxyIndex);
    int indexOfTask(const Task::Ptr &) const;
//------------------------------------------------------------------------------
// Stuff for tags
    Q_INVOKABLE bool removeTag(const QString &tagName);
    Q_INVOKABLE Tag::Ptr createTag(const QString &tagName);
    Tag::Ptr tag(const QString &name, bool create = true);
    QAbstractItemModel *tagsModel() const;
    QString deletedTagName() const;
    bool containsTag(const QString &name) const;
//------------------------------------------------------------------------------

public Q_SLOTS:
    bool renameTag(const QString &oldName, const QString &newName);
    void dumpDebugInfo();

Q_SIGNALS:
    void tagAboutToBeRemoved(const QString &name);
    void taskRemoved();
    void taskAdded();
    void taskChanged();
private Q_SLOTS:
    void onTagAboutToBeRemoved(const QString &tagName);

protected:
    explicit Storage(QObject *parent = 0);
    Task::Ptr addTask(const Task::Ptr &task);
    TagList m_tags;
    TaskList m_tasks;
    virtual void load_impl() = 0;
    virtual void save_impl() = 0;

private:
    int indexOfTag(const QString &name) const;
    int proxyRowToSource(int proxyIndex) const;
    QTimer m_scheduleTimer;
    SortedTagsModel *m_sortedTagModel;
    QString m_deletedTagName;
    int m_savingDisabled;
    TaskFilterProxyModel *m_taskFilterModel;
    TaskFilterProxyModel *m_untaggedTasksModel;
    ArchivedTasksFilterModel *m_stagedTasksModel;
    ArchivedTasksFilterModel *m_archivedTasksModel;
};

#endif
