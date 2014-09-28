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

#include "storage.h"
#include "tag.h"
#include "task.h"
#include "kernel.h"
#include "jsonstorage.h"
#include "sortedtagsmodel.h"
#include "archivedtasksfiltermodel.h"
#include "taskfilterproxymodel.h"
#include "webdavsyncer.h"
#include "runtimeconfiguration.h"

#if defined(UNIT_TEST_RUN)
# include "assertingproxymodel.h"
  int Storage::storageCount = 0;
#endif

static QVariant tagsDataFunction(const TagList &list, int index, int role)
{
    switch (role) {
    case Storage::TagRole:
        return QVariant::fromValue<Tag*>(list.at(index).data());
    case Storage::TagPtrRole:
        return QVariant::fromValue<Tag::Ptr>(list.at(index));
    default:
        return QVariant();
    }
}

static QVariant tasksDataFunction(const TaskList &list, int index, int role)
{
    switch (role) {
    case Storage::TaskRole:
        return QVariant::fromValue<Task*>(list.at(index).data());
    case Storage::TaskPtrRole:
        return QVariant::fromValue<Task::Ptr>(list.at(index));
    default:
        return QVariant();
    }
}

Storage::Storage(Kernel *kernel, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
    , m_savingDisabled(0)
    , m_taskFilterModel(new TaskFilterProxyModel(this))
    , m_untaggedTasksModel(new TaskFilterProxyModel(this))
    , m_stagedTasksModel(new ArchivedTasksFilterModel(this))
    , m_archivedTasksModel(new ArchivedTasksFilterModel(this))
    , m_savingInProgress(false)
    , m_loadingInProgress(false)
{
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);
    connect(&m_scheduleTimer, &QTimer::timeout, this, &Storage::save);

    m_data.tags.setDataFunction(&tagsDataFunction);
    m_data.tags.insertRole("tag", Q_NULLPTR, TagRole);
    m_data.tags.insertRole("tagPtr", Q_NULLPTR, TagPtrRole);
    QAbstractItemModel *tagsModel = m_data.tags; // android doesn't build if you use m_data.tags directly in the connect statement
    connect(tagsModel, &QAbstractListModel::dataChanged, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::rowsInserted, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::rowsRemoved, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::modelReset, this, &Storage::scheduleSave);
    qRegisterMetaType<Tag::Ptr>("Tag::Ptr");
    m_sortedTagModel = new SortedTagsModel(m_data.tags, this);

    connect(this, &Storage::tagAboutToBeRemoved,
            this, &Storage::onTagAboutToBeRemoved);

    m_taskFilterModel->setSourceModel(m_data.tasks);
    m_untaggedTasksModel->setSourceModel(m_archivedTasksModel);
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);

    connect(&m_scheduleTimer, &QTimer::timeout, this, &Storage::save);
    QAbstractItemModel *tasksModel = m_data.tasks; // android doesn't build if you use m_data.tasks directly in the connect statement
    connect(tasksModel, &QAbstractListModel::dataChanged, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::rowsInserted, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::rowsRemoved, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::modelReset, this, &Storage::scheduleSave);

    m_data.tasks.setDataFunction(&tasksDataFunction);
    m_data.tasks.insertRole("task", Q_NULLPTR, TaskRole);
    m_data.tasks.insertRole("taskPtr", Q_NULLPTR, TaskPtrRole);
    m_stagedTasksModel->setSourceModel(m_data.tasks);
    m_stagedTasksModel->setAcceptArchived(false);

    m_archivedTasksModel->setSourceModel(m_data.tasks);
    m_archivedTasksModel->setAcceptArchived(true);
    m_untaggedTasksModel->setFilterUntagged(true);
    m_untaggedTasksModel->setObjectName("Untagged and archived tasks model");

#if defined(UNIT_TEST_RUN)
    AssertingProxyModel *assert = new AssertingProxyModel(this);
    assert->setSourceModel(tasksModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(tagsModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_stagedTasksModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_archivedTasksModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_untaggedTasksModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_taskFilterModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_sortedTagModel);
    storageCount++;
    qDebug() << "Created storage" << this << "; count is now" << storageCount;
#endif

}

Storage::~Storage()
{
#if defined(UNIT_TEST_RUN)
    storageCount--;
    qDebug() << "Deleted storage" << this << "; count is now" << storageCount;
#endif
}

const TagList& Storage::tags() const
{
    return m_data.tags;
}

TaskList Storage::tasks() const
{
    return m_data.tasks;
}

Storage::Data Storage::data() const
{
    return m_data;
}

void Storage::setData(const Storage::Data &data)
{
    QByteArray oldInstanceId = m_data.instanceId;
    m_data = data;
    if (m_data.instanceId.isEmpty())
        m_data.instanceId = oldInstanceId;
}

void Storage::load()
{
    m_loadingInProgress = true;
    m_savingDisabled += 1;
    load_impl();
    m_savingDisabled += -1;

    if (m_data.tags.isEmpty()) {
        // Create default tags. We always use the same uuids for these so we don't get
        // duplicates when synching with other devices
        createTag(tr("work"), "{bb2ab284-8bb7-4aec-a452-084d64e85697}");
        createTag(tr("personal"), "{73533168-9a57-4fc0-ba9a-9120bbadcb6c}");
        createTag(tr("family"), "{4e81dd75-84c4-4359-912c-f3ead717f694}");
        createTag(tr("bills"), "{4b4ae5fb-f35d-4389-9417-96b7ddcb3b8f}");
        createTag(tr("books"), "{b2697470-f457-461c-9310-7d4b56aea395}");
        createTag(tr("movies"), "{387be44a-1eb7-4895-954a-cf5bc82d8f03}");
    }
    m_loadingInProgress = false;
}

void Storage::save()
{
    if (!m_kernel->runtimeConfiguration().saveEnabled()) // Unit-tests don't save
        return;

    m_savingInProgress = true;
    m_savingDisabled += 1;
    save_impl();
    m_savingDisabled += -1;
    m_savingInProgress = false;
}

int Storage::serializerVersion() const
{
    return JsonSerializerVersion1;
}

bool Storage::saveScheduled() const
{
    return m_scheduleTimer.isActive();
}

void Storage::scheduleSave()
{
    if (m_savingDisabled == 0) {
        //if (!m_scheduleTimer.isActive())
            //qDebug() << Q_FUNC_INFO;
        m_scheduleTimer.start();
    }
}

bool Storage::removeTag(const QString &tagName)
{
    int index = indexOfTag(tagName);
    if (index == -1) {
        qWarning() << Q_FUNC_INFO << "Non existant tag" << tagName;
        return false;
    }

    emit tagAboutToBeRemoved(tagName);

    if (webDAVSyncSupported())
        m_data.deletedItemUids << m_data.tags.at(index)->uuid(); // TODO: Make this persistent
    m_data.tags.removeAt(index);
    m_deletedTagName = tagName;
    return true;
}

Tag::Ptr Storage::tag(const QString &name, bool create)
{
    Tag::Ptr tag = m_data.tags.value(indexOfTag(name));
    return (tag || !create) ? tag : createTag(name);
}

Tag::Ptr Storage::createTag(const QString &tagName, const QString &uid)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Will not add empty tag";
        return Tag::Ptr();
    }

    const int index = indexOfTag(trimmedName);
    if (index != -1) {
        qDebug() << Q_FUNC_INFO << "Refusing to add duplicate tag " << tagName;
        return m_data.tags.at(index);
    }

    Tag::Ptr tag = Tag::Ptr(new Tag(m_kernel, trimmedName));
    if (!uid.isEmpty())
        tag->setUuid(uid);

    m_data.tags << tag;
    return tag;
}

int Storage::indexOfTag(const QString &name) const
{
    QString normalizedName = name.toLower().trimmed();
    for (int i = 0; i < m_data.tags.count(); ++i) {
        if (m_data.tags.at(i)->name().toLower() == normalizedName)
            return i;
    }

    return -1;
}

QString Storage::dataFile() const
{
    return m_kernel->runtimeConfiguration().dataFileName();
}

QAbstractItemModel *Storage::tagsModel() const
{
    return m_sortedTagModel;
}

QString Storage::deletedTagName() const
{
    return m_deletedTagName;
}

bool Storage::containsTag(const QString &name) const
{
    QString normalizedName = name.toLower().trimmed();
    foreach (const Tag::Ptr &tag, m_data.tags) {
        if (tag->name().toLower() == normalizedName)
            return true;
    }

    return false;
}

void Storage::clearTags()
{
    // Don't use clear here
    foreach (const Tag::Ptr &tag, m_data.tags) {
        removeTag(tag->name());
    }
}

bool Storage::renameTag(const QString &oldName, const QString &newName)
{
    QString trimmedNewName = newName.trimmed();
    if (oldName == newName || trimmedNewName.isEmpty())
        return true;

    if (indexOfTag(trimmedNewName) != -1)
        return false; // New name already exists

    Tag::Ptr tag = m_data.tags.value(indexOfTag(oldName));
    if (!tag) {
        qWarning() << "Could not find tag with name" << oldName;
        return false;
    }

    tag->setName(trimmedNewName);
    scheduleSave();

    return true;
}

void Storage::onTagAboutToBeRemoved(const QString &tagName)
{
    foreach (const Task::Ptr &task, m_data.tasks)
        task->removeTag(tagName);
}

TaskFilterProxyModel *Storage::taskFilterModel() const
{
    return m_taskFilterModel;
}

TaskFilterProxyModel *Storage::untaggedTasksModel() const
{
    return m_untaggedTasksModel;
}

ArchivedTasksFilterModel *Storage::stagedTasksModel() const
{
    return m_stagedTasksModel;
}

ArchivedTasksFilterModel *Storage::archivedTasksModel() const
{
    return m_archivedTasksModel;
}

void Storage::dumpDebugInfo()
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "task count:" << m_data.tasks.count();
    for (int i = 0; i < m_data.tasks.count(); ++i)
        qDebug() << i << m_data.tasks.at(i)->summary();

    if (!m_data.deletedItemUids.isEmpty()) {
        qDebug() << "Items pending deletion on webdav:";
        foreach (const QString &uid, m_data.deletedItemUids)
            qDebug() << uid;
    }
}

int Storage::proxyRowToSource(int proxyRow) const
{
    QModelIndex proxyIndex = m_taskFilterModel->index(proxyRow, 0);
    QModelIndex index = m_taskFilterModel->mapToSource(proxyIndex);

    return index.isValid() ? index.row() : -1;
}

int Storage::indexOfTask(const Task::Ptr &task) const
{
    for (int i = 0; i < m_data.tasks.count(); ++i) {
        if (m_data.tasks.at(i) == task)
            return i;
    }

    return -1;
}

void Storage::clearTasks()
{
    // Don't use clear() here
    foreach (const Task::Ptr &task, m_data.tasks) {
        removeTask(task);
    }
}

void Storage::setDisableSaving(bool disable)
{
    m_savingDisabled += (disable ? 1 : -1);
}

bool Storage::savingInProgress() const
{
    return m_savingInProgress;
}

bool Storage::loadingInProgress() const
{
    return m_loadingInProgress;
}

bool Storage::webDAVSyncSupported() const
{
#ifndef NO_WEBDAV
    return true;
#endif
    return false;
}

QByteArray Storage::instanceId()
{
    if (m_data.instanceId.isEmpty())
        m_data.instanceId = QUuid::createUuid().toByteArray();

    return m_data.instanceId;
}

Task::Ptr Storage::taskAt(int proxyIndex) const
{
    return m_data.tasks.value(proxyRowToSource(proxyIndex));
}

Task::Ptr Storage::addTask(const QString &taskText)
{
    Task::Ptr task = Task::createTask(m_kernel, taskText);
    return addTask(task);
}

Task::Ptr Storage::prependTask(const QString &taskText)
{
    Task::Ptr task = Task::createTask(m_kernel, taskText);
    connectTask(task);
    m_data.tasks.prepend(task);
    return task;
}

Task::Ptr Storage::addTask(const Task::Ptr &task)
{
    connectTask(task);
    m_data.tasks << task;
    return task;
}

void Storage::connectTask(const Task::Ptr &task)
{
    connect(task.data(), &Task::changed, this,
            &Storage::scheduleSave, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_stagedTasksModel,
            &ArchivedTasksFilterModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_archivedTasksModel,
            &ArchivedTasksFilterModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::tagsChanged, m_untaggedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::statusChanged, m_stagedTasksModel,
            &ArchivedTasksFilterModel::invalidateFilter, Qt::UniqueConnection);
}

void Storage::removeTask(const Task::Ptr &task)
{
    m_data.tasks.removeAll(task);
    task->setTagList(TagRef::List()); // So Tag::taskCount() decreases in case Task::Ptr is left hanging somewhere
    if (webDAVSyncSupported())
        m_data.deletedItemUids << task->uuid(); // TODO: Make this persistent
}

#ifdef DEVELOPER_MODE
void Storage::removeDuplicateData()
{
    Data newData;
    foreach (const Task::Ptr &task, m_data.tasks) {
        bool found = false;
        foreach (const Task::Ptr &task2, newData.tasks) {
            if (task->uuid() != task2->uuid())
                continue;

            found = true;
            break;
        }

        if (found) {
            qDebug() << "Task " << task->summary() << task->uuid() << "is a duplicate";
        } else {
            newData.tasks << task;
        }
    }

    foreach (const Tag::Ptr &tag, m_data.tags) {
        bool found = false;
        foreach (const Tag::Ptr &tag2, newData.tags) {
            if (tag->name() != tag2->name())
                continue;

            found = true;
            break;
        }

        if (found) {
            qDebug() << "Tag " << tag->name() << "is a duplicate";
        } else {
            newData.tags << tag;
        }
    }

    setData(newData);
    qDebug() << Q_FUNC_INFO << "done";
}

#endif
