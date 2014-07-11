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
#include "tagstorage.h"

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
