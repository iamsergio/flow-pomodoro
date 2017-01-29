/*
  This file is part of Flow.

  Copyright (C) 2016 Sérgio Martins <iamsergio@gmail.com>

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

#include "tagmanager.h"

#include <QString>
#include <QDebug>

template <typename T>
static inline bool itemListContains(const GenericListModel<T> &list, const T &item)
{
    return indexOfItem(list, item) != -1;
}

template <typename T>
static inline int indexOfItem(const GenericListModel<T> &list, const T &item)
{
    const int count = list.count();
    for (int i = 0; i < count; i++)
        if (*list.at(i).data() == *item.data())
            return i;

    return -1;
}

static QVariant tagsDataFunction(const TagList &list, int index, int role)
{
    switch (role) {
    case TagManager::TagRole:
        return QVariant::fromValue<Tag*>(list.at(index).data());
    case TagManager::TagPtrRole:
        return QVariant::fromValue<Tag::Ptr>(list.at(index));
    default:
        return QVariant();
    }
}

TagManager::TagManager(Kernel *kernel, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
{
    m_tags.setDataFunction(&tagsDataFunction);
    m_tags.insertRole("tag", Q_NULLPTR, TagRole);
    m_tags.insertRole("tagPtr", Q_NULLPTR, TagPtrRole);
}

Tag::Ptr TagManager::createTag(const QString &tagName, const QString &uid)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Will not add empty tag";
        return Tag::Ptr();
    }

    const int index = indexOfTag(trimmedName);
    if (index != -1) {
        qDebug() << Q_FUNC_INFO << "Refusing to add duplicate tag " << tagName;
        return m_tags.at(index);
    }

    Tag::Ptr tag = Tag::Ptr(new Tag(m_kernel, trimmedName));
    if (!uid.isEmpty())
        tag->setUuid(uid);

    m_tags << tag;
    return tag;
}

void TagManager::createTag(const QVariantMap &map)
{
    Tag::Ptr tag = Tag::Ptr(new Tag(m_kernel, QString()));
    tag->fromJson(map);
    if (!tag->name().isEmpty() && !itemListContains<Tag::Ptr>(m_tags, tag)) {
        if (Tag::Ptr tag2 = this->tag(tag->name(), /*create=*/ false)) {
            tag = tag2;
        }

        m_tags << tag;
    }
}

int TagManager::indexOfTag(const QString &name) const
{
    const QString normalizedName = name.toLower().trimmed();
    const int tagCount = m_tags.size();
    for (int i = 0; i < tagCount; ++i) {
        if (m_tags.at(i)->name().toLower() == normalizedName)
            return i;
    }

    return -1;
}

Tag::Ptr TagManager::tag(const QString &name, bool create)
{
    Tag::Ptr tag = m_tags.value(indexOfTag(name));
    return (tag || !create) ? tag : createTag(name);
}

bool TagManager::removeTag(const QString &tagName)
{
    const int index = indexOfTag(tagName);
    if (index == -1) {
        qWarning() << Q_FUNC_INFO << "Non existant tag" << tagName;
        return false;
    }

    emit tagAboutToBeRemoved(tagName);

    m_tags.removeAt(index);
    return true;
}

const TagList& TagManager::tags() const
{
    return m_tags;
}

void TagManager::maybeCreateDefaultTags()
{
    if (m_tags.isEmpty()) {
        // Create default tags. We always use the same uuids for these so we don't get
        // duplicates when synching with other devices
        createTag(tr("work"), QStringLiteral("{bb2ab284-8bb7-4aec-a452-084d64e85697}"));
        createTag(tr("personal"), QStringLiteral("{73533168-9a57-4fc0-ba9a-9120bbadcb6c}"));
        createTag(tr("family"), QStringLiteral("{4e81dd75-84c4-4359-912c-f3ead717f694}"));
        createTag(tr("bills"), QStringLiteral("{4b4ae5fb-f35d-4389-9417-96b7ddcb3b8f}"));
        createTag(tr("books"), QStringLiteral("{b2697470-f457-461c-9310-7d4b56aea395}"));
        createTag(tr("movies"), QStringLiteral("{387be44a-1eb7-4895-954a-cf5bc82d8f03}"));
    }
}

bool TagManager::containsTag(const QString &name) const
{
    const QString normalizedName = name.toLower().trimmed();
    foreach (const Tag::Ptr &tag, m_tags) {
        if (tag->name().toLower() == normalizedName)
            return true;
    }

    return false;
}

void TagManager::clearTags()
{
    // Don't use clear here
    foreach (const Tag::Ptr &tag, m_tags) {
        removeTag(tag->name());
    }
}

void TagManager::setTags(const QStringList &tagNames)
{
    // TODO: check if this works, when it's needed
    clearTags();
    foreach (const QString &name, tagNames) {
        createTag(name /**uid*/);
    }
}
