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

#include "tagref.h"
#include "storage.h"
#include "task.h"
#include "kernel.h"

TagRef::TagRef(const TagRef &other)
{
    m_task = other.m_task;
    m_tagName = other.m_tagName;
    m_temporary = false;
    m_tag = other.m_tag;
    m_storage = other.m_storage;

    Q_ASSERT(!m_tagName.isEmpty());
    incrementCount();
}

TagRef TagRef::operator=(const TagRef &other)
{
    Q_ASSERT(m_storage == other.m_storage);
    decrementCount();

    m_task = other.m_task;
    m_tagName = other.m_tagName;
    m_tag = other.m_tag;
    m_storage = other.m_storage;

    incrementCount();

    return *this;
}


// Temporary is a performance optimization, we don't want to emit uneeded signals
// when temporaries are constructed while appending TagRefs into a list
TagRef::TagRef(const QPointer<Task> &task, const QString &tagName,
               Storage *storage, bool temporary)
    : m_task(task)
    , m_tagName(tagName)
    , m_temporary(temporary)
    , m_storage(storage)
{
    Q_ASSERT(!tagName.isEmpty());
    if (m_storage) {
        m_tag = m_storage->tag(tagName); // Create if it doesn't exist

        if (!temporary)
            incrementCount();
    }
}

TagRef::~TagRef()
{
    if (!m_temporary)
        decrementCount();
}

Tag::Ptr TagRef::tag() const
{
    return m_tag;
}

Storage *TagRef::storage() const
{
    return m_storage;
}

QString TagRef::tagName() const
{
    return m_tag ? m_tag->name() : m_tagName;
}

void TagRef::incrementCount()
{
    Tag::Ptr tag = this->tag();
    if (!tag)
        return;

    tag->incrementTaskCount(1);
    QObject::connect(tag.data(), &Tag::nameChanged, m_task.data(), &Task::changed);
}

void TagRef::decrementCount()
{
    Tag::Ptr tag = this->tag();
    if (!tag)
        return;

    tag->incrementTaskCount(-1);
    QObject::disconnect(tag.data(), &Tag::nameChanged, m_task.data(), &Task::changed);
}

bool operator==(const TagRef &tagRef, const QString &name)
{
    return tagRef.tagName() == name;
}
