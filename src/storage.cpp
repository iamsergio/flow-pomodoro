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

#include "storage.h"
#include "tag.h"
#include "task.h"
#include "kernel.h"
#include "extendedtagsmodel.h"
#include "jsonstorage.h"
#include "sortedtagsmodel.h"
#include "taskfilterproxymodel.h"
#include "runtimeconfiguration.h"
#include "nonemptytagfilterproxy.h"
#include "utils.h"

#if defined(UNIT_TEST_RUN)
# include "assertingproxymodel.h"
  int Storage::storageCount = 0;
  int Storage::saveCallCount = 0;
#endif

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
    case Storage::TagsStrSectionRole:
        return task ? task->tagsStr() : QString();
    }

    return QVariant();
}

Storage::Storage(Kernel *kernel, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
    , m_taskFilterModel(new TaskFilterProxyModel(this))
    , m_untaggedTasksModel(new TaskFilterProxyModel(this))
    , m_dueDateTasksModel(new TaskFilterProxyModel(this))
    , m_stagedTasksModel(new TaskFilterProxyModel(this))
    , m_archivedTasksModel(new TaskFilterProxyModel(this))
    , m_archivedHotTasksModel(new TaskFilterProxyModel(this))
    , m_nonEmptyTagsModel(new NonEmptyTagFilterProxy(this))
    , m_extendedTagsModel(new ExtendedTagsModel(this))
{
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);
    connect(&m_scheduleTimer, &QTimer::timeout, this, &Storage::save);

    connect(this, &Storage::taskCountChanged, this, &Storage::totalNeededEffortChanged);

    QAbstractItemModel *tagsModel = kernel->tagManager()->tags();
    connect(tagsModel, &QAbstractListModel::dataChanged, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::rowsInserted, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::rowsRemoved, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::modelReset, this, &Storage::scheduleSave);
    qRegisterMetaType<Tag::Ptr>("Tag::Ptr");
    m_sortedTagModel = new SortedTagsModel(kernel->tagManager()->tags(), this);
    m_extendedTagsModel->setSourceModel(m_sortedTagModel);
    m_nonEmptyTagsModel->setSourceModel(m_extendedTagsModel);

    connect(kernel->tagManager(), &TagManager::tagAboutToBeRemoved,
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
    m_data.tasks.insertRole("tagsStrSection", Q_NULLPTR, TagsStrSectionRole);
    m_stagedTasksModel->setSourceModel(m_data.tasks);
    m_stagedTasksModel->setFilterStaged(true);

    m_archivedTasksModel->setSourceModel(m_data.tasks);
    m_archivedTasksModel->setFilterArchived(true);
    m_untaggedTasksModel->setFilterUntagged(true);
    m_untaggedTasksModel->setObjectName(QStringLiteral("Untagged and archived tasks model"));
    m_dueDateTasksModel->setFilterDueDated(true);
    m_dueDateTasksModel->setObjectName(QStringLiteral("Archived tasks with due date"));
    m_dueDateTasksModel->sort(0, Qt::AscendingOrder);

    m_archivedHotTasksModel->setSourceModel(m_data.tasks);
    m_archivedHotTasksModel->setSortByTagsFirst(true);
    m_archivedHotTasksModel->setFilterArchived(true);
    m_archivedHotTasksModel->setFilterHot(true);

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
    return m_kernel->tagManager()->tags();
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
    m_kernel->tagManager()->setTags(data.tags);

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
    load_impl();

    m_kernel->tagManager()->maybeCreateDefaultTags();

    m_loadingInProgress = false;

    verifyLoadedData();

    emit taskCountChanged();
}

void Storage::verifyLoadedData()
{
    foreach (const Tag::Ptr &tag, tags()) {
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
    save_impl();
    m_savingInProgress = false;

    emit saveFinished();
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
    m_scheduleTimer.start();
}

bool Storage::removeTag(const QString &tagName)
{
    return m_kernel->tagManager()->removeTag(tagName);
}

Tag::Ptr Storage::tag(const QString &name, bool create)
{
    return m_kernel->tagManager()->tag(name, create);
}

Tag::Ptr Storage::createTag(const QString &tagName, const QString &uid)
{
    return m_kernel->tagManager()->createTag(tagName, uid);
}

int Storage::indexOfTag(const QString &name) const
{
    return m_kernel->tagManager()->indexOfTag(name);
}

QString Storage::dataFile() const
{
    if (m_kernel->isReadOnly() && QFile::exists(remoteDataFile())) {
        if (Utils::isValidJsonFile(remoteDataFile())) {
            return remoteDataFile();
        } else {
            qWarning() << "Invalid json file" << remoteDataFile();
            return m_kernel->runtimeConfiguration().dataFileName();
        }
    } else {
        return m_kernel->runtimeConfiguration().dataFileName();
    }
}

QString Storage::remoteDataFile() const
{
    return m_kernel->runtimeConfiguration().dataFileName() + QStringLiteral(".remote");
}

QAbstractItemModel *Storage::tagsModel() const
{
    return m_sortedTagModel;
}

bool Storage::containsTag(const QString &name) const
{
    return m_kernel->tagManager()->containsTag(name);
}

void Storage::clearTags()
{
    // Don't use clear here
    m_kernel->tagManager()->clearTags();
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

TaskFilterProxyModel *Storage::archivedHotTasksModel() const
{
    return m_archivedHotTasksModel;
}

void Storage::dumpDebugInfo()
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "task count:" << m_data.tasks.count();
    for (int i = 0; i < m_data.tasks.count(); ++i)
        qDebug() << i << m_data.tasks.at(i)->summary();
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

bool Storage::savingInProgress() const
{
    return m_savingInProgress;
}

bool Storage::loadingInProgress() const
{
    return m_loadingInProgress;
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

    connect(task.data(), &Task::priorityChanged, m_archivedHotTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);
    connect(task.data(), &Task::stagedChanged, m_archivedHotTasksModel,
            &TaskFilterProxyModel::invalidateFilter, Qt::UniqueConnection);

    connect(task.data(), &Task::priorityChanged, m_stagedTasksModel,
            &TaskFilterProxyModel::invalidate, Qt::UniqueConnection);
    connect(task.data(), &Task::priorityChanged, m_archivedTasksModel,
            &TaskFilterProxyModel::invalidate, Qt::UniqueConnection);
    connect(task.data(), &Task::estimatedEffortChanged, this,
            &Storage::totalNeededEffortChanged);
}

void Storage::removeTask(const Task::Ptr &task)
{
    m_data.tasks.removeAll(task);
    task->setTagList(TagRef::List()); // So Tag::taskCount() decreases in case Task::Ptr is left hanging somewhere
    emit taskCountChanged();
}

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

int Storage::totalNeededEffort() const
{
    int total = 0;
    foreach (const Task::Ptr &task, m_data.tasks) {
        if (task->estimatedEffort()) {
            total += task->estimatedEffort();
        }
    }

    return total;
}

int Storage::numTasksWithEffort() const
{
    return std::count_if(m_data.tasks.cbegin(), m_data.tasks.cend(), [](const Task::Ptr &t) {
       return t->estimatedEffort() > 0;
    });
}

ExtendedTagsModel* Storage::extendedTagsModel() const
{
    return m_extendedTagsModel;
}
