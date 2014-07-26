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

#include "tagstorage.h"
#include "tagstorageqsettings.h"
#include "sortmodel.h"
#include "remove_if.h"

#include <QDebug>
#include <QGuiApplication>

static bool tagLessThan(const QVariant &left, const QVariant &right)
{
    Tag::Ptr leftTag = left.value<Tag::Ptr>();
    Tag::Ptr rightTag = right.value<Tag::Ptr>();
    Q_ASSERT(leftTag);
    Q_ASSERT(rightTag);

    if (!leftTag || !rightTag)
        return false;

    if (leftTag->taskCount() == rightTag->taskCount()) {
        return QString::compare(leftTag->name(), rightTag->name(), Qt::CaseInsensitive) < 0;
    } else {
        return leftTag->taskCount() < rightTag->taskCount();
    }
}

static bool isNonEmptyTag(const QVariant &variant)
{
    Tag::Ptr tag = variant.value<Tag::Ptr>();
    if (!tag) {
        qWarning() << Q_FUNC_INFO << "Unexpected null tag";
        return false;
    }

    return tag->taskModel()->rowCount() > 0;
}

TagStorage::TagStorage(QObject *parent)
    : QObject(parent)
    , m_nonEmptyTagModel(nullptr)
{
    m_scheduleTimer.setSingleShot(true);
    m_scheduleTimer.setInterval(0);
    connect(&m_scheduleTimer, &QTimer::timeout, this, &TagStorage::saveTags);

    m_tags.insertRole("tag", [&](int i) { return QVariant::fromValue<Tag*>(m_tags.at(i).data()); }, TagRole);
    m_tags.insertRole("tagPtr", [&](int i) { return QVariant::fromValue<Tag::Ptr>(m_tags.at(i)); }, TagPtrRole);

    connect(m_tags, &QAbstractListModel::dataChanged, this, &TagStorage::scheduleSaveTags);
    connect(m_tags, &QAbstractListModel::rowsInserted, this, &TagStorage::scheduleSaveTags);
    connect(m_tags, &QAbstractListModel::rowsRemoved, this, &TagStorage::scheduleSaveTags);
    connect(m_tags, &QAbstractListModel::modelReset, this, &TagStorage::scheduleSaveTags);
    qRegisterMetaType<Tag::Ptr>("Tag::Ptr");
    m_sortModel = new FunctionalModels::SortModel(m_tags, &tagLessThan, TagPtrRole);
}

TagStorage *TagStorage::instance()
{
    static TagStorage *storage = new TagStorageQSettings(qApp);
    return storage;
}

void TagStorage::scheduleSaveTags()
{
    m_scheduleTimer.start();
}

bool TagStorage::removeTag(const QString &tagName)
{
    int index = indexOf(tagName);
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

Tag::Ptr TagStorage::createTag(const QString &tagName)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Will not add empty tag";
        return Tag::Ptr();
    }

    if (indexOf(trimmedName) != -1) {
        qDebug() << Q_FUNC_INFO << "Refusing to add duplicate tag " << tagName;
        return Tag::Ptr();
    }

    Tag::Ptr tag = Tag::Ptr(new Tag(trimmedName));
    m_tags << tag;

    return tag;
}

Tag::Ptr TagStorage::tag(const QString &name, bool create)
{
    Tag::Ptr tag = m_tags.value(indexOf(name));
    return (tag || !create) ? tag : createTag(name);
}

bool TagStorage::contains(const QString &name) const
{
    QString normalizedName = name.toLower().trimmed();
    return std::find_if(m_tags.cbegin(), m_tags.cend(),
                        [&](const Tag::Ptr &tag) { return tag->name().toLower() == normalizedName; }) != m_tags.cend();
}

QAbstractItemModel *TagStorage::model() const
{
    return m_sortModel;
}

QAbstractItemModel *TagStorage::nonEmptyTagModel() const
{
    if (!m_nonEmptyTagModel) {
        // Delayed loading, due to __cxa_guard_acquire hang
        m_nonEmptyTagModel = new FunctionalModels::Remove_if(m_sortModel, &isNonEmptyTag, TagPtrRole);

        connect(this, &TagStorage::invalidateNonEmptyTagModel,
                m_nonEmptyTagModel, &FunctionalModels::Remove_if::invalidateFilter,
                Qt::QueuedConnection);
    }

    return m_nonEmptyTagModel;
}

QString TagStorage::deletedTagName() const
{
    return m_deletedTagName;
}

void TagStorage::monitorTag(Tag *tag)
{
    //connect(tag, &Tag::taskCountChanged, this, &TagStorage::onTaskCountChanged);
    connect(tag, &Tag::archivedTaskCountChanged, this, &TagStorage::onTaskCountChanged);
}

bool TagStorage::renameTag(const QString &oldName, const QString &newName)
{
    QString trimmedNewName = newName.trimmed();
    if (oldName == newName || trimmedNewName.isEmpty())
        return true;

    if (indexOf(trimmedNewName) != -1)
        return false; // New name already exists

    Tag::Ptr tag = m_tags.value(indexOf(oldName));
    if (!tag) {
        qWarning() << "Could not find tag with name" << oldName;
        Q_ASSERT(false);
        return false;
    }

    tag->setName(trimmedNewName);
    scheduleSaveTags();

    return true;
}

void TagStorage::setTags(const Tag::List &tags)
{
    Tag::List sanitizedList = tags;
    sanitizedList.removeAll(Tag::Ptr()); // Remove invalid tags
    m_tags = sanitizedList;
}

void TagStorage::onTaskCountChanged(int oldCount, int newCount)
{
    if ((oldCount == 0) ^ (newCount == 0))
        emit invalidateNonEmptyTagModel();
}

void TagStorage::saveTags()
{
    m_deletedTagName = QString();
    saveTags_impl();
}

void TagStorage::loadTags()
{
    loadTags_impl();
    // qDebug() << Q_FUNC_INFO << "Loaded:" << m_tags.count();
}

int TagStorage::indexOf(const QString &name) const
{
    QString normalizedName = name.toLower().trimmed();
    for (int i = 0; i < m_tags.count(); ++i) {
        if (m_tags.at(i)->name().toLower() == normalizedName)
            return i;
    }

    return -1;
}
