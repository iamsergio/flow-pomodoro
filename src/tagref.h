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

#ifndef TAGREF_H
#define TAGREF_H

#include "genericlistmodel.h"
#include "tag.h"

class TagRef
{
public:
    typedef GenericListModel<TagRef> List;

    TagRef() = delete;
    TagRef(const TagRef &other);
    TagRef(const QString &name);
    ~TagRef();

    Tag::Ptr m_tag;
};

#endif
