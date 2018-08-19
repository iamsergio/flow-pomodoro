/*
  This file is part of Flow.

  Copyright (C) 2014,2016,2018 Sérgio Martins <iamsergio@gmail.com>

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
#include "tagmanager.h"
#include "genericlistmodel.h"

#include <QTimer>
#include <QObject>
#include <QUuid>

class Kernel;
class SortedTagsModel;
class TaskFilterProxyModel;
class NonEmptyTagFilterProxy;
class ExtendedTagsModel;

typedef GenericListModel<Task::Ptr> TaskList;

enum {
    JsonSerializerVersion1 = 1
};

class Storage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ExtendedTagsModel* extendedTagsModel READ extendedTagsModel CONSTANT)
    Q_PROPERTY(int ageAverage READ ageAverage NOTIFY taskCountChanged)
    Q_PROPERTY(int taskCount READ taskCount NOTIFY taskCountChanged)
    Q_PROPERTY(int totalNeededEffort READ totalNeededEffort NOTIFY totalNeededEffortChanged)
    Q_PROPERTY(int numTasksWithEffort READ numTasksWithEffort NOTIFY totalNeededEffortChanged)
    Q_PROPERTY(int nonRecurringTaskCount READ nonRecurringTaskCount NOTIFY taskCountChanged)
    Q_PROPERTY(int nonDatedTaskCount READ nonDatedTaskCount NOTIFY taskCountChanged)
    Q_PROPERTY(QAbstractItemModel* nonEmptyTagsModel READ nonEmptyTagsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* tagsModel READ tagsModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* stagedTasksModel READ stagedTasksModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* archivedTasksModel READ archivedTasksModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* taskFilterModel READ taskFilterModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* untaggedTasksModel READ untaggedTasksModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* dueDateTasksModel READ dueDateTasksModel CONSTANT)

public:
    enum TaskModelRole {
        TaskRole = Qt::UserRole + 1,
        TaskPtrRole,
        DueDateSectionRole,
        TagsStrSectionRole
    };

    struct Data {
        Data() {}
        TaskList tasks;
        QStringList tags;
        int serializerVersion = JsonSerializerVersion1;
        QByteArray instanceId;
    };

    explicit Storage(Kernel *kernel, QObject *parent = nullptr);
    ~Storage();

    const TagList& tags() const;
    TaskList tasks() const;
    Storage::Data data() const;
    void setData(Data &data);

    int serializerVersion() const;

    bool saveScheduled() const;
    void scheduleSave();

    bool savingInProgress() const;
    bool loadingInProgress() const;

    QByteArray instanceId();

//------------------------------------------------------------------------------
// Stuff for tasks
    TaskFilterProxyModel* taskFilterModel() const;
    TaskFilterProxyModel* untaggedTasksModel() const;
    TaskFilterProxyModel* dueDateTasksModel() const;
    TaskFilterProxyModel* stagedTasksModel() const;
    TaskFilterProxyModel* archivedTasksModel() const;
    TaskFilterProxyModel* archivedHotTasksModel() const;
    Task::Ptr taskAt(int index) const;
    Task::Ptr addTask(const QString &taskText, const QString &uid = QString());
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
    ExtendedTagsModel *extendedTagsModel() const;
    QString deletedTagName() const;
    bool containsTag(const QString &name) const;
    void clearTags();
    int indexOfTag(const QString &name) const;
//------------------------------------------------------------------------------

    Q_INVOKABLE QString dataFile() const;
    QString remoteDataFile() const;

#if defined(UNIT_TEST_RUN)
    static int storageCount;
    static int saveCallCount;
#endif

    QAbstractItemModel* nonEmptyTagsModel() const;
    int taskCount() const;
    int nonRecurringTaskCount() const;
    int nonDatedTaskCount() const;

    /**
     * Returns the age average, in days since creation, of all tasks that don't have due date
     */
    int ageAverage() const;

    int totalNeededEffort() const;
    int numTasksWithEffort() const;

public Q_SLOTS:
    bool renameTag(const QString &oldName, const QString &newName);
    void dumpDebugInfo();
    void load();
    void save();

Q_SIGNALS:
    void taskCountChanged();
    void totalNeededEffortChanged();
    void saveFinished();

private Q_SLOTS:
    void onTagAboutToBeRemoved(const QString &tagName);

protected:
    Task::Ptr addTask(const Task::Ptr &task);
    Data m_data;
    Kernel *m_kernel;
    virtual void load_impl() = 0;
    virtual void save_impl() = 0;

private:
    void verifyLoadedData();
    void connectTask(const Task::Ptr &);
    int proxyRowToSource(int proxyIndex) const;
    QTimer m_scheduleTimer;
    SortedTagsModel *m_sortedTagModel = nullptr;
    TaskFilterProxyModel *const m_taskFilterModel;
    TaskFilterProxyModel *const m_untaggedTasksModel;
    TaskFilterProxyModel *const m_dueDateTasksModel;
    TaskFilterProxyModel *const m_stagedTasksModel;
    TaskFilterProxyModel *const m_archivedTasksModel;
    TaskFilterProxyModel *const m_archivedHotTasksModel;
    NonEmptyTagFilterProxy *const m_nonEmptyTagsModel;
    ExtendedTagsModel *const m_extendedTagsModel;
    bool m_savingInProgress = false;
    bool m_loadingInProgress = false;
};

#endif
