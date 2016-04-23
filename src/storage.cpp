/*
  This file is part of Flow.

  Copyright (C) 2014,2016 Sérgio Martins <iamsergio@gmail.com>

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
#include "extendedtagsmodel.h"
#include "jsonstorage.h"
#include "sortedtagsmodel.h"
#include "taskfilterproxymodel.h"
#include "webdavsyncer.h"
#include "runtimeconfiguration.h"
#include "nonemptytagfilterproxy.h"

#if defined(UNIT_TEST_RUN)
# include "assertingproxymodel.h"
  int Storage::storageCount = 0;
  int Storage::saveCallCount = 0;
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
    Task::Ptr task = list.at(index);

    switch (static_cast<Storage::TaskModelRole>(role)) {
    case Storage::TaskRole:
        return QVariant::fromValue<Task*>(task.data());
    case Storage::TaskPtrRole:
        return QVariant::fromValue<Task::Ptr>(task);
    case Storage::DueDateSectionRole:
        if (task && task->dueDate().isValid()) {
            const QDate today = QDate::currentDate();
            const QDate date = task->dueDate();
            if (today.year() == date.year()) {
                if (date.weekNumber() == today.weekNumber()) {
                    return QObject::tr("This week");
                } else if (date.month() == today.month()) {
                    return QObject::tr("This month");
                } else {
                    return QDate::longMonthName(date.month());
                }
            } else {
                return date.year();
            }
        }
        break;
    }

    return QVariant();
}

Storage::Storage(Kernel *kernel, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
    , m_savingDisabled(0)
    , m_taskFilterModel(new TaskFilterProxyModel(this))
    , m_untaggedTasksModel(new TaskFilterProxyModel(this))
    , m_dueDateTasksModel(new TaskFilterProxyModel(this))
    , m_stagedTasksModel(new TaskFilterProxyModel(this))
    , m_archivedTasksModel(new TaskFilterProxyModel(this))
    , m_nonEmptyTagsModel(new NonEmptyTagFilterProxy(this))
    , m_extendedTagsModel(new ExtendedTagsModel(this))
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
    m_extendedTagsModel->setSourceModel(m_sortedTagModel);
    m_nonEmptyTagsModel->setSourceModel(m_extendedTagsModel);

    connect(this, &Storage::tagAboutToBeRemoved,
            this, &Storage::onTagAboutToBeRemoved);

    m_taskFilterModel->setSourceModel(m_data.tasks);
    m_untaggedTasksModel->setSourceModel(m_archivedTasksModel);
    m_dueDateTasksModel->setSourceModel(m_archivedTasksModel);

    QAbstractItemModel *tasksModel = m_data.tasks; // android doesn't build if you use m_data.tasks directly in the connect statement
    connect(tasksModel, &QAbstractListModel::dataChanged, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::rowsInserted, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::rowsRemoved, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::modelReset, this, &Storage::scheduleSave);

    m_data.tasks.setDataFunction(&tasksDataFunction);
    m_data.tasks.insertRole("task", Q_NULLPTR, TaskRole);
    m_data.tasks.insertRole("taskPtr", Q_NULLPTR, TaskPtrRole);
    m_data.tasks.insertRole("dueDateSection", Q_NULLPTR, DueDateSectionRole);
    m_stagedTasksModel->setSourceModel(m_data.tasks);
    m_stagedTasksModel->setFilterStaged(true);

    m_archivedTasksModel->setSourceModel(m_data.tasks);
    m_archivedTasksModel->setFilterArchived(true);
    m_untaggedTasksModel->setFilterUntagged(true);
    m_untaggedTasksModel->setObjectName(QStringLiteral("Untagged and archived tasks model"));
    m_dueDateTasksModel->setFilterDueDated(true);
    m_dueDateTasksModel->setObjectName(QStringLiteral("Archived tasks with due date"));
    m_dueDateTasksModel->sort(0, Qt::AscendingOrder);

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
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_extendedTagsModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_dueDateTasksModel);
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

void Storage::setData(Storage::Data &data)
{
    Tag::List newTags;
    foreach (const Tag::Ptr &tag, data.tags) {
        if (tag->kernel())
            newTags << tag;
        else
            newTags << createTag(tag->name(), tag->uuid());
    }

    data.tags = newTags;

    foreach (const Task::Ptr &task, data.tasks) {
        if (!task->kernel())
            task->setKernel(m_kernel);
    }

    QByteArray oldInstanceId = m_data.instanceId;
    m_data = data;
    if (m_data.instanceId.isEmpty())
        m_data.instanceId = oldInstanceId;

    emit taskCountChanged();
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
        createTag(tr("work"), QStringLiteral("{bb2ab284-8bb7-4aec-a452-084d64e85697}"));
        createTag(tr("personal"), QStringLiteral("{73533168-9a57-4fc0-ba9a-9120bbadcb6c}"));
        createTag(tr("family"), QStringLiteral("{4e81dd75-84c4-4359-912c-f3ead717f694}"));
        createTag(tr("bills"), QStringLiteral("{4b4ae5fb-f35d-4389-9417-96b7ddcb3b8f}"));
        createTag(tr("books"), QStringLiteral("{b2697470-f457-461c-9310-7d4b56aea395}"));
        createTag(tr("movies"), QStringLiteral("{387be44a-1eb7-4895-954a-cf5bc82d8f03}"));
    }

    m_loadingInProgress = false;

    verifyLoadedData();

    emit taskCountChanged();
}

void Storage::verifyLoadedData()
{
    foreach (const Tag::Ptr &tag, m_data.tags) {
        if (tag->uuid(/*createIfEmpty=*/false).isEmpty()) {
            qWarning() << Q_FUNC_INFO << "No uuid for tag" << tag;
#ifdef UNIT_TEST_RUN
           //  Q_ASSERT(false);
#endif
        }
    }
}

void Storage::save()
{
#if defined(UNIT_TEST_RUN)
    Storage::saveCallCount++;
#endif

    if (!m_kernel->runtimeConfiguration().saveEnabled()) // Unit-tests don't save
        return;

    m_savingInProgress = true;
    m_savingDisabled++;
    save_impl();
    m_savingDisabled--;
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
    // For renaming we remove and create instead of simply renaming.
    // Webdav sync isn't prepared for a simple rename, because the task json
    // references the tag name, not the tag uid

    QString trimmedNewName = newName.trimmed();
    if (oldName == newName || trimmedNewName.isEmpty())
        return true;

    if (indexOfTag(trimmedNewName) != -1)
        return false; // New name already exists

    Tag::Ptr oldTag = tag(oldName, /*create=*/ false);
    if (!oldTag) {
        qWarning() << "Could not find tag with name" << oldName;
        return false;
    }

    if (!createTag(trimmedNewName, oldTag->uuid()))
        return false;

    foreach (const Task::Ptr &task, m_data.tasks) {
        if (task->containsTag(oldName))
            task->addTag(trimmedNewName);
    }

    if (!removeTag(oldName))
        return false;

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

TaskFilterProxyModel *Storage::dueDateTasksModel() const
{
    return m_dueDateTasksModel;
}


TaskFilterProxyModel *Storage::stagedTasksModel() const
{
    return m_stagedTasksModel;
}

TaskFilterProxyModel *Storage::archivedTasksModel() const
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
    if (!m_data.tasks.isEmpty()) {
        // Don't use clear() here
        foreach (const Task::Ptr &task, m_data.tasks) {
            removeTask(task);
        }
        emit taskCountChanged();
    }
}

void Storage::setDisableSaving(bool disable)
{
    m_savingDisabled += (disable ? 1 : -1);
    if (m_savingDisabled < 0) {
        qWarning() << "invalid value for m_savingDisabled" << m_savingDisabled;
        Q_ASSERT(false);
    }
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

Task::Ptr Storage::taskAt(int index) const
{
    return m_data.tasks.value(index);
}

Task::Ptr Storage::addTask(const QString &taskText, const QString &uid)
{
    Task::Ptr task = Task::createTask(m_kernel, taskText, uid);
    return addTask(task);
}

Task::Ptr Storage::prependTask(const QString &taskText)
{
    Task::Ptr task = Task::createTask(m_kernel, taskText);
    connectTask(task);
    m_data.tasks.prepend(task);
    emit taskCountChanged();
    return task;
}

Task::Ptr Storage::addTask(const Task::Ptr &task)
{
    connectTask(task);
    m_data.tasks << task;
    emit taskCountChanged();
    return task;
}

void Storage::connectTask(const Task::Ptr &task)
{
    connect(task.data(), &Task::changed, this,
            &Storage::scheduleSave, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_stagedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_archivedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::tagsChanged, m_untaggedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::dueDateChanged, m_dueDateTasksModel,
            &TaskFilterProxyModel::invalidate, Qt::UniqueConnection); // invalidate sorting too
    connect(task.data(), &Task::statusChanged, m_stagedTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);

    connect(task.data(), &Task::priorityChanged, m_stagedTasksModel,
            &TaskFilterProxyModel::invalidate, Qt::UniqueConnection);
    connect(task.data(), &Task::priorityChanged, m_archivedTasksModel,
            &TaskFilterProxyModel::invalidate, Qt::UniqueConnection);
}

void Storage::removeTask(const Task::Ptr &task)
{
    m_data.tasks.removeAll(task);
    task->setTagList(TagRef::List()); // So Tag::taskCount() decreases in case Task::Ptr is left hanging somewhere
    if (webDAVSyncSupported())
        m_data.deletedItemUids << task->uuid(); // TODO: Make this persistent
    emit taskCountChanged();
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

QAbstractItemModel* Storage::nonEmptyTagsModel() const
{
    return m_nonEmptyTagsModel;
}

int Storage::taskCount() const
{
    return m_data.tasks.count();
}

int Storage::nonRecurringTaskCount() const
{
    return std::count_if(m_data.tasks.cbegin(), m_data.tasks.cend(), [](const Task::Ptr &t) {
       return !t->recurs();
    });
}

int Storage::nonDatedTaskCount() const
{
    return std::count_if(m_data.tasks.cbegin(), m_data.tasks.cend(), [](const Task::Ptr &t) {
       return !t->hasDueDate();
    });
}

int Storage::ageAverage() const
{
    int totalAge = 0;
    const int numTasksWithoutDueDate = nonDatedTaskCount();
    foreach (const Task::Ptr &task, m_data.tasks) {
        if (!task->hasDueDate()) {
            totalAge += task->daysSinceCreation();
        }
    }

    return numTasksWithoutDueDate == 0 ? 0 : totalAge / numTasksWithoutDueDate;
}

ExtendedTagsModel* Storage::extendedTagsModel() const
{
    return m_extendedTagsModel;
}
