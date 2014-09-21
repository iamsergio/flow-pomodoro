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
    m_tag = other.m_tag;
    m_temporary = false;
    Q_ASSERT(m_tag);
    incrementCount();
}

TagRef TagRef::operator=(const TagRef &other)
{
    decrementCount();

    m_task = other.m_task;
    m_tag = other.m_tag;

    incrementCount();

    return *this;
}


// Temporary is a performance optimization, we don't want to emit uneeded signals
// when temporaries are constructed while appending TagRefs into a list
TagRef::TagRef(const QPointer<Task> &task, const QString &tagName, bool temporary)
    : m_tag(task->kernel()->storage()->tag(tagName))
    , m_task(task)
    , m_temporary(temporary)
{
    Q_ASSERT(m_tag);
    if (!temporary)
        incrementCount();
}

TagRef::~TagRef()
{
    if (!m_temporary)
        decrementCount();
}

void TagRef::incrementCount()
{
    m_tag->incrementTaskCount(1);

    if (m_task)
        QObject::connect(m_tag.data(), &Tag::nameChanged, m_task.data(), &Task::changed);
}

void TagRef::decrementCount()
{
    m_tag->incrementTaskCount(-1);

    if (m_task)
        QObject::disconnect(m_tag.data(), &Tag::nameChanged, m_task.data(), &Task::changed);
}

bool operator==(const TagRef &tagRef, const QString &name)
{
    return tagRef.m_tag->name() == name;
}
