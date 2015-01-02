/*
  This file is part of Flow.

  Copyright (C) 2015 Sérgio Martins <iamsergio@gmail.com>

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

#include "sortedtaskcontextmenumodel.h"
#include "taskcontextmenumodel.h"

SortedTaskContextMenuModel::SortedTaskContextMenuModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &SortedTaskContextMenuModel::rowsInserted, this, &SortedTaskContextMenuModel::countChanged);
    connect(this, &SortedTaskContextMenuModel::rowsRemoved, this, &SortedTaskContextMenuModel::countChanged);
    connect(this, &SortedTaskContextMenuModel::modelReset, this, &SortedTaskContextMenuModel::countChanged);
    connect(this, &SortedTaskContextMenuModel::layoutChanged, this, &SortedTaskContextMenuModel::countChanged);
    setDynamicSortFilter(true);
    sort(Qt::AscendingOrder);
}

SortedTaskContextMenuModel::~SortedTaskContextMenuModel()
{
}

bool SortedTaskContextMenuModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool leftIsCheckable = left.data(TaskContextMenuModel::CheckableRole).toBool();
    bool rightIsCheckable = right.data(TaskContextMenuModel::CheckableRole).toBool();

    if (leftIsCheckable && !rightIsCheckable)
        return false;

    if (!leftIsCheckable && rightIsCheckable)
        return true;

    if (leftIsCheckable) {
        return left.data(TaskContextMenuModel::TextRole).toString().localeAwareCompare(right.data(TaskContextMenuModel::TextRole).toString()) < 0;
    } else {
        return left.row() < right.row();
    }


    //QString leftText = left.data();
    Q_UNUSED(right);
    return true;
}

int SortedTaskContextMenuModel::count() const
{
    return rowCount();
}

void SortedTaskContextMenuModel::toggleTag(int idx)
{
    TaskContextMenuModel *src = qobject_cast<TaskContextMenuModel*>(sourceModel());
    if (src) {
        src->toggleTag(mapToSource(index(idx, 0)).row());
    }
}
