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
#include "jsonstorage.h"
#include "sortedtagsmodel.h"
#include "archivedtasksfiltermodel.h"
#include "taskfilterproxymodel.h"

Storage::Storage(QObject *parent)
    : QObject(parent)
    , m_savingDisabled(0)
    , m_taskFilterModel(new TaskFilterProxyModel(this))
    , m_untaggedTasksModel(new TaskFilterProxyModel(this))
    , m_stagedTasksModel(new ArchivedTasksFilterModel(this))
    , m_archivedTasksModel(new ArchivedTasksFilterModel(this))
    , m_createNonExistentTags(false)
{
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);
    connect(&m_scheduleTimer, &QTimer::timeout, this, &Storage::save);

    m_tags.insertRole("tag", [&](int i) { return QVariant::fromValue<Tag*>(m_tags.at(i).data()); }, TagRole);
    m_tags.insertRole("tagPtr", [&](int i) { return QVariant::fromValue<Tag::Ptr>(m_tags.at(i)); }, TagPtrRole);
    QAbstractItemModel *tagsModel = m_tags; // android doesn't build if you use m_tags directly in the connect statement
    connect(tagsModel, &QAbstractListModel::dataChanged, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::rowsInserted, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::rowsRemoved, this, &Storage::scheduleSave);
    connect(tagsModel, &QAbstractListModel::modelReset, this, &Storage::scheduleSave);
    qRegisterMetaType<Tag::Ptr>("Tag::Ptr");
    m_sortedTagModel = new SortedTagsModel(m_tags, this);

    connect(this, &Storage::tagAboutToBeRemoved,
            this, &Storage::onTagAboutToBeRemoved);

    m_taskFilterModel->setSourceModel(m_tasks);
    m_untaggedTasksModel->setSourceModel(m_archivedTasksModel);
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);

    connect(&m_scheduleTimer, &QTimer::timeout, this, &Storage::save);
    QAbstractItemModel *tasksModel = m_tasks; // android doesn't build if you use m_tasks directly in the connect statement
    connect(tasksModel, &QAbstractListModel::dataChanged, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::rowsInserted, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::rowsRemoved, this, &Storage::scheduleSave);
    connect(tasksModel, &QAbstractListModel::modelReset, this, &Storage::scheduleSave);

    m_tasks.insertRole("task", [&](int i) { return QVariant::fromValue<Task*>(m_tasks.at(i).data()); }, TaskRole);
    m_tasks.insertRole("taskPtr", [&](int i) { return QVariant::fromValue<Task::Ptr>(m_tasks.at(i)); }, TaskPtrRole);
    m_stagedTasksModel->setSourceModel(m_tasks);
    m_stagedTasksModel->setAcceptArchived(false);

    m_archivedTasksModel->setSourceModel(m_tasks);
    m_archivedTasksModel->setAcceptArchived(true);
    m_untaggedTasksModel->setFilterUntagged(true);
    m_untaggedTasksModel->setObjectName("Untagged and archived tasks model");
}

Storage *Storage::instance()
{
    static Storage *storage = new JsonStorage(qApp);
    return storage;
}

Storage::~Storage()
{
    if (m_scheduleTimer.isActive())
        save();
}

TagList Storage::tags() const
{
    return m_tags;
}

TaskList Storage::tasks() const
{
    return m_tasks;
}

void Storage::load()
{
    m_savingDisabled += 1;
    m_createNonExistentTags = true;
    load_impl();
    m_savingDisabled += -1;
    m_createNonExistentTags = false;

    if (m_tags.isEmpty()) {
        // Create default tags
        createTag(tr("work"));
        createTag(tr("personal"));
        createTag(tr("family"));
        createTag(tr("bills"));
        createTag(tr("books"));
        createTag(tr("movies"));
    }
}

void Storage::save()
{
    m_savingDisabled += 1;
    save_impl();
    m_savingDisabled += -1;
}

int Storage::serializerVersion() const
{
    return JsonSerializerVersion1;
}

void Storage::scheduleSave()
{
    if (m_savingDisabled == 0) {
        if (!m_scheduleTimer.isActive())
            qDebug() << Q_FUNC_INFO;
        m_scheduleTimer.start();
    }
}

bool Storage::removeTag(const QString &tagName)
{
    int index = indexOfTag(tagName);
    if (index == -1) {
        qWarning() << Q_FUNC_INFO << "Non existant tag" << tagName;
        Q_ASSERT(false);
        return false;
    }

    emit tagAboutToBeRemoved(tagName);
    m_tags.removeAt(index);
    m_deletedTagName = tagName;
    return true;
}

Tag::Ptr Storage::tag(const QString &name, bool create)
{
    Tag::Ptr tag = m_tags.value(indexOfTag(name));

    create = create && m_createNonExistentTags;
    return (tag || !create) ? tag : createTag(name);
}

Tag::Ptr Storage::createTag(const QString &tagName)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Will not add empty tag";
        return Tag::Ptr();
    }

    if (indexOfTag(trimmedName) != -1) {
        qDebug() << Q_FUNC_INFO << "Refusing to add duplicate tag " << tagName;
        return Tag::Ptr();
    }

    Tag::Ptr tag = Tag::Ptr(new Tag(trimmedName));
    m_tags << tag;

    return tag;
}

int Storage::indexOfTag(const QString &name) const
{
    QString normalizedName = name.toLower().trimmed();
    for (int i = 0; i < m_tags.count(); ++i) {
        if (m_tags.at(i)->name().toLower() == normalizedName)
            return i;
    }

    return -1;
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
    return std::find_if(m_tags.cbegin(), m_tags.cend(),
                        [&](const Tag::Ptr &tag) { return tag->name().toLower() == normalizedName; }) != m_tags.cend();
}

bool Storage::renameTag(const QString &oldName, const QString &newName)
{
    QString trimmedNewName = newName.trimmed();
    if (oldName == newName || trimmedNewName.isEmpty())
        return true;

    if (indexOfTag(trimmedNewName) != -1)
        return false; // New name already exists

    Tag::Ptr tag = m_tags.value(indexOfTag(oldName));
    if (!tag) {
        qWarning() << "Could not find tag with name" << oldName;
        Q_ASSERT(false);
        return false;
    }

    tag->setName(trimmedNewName);
    scheduleSave();

    return true;
}

void Storage::onTagAboutToBeRemoved(const QString &tagName)
{
    for (int i = 0; i < m_tasks.count(); ++i)
        m_tasks.at(i)->removeTag(tagName);
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
    qDebug() << Q_FUNC_INFO << "task count:" << m_tasks.count();
    for (int i = 0; i < m_tasks.count(); ++i)
        qDebug() << Q_FUNC_INFO << i << m_tasks.at(i)->summary();
}

int Storage::proxyRowToSource(int proxyRow) const
{
    QModelIndex proxyIndex = m_taskFilterModel->index(proxyRow, 0);
    QModelIndex index = m_taskFilterModel->mapToSource(proxyIndex);

    return index.isValid() ? index.row() : -1;
}

int Storage::indexOfTask(const Task::Ptr &task) const
{
    for (int i = 0; i < m_tasks.count(); ++i) {
        if (m_tasks.at(i) == task)
            return i;
    }

    return -1;
}

void Storage::setDisableSaving(bool disable)
{
    m_savingDisabled += (disable ? 1 : -1);
}

Task::Ptr Storage::taskAt(int proxyIndex) const
{
    return m_tasks.value(proxyRowToSource(proxyIndex));
}

Task::Ptr Storage::addTask(const QString &taskText)
{
    Task::Ptr task = Task::createTask(taskText);
    return addTask(task);
}

Task::Ptr Storage::addTask(const Task::Ptr &task)
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

    m_tasks << task;

    return task;
}

void Storage::removeTask(const Task::Ptr &task)
{
    m_tasks.removeAll(task);
}
