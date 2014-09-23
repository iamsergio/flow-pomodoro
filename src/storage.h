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
#include <QUuid>

class Kernel;
class SortedTagsModel;
class ArchivedTasksFilterModel;
class TaskFilterProxyModel;

typedef GenericListModel<Tag::Ptr> TagList;
typedef GenericListModel<Task::Ptr> TaskList;

enum {
    JsonSerializerVersion1 = 1
};

class Storage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* tagsModel READ tagsModel CONSTANT)
    Q_PROPERTY(ArchivedTasksFilterModel* stagedTasksModel READ stagedTasksModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* taskFilterModel READ taskFilterModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* untaggedTasksModel READ untaggedTasksModel CONSTANT)
    Q_PROPERTY(bool webDAVSyncSupported READ webDAVSyncSupported CONSTANT)

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

    struct Data {
        Data() : serializerVersion(JsonSerializerVersion1) {}
        TaskList tasks;
        TagList tags;
        QStringList deletedItemUids; // so we can sync to server
        int serializerVersion;
        QByteArray instanceId;
    };

    explicit Storage(Kernel *kernel, QObject *parent = 0);
    ~Storage();

    const TagList& tags() const;
    TaskList tasks() const;
    Storage::Data data() const;
    void setData(const Storage::Data &data);

    int serializerVersion() const;

    bool saveScheduled() const;
    void scheduleSave();
    // Temporary disable saving. For performance purposes
    void setDisableSaving(bool);

    void setCreateNonExistentTags(bool);

    bool savingInProgress() const;
    bool loadingInProgress() const;

    bool webDAVSyncSupported() const;

    QByteArray instanceId();
#ifdef DEVELOPER_MODE
    Q_INVOKABLE void removeDuplicateData();
#endif

    template <typename T>
    static inline bool itemListContains(const GenericListModel<T> &list, const T &item)
    {
        return Storage::indexOfItem(list, item) != -1;
    }

    template <typename T>
    static inline int indexOfItem(const GenericListModel<T> &list, const T &item)
    {
        for (int i = 0; i < list.count(); i++)
            if (*list.at(i).data() == *item.data())
                return i;

        return -1;
    }
//------------------------------------------------------------------------------
//Stuff fot tasks
    TaskFilterProxyModel* taskFilterModel() const;
    TaskFilterProxyModel* untaggedTasksModel() const;
    ArchivedTasksFilterModel* stagedTasksModel() const;
    ArchivedTasksFilterModel* archivedTasksModel() const;
    Task::Ptr taskAt(int proxyIndex) const;
    Task::Ptr addTask(const QString &taskText);
    Task::Ptr prependTask(const QString &taskText);
    void removeTask(const Task::Ptr &task);
    int indexOfTask(const Task::Ptr &) const;
    void clearTasks();
//------------------------------------------------------------------------------
// Stuff for tags
    Q_INVOKABLE bool removeTag(const QString &tagName);
    Q_INVOKABLE Tag::Ptr createTag(const QString &tagName, const QString &uid = QString());
    Tag::Ptr tag(const QString &name, bool create = true);
    QAbstractItemModel *tagsModel() const;
    QString deletedTagName() const;
    bool containsTag(const QString &name) const;
    void clearTags();
    int indexOfTag(const QString &name) const;
//------------------------------------------------------------------------------

    Q_INVOKABLE QString dataFile() const;

public Q_SLOTS:
    bool renameTag(const QString &oldName, const QString &newName);
    void dumpDebugInfo();
    void load();
    void save();

Q_SIGNALS:
    void tagAboutToBeRemoved(const QString &name);

private Q_SLOTS:
    void onTagAboutToBeRemoved(const QString &tagName);

protected:
    Task::Ptr addTask(const Task::Ptr &task);
    Data m_data;
    Kernel *m_kernel;
    virtual void load_impl() = 0;
    virtual void save_impl() = 0;

private:
    void connectTask(const Task::Ptr &);
    int proxyRowToSource(int proxyIndex) const;
    QTimer m_scheduleTimer;
    SortedTagsModel *m_sortedTagModel;
    QString m_deletedTagName;
    int m_savingDisabled;
    TaskFilterProxyModel *m_taskFilterModel;
    TaskFilterProxyModel *m_untaggedTasksModel;
    ArchivedTasksFilterModel *m_stagedTasksModel;
    ArchivedTasksFilterModel *m_archivedTasksModel;
    bool m_createNonExistentTags;
    bool m_savingInProgress;
    bool m_loadingInProgress;
};

#endif
