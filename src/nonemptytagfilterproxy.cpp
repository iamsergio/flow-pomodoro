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

#include "nonemptytagfilterproxy.h"
#include "storage.h"
#include "taskfilterproxymodel.h"
#include <QDebug>

NonEmptyTagFilterProxy::NonEmptyTagFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &NonEmptyTagFilterProxy::rowsInserted,
            this, &NonEmptyTagFilterProxy::countChanged);
    connect(this, &NonEmptyTagFilterProxy::rowsRemoved,
            this, &NonEmptyTagFilterProxy::countChanged);
    connect(this, &NonEmptyTagFilterProxy::modelReset,
            this, &NonEmptyTagFilterProxy::countChanged);
    connect(this, &NonEmptyTagFilterProxy::layoutChanged,
            this, &NonEmptyTagFilterProxy::countChanged);
    setObjectName("NonEmptyTagFilterProxy");
}

int NonEmptyTagFilterProxy::count() const
{
    return rowCount();
}

bool NonEmptyTagFilterProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    if (source_parent.isValid() || source_row < 0 || source_row >= sourceModel()->rowCount()) {
        qWarning() << source_parent.isValid() << source_row << sourceModel()->rowCount();
        Q_ASSERT(false);
        return false;
    }

    QModelIndex index = sourceModel()->index(source_row, 0);
    if (!index.isValid())
        return false;

    Tag::Ptr tag = index.data(Storage::TagPtrRole).value<Tag::Ptr>();
    Q_ASSERT(tag);
    if (!tag) // null tag means it's "All" or "Untagged" special tags
        return false;

    connect(static_cast<TaskFilterProxyModel*>(tag.data()->taskModel()), &TaskFilterProxyModel::countChanged,
            this, &NonEmptyTagFilterProxy::invalidate, Qt::UniqueConnection);

    return tag->taskModel()->rowCount() > 0;
}
