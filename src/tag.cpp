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

#include "tag.h"
#include "tagstorage.h"
#include <QQmlEngine>

Tag::Tag(const QString &_name)
    : QObject()
    , m_name(_name)
    , m_taskCount(0)
    , m_beingEdited(false)
{
    Q_ASSERT(!m_name.isEmpty());
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Tag::~Tag()
{
}

int Tag::taskCount() const
{
    return m_taskCount;
}

void Tag::setTaskCount(int count)
{
    if (count != m_taskCount) {
        m_taskCount = count;
        emit taskCountChanged();
    }
}

QString Tag::name() const
{
    return m_name;
}

void Tag::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged();
    }
}

bool Tag::beingEdited() const
{
    return m_beingEdited;
}

void Tag::setBeingEdited(bool yes)
{
    if (m_beingEdited != yes) {
        m_beingEdited = yes;
        emit beingEditedChanged();
    }
}

bool operator==(const Tag::Ptr &tag1, const Tag::Ptr &tag2)
{
    return tag1 && tag2 && tag1->name() == tag2->name();
}

TagRef::TagRef(const TagRef &other)
{
    m_tag = other.m_tag;
    m_tag->setTaskCount(m_tag->taskCount() + 1);
}

TagRef::TagRef(const QString &name)
    : m_tag(TagStorage::instance()->tag(name))
{
    m_tag->setTaskCount(m_tag->taskCount() + 1);
}

TagRef::~TagRef()
{
    m_tag->setTaskCount(m_tag->taskCount() - 1);
}

bool operator==(const TagRef &tagRef, const QString &name)
{
    return tagRef.m_tag->name() == name;
}
