/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "task.h"
#include "settings.h"
#include "checkabletagmodel.h"
#include "taskcontextmenumodel.h"
#include "storage.h"
#include "kernel.h"

#include <QQmlEngine>
#include <QUuid>

enum {
    TagRole = Qt::UserRole,
    TaskRole
};

static QVariant dataFunction(const TagRef::List &list, int index, int role)
{
    switch (role) {
    case TagRole:
        return QVariant::fromValue<Tag*>(list.at(index).m_tag.data());
    case TaskRole:
        return QVariant::fromValue<Task*>(list.at(index).m_task.data());
    default:
        return QVariant();
    }
}

Task::Task(Storage *storage, const QString &summary)
    : QObject()
    , Syncable()
    , m_summary(summary.isEmpty() ? tr("New Task") : summary)
    , m_status(TaskStopped)
    , m_staged(false)
    , m_creationDate(QDateTime::currentDateTimeUtc())
    , m_modificationDate(m_creationDate)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    m_checkableTagModel = new CheckableTagModel(this);

    modelSetup(storage);
}

void Task::modelSetup(Storage *storage)
{
    m_tags.setDataFunction(&dataFunction);
    m_tags.insertRole("tag", Q_NULLPTR, TagRole);
    m_tags.insertRole("task", Q_NULLPTR, TaskRole);

    connect(this, &Task::summaryChanged, &Task::onEdited);
    connect(this, &Task::tagsChanged, &Task::onEdited);
    connect(this, &Task::descriptionChanged, &Task::onEdited);
    connect(this, &Task::statusChanged, &Task::onEdited);
    connect(this, &Task::stagedChanged, &Task::onEdited);
    QAbstractItemModel *tagsModel = m_tags; // android doesn't build if you use m_tags directly in the connect statement
    connect(tagsModel, &QAbstractListModel::modelReset, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::rowsInserted, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::rowsRemoved, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::layoutChanged, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::dataChanged, this, &Task::tagsChanged);

    if (storage) { // Can be null when creating temporary tasks for webdav sync
        auto roleNames = storage->tagsModel()->roleNames();
        roleNames.insert(Qt::CheckStateRole, QByteArray("checkState"));
        QAbstractItemModel *allTagsModel = storage->tagsModel();
        Q_ASSERT(allTagsModel);
        m_checkableTagModel->setSourceModel(allTagsModel);
    }

    m_contextMenuModel = new TaskContextMenuModel(this, this);
}

Task::Ptr Task::createTask(Storage *storage, const QString &summary)
{
    Task::Ptr task = Task::Ptr(new Task(storage, summary));
    task->setWeakPointer(task);
    return task;
}

bool Task::staged() const
{
   return m_staged;
}

void Task::setStaged(bool staged)
{
    if (m_staged != staged) {
        m_staged = staged;
        emit stagedChanged();
    }
}

QString Task::summary() const
{
    return m_summary;
}

void Task::setSummary(const QString &text)
{
    if (m_summary != text) {
        m_summary = text;
        emit summaryChanged();
    }
}

QString Task::description() const
{
    return m_description;
}

void Task::setDescription(const QString &text)
{
    if (m_description != text) {
        m_description = text;
        emit descriptionChanged();
    }
}

bool Task::containsTag(const QString &name) const
{
    return indexOfTag(name) != -1;
}

int Task::indexOfTag(const QString &name) const
{
    QString trimmedName = name.toLower().trimmed();
    for (int i = 0; i < m_tags.count(); ++i) {
        Q_ASSERT(m_tags.at(i).m_tag);
        if (m_tags.at(i).m_tag->name().toLower() == trimmedName)
            return i;
    }

    return -1;
}

TagRef::List Task::tags() const
{
    return m_tags;
}

void Task::setTagList(const TagRef::List &list)
{
    if (!m_tags.isEmpty()) // No need to emit uneeded signals
        m_tags.clear();

    // Filter out duplicated tags
    QStringList addedTags;
    foreach (const TagRef &ref, list) {
        QString name = ref.m_tag->name().toLower();
        if (!addedTags.contains(name) && !name.isEmpty()) {
            addedTags << name;
            m_tags << ref;
        }
    }
}

QAbstractItemModel *Task::tagModel() const
{
    return m_tags;
}

QAbstractItemModel *Task::checkableTagModel() const
{
    return m_checkableTagModel;
}

void Task::addTag(const QString &tagName)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty())
        return;

    if (!containsTag(trimmedName)) {
        m_tags.append(TagRef(this, trimmedName, /*temporary=*/ true));
        emit tagToggled(trimmedName);
    }
}

void Task::removeTag(const QString &tagName)
{
    int index = indexOfTag(tagName);
    if (index != -1) {
        m_tags.removeAt(index);
        emit tagToggled(tagName);
    }
}

TaskStatus Task::status() const
{
    return m_status;
}

void Task::setStatus(TaskStatus status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void Task::setCreationDate(const QDateTime &date)
{
    m_creationDate = date;
}

QDateTime Task::creationDate() const
{
    return m_creationDate;
}

void Task::setModificationDate(const QDateTime &date)
{
    m_modificationDate = date;
}

QDateTime Task::modificationDate() const
{
    return m_modificationDate;
}

bool Task::running() const
{
    return m_status == TaskStarted;
}

bool Task::stopped() const
{
    return m_status == TaskStopped;
}

bool Task::paused() const
{
    return m_status == TaskPaused;
}

Task::Ptr Task::toStrongRef() const
{
    Q_ASSERT(m_this);
    return m_this.toStrongRef();
}

void Task::setWeakPointer(const QWeakPointer<Task> &ptr)
{
    m_this = ptr;
}

QVariantMap Task::toJson() const
{
    QVariantMap map = Syncable::toJson();
    map.insert("summary", m_summary);
    map.insert("staged", m_staged);
    map.insert("description", m_description);
    QVariantList tags;
    for (int i = 0; i < m_tags.count(); ++i)
        tags << m_tags.at(i).m_tag->name();
    map.insert("tags", tags);
    map.insert("creationTimestamp", m_creationDate.toMSecsSinceEpoch());

    if (m_modificationDate.isValid())
        map.insert("modificationTimestamp", m_modificationDate.toMSecsSinceEpoch());

    return map;
}

void Task::fromJson(const QVariantMap &map)
{
    Syncable::fromJson(map);

    QString summary = map.value("summary").toString();
    if (summary.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "empty task summary";
        summary = tr("New Task");
    }

    blockSignals(true); // so we don't increment revision while calling setters
    setSummary(summary);

    QString description = map.value("description").toString();
    setDescription(description);
    setStaged(map.value("staged", false).toBool());

    QDateTime creationDate = QDateTime::fromMSecsSinceEpoch(map.value("creationTimestamp", QDateTime()).toLongLong());
    if (creationDate.isValid()) // If invalid it uses the ones set in CTOR
        setCreationDate(creationDate);

    QDateTime modificationDate = QDateTime::fromMSecsSinceEpoch(map.value("modificationTimestamp", QDateTime()).toLongLong());
    if (modificationDate.isValid())
        setModificationDate(modificationDate);

    QVariantList tagsVariant = map.value("tags").toList();
    TagRef::List tags;
    foreach (const QVariant &tag, tagsVariant) {
        if (!tag.toString().isEmpty())
            tags << TagRef(this, tag.toString());
    }

    setTagList(tags);
    blockSignals(false);
}

TaskContextMenuModel *Task::contextMenuModel() const
{
    return m_contextMenuModel;
}

bool Task::operator==(const Task &other) const
{
    return m_uuid == other.uuid();
}

void Task::onEdited()
{
    m_modificationDate = QDateTime::currentDateTimeUtc();
    m_revision++;
    emit changed();
}
