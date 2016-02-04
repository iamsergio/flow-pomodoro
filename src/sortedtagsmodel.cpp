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

#include "sortedtagsmodel.h"
#include "storage.h"
#include "tag.h"

SortedTagsModel::SortedTagsModel(QAbstractItemModel *source, QObject *parent) :
    QSortFilterProxyModel(parent)
{
    setObjectName(QStringLiteral("SortedTagsModel"));
    Q_ASSERT(source);
    setSourceModel(source);

    sort(Qt::AscendingOrder);

    connect(this, &SortedTagsModel::rowsInserted, this, &SortedTagsModel::countChanged);
    connect(this, &SortedTagsModel::rowsRemoved, this, &SortedTagsModel::countChanged);
    connect(this, &SortedTagsModel::modelReset, this, &SortedTagsModel::countChanged);
    connect(this, &SortedTagsModel::layoutChanged, this, &SortedTagsModel::countChanged);
}

bool SortedTagsModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    Tag::Ptr leftTag = left.data(Storage::TagPtrRole).value<Tag::Ptr>();
    Tag::Ptr rightTag = right.data(Storage::TagPtrRole).value<Tag::Ptr>();
    Q_ASSERT(leftTag);
    Q_ASSERT(rightTag);

    if (!leftTag || !rightTag)
        return false;

    if (leftTag->taskCount() == 0 && rightTag->taskCount() != 0)
        return false;

    if (leftTag->name() != rightTag->name())
        return QString::compare(leftTag->name(), rightTag->name(), Qt::CaseInsensitive) < 0;

    return leftTag->taskCount() < rightTag->taskCount();
}

int SortedTagsModel::count() const
{
    return rowCount();
}

Tag* SortedTagsModel::at(int idx) const
{
    QVariant variant = data(index(idx, 0), Storage::TagPtrRole);
    return variant.value<Tag::Ptr>().data();
}
