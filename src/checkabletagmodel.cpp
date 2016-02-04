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

#include "checkabletagmodel.h"
#include "storage.h"
#include "task.h"
#include "kernel.h"

CheckableTagModel::CheckableTagModel(Task *parent)
    : QIdentityProxyModel(parent)
    , m_parentTask(parent)
{
    Q_ASSERT(parent);
    setObjectName(QStringLiteral("CheckableTagModel"));
    connect(this, &CheckableTagModel::rowsInserted, this, &CheckableTagModel::countChanged);
    connect(this, &CheckableTagModel::rowsRemoved, this, &CheckableTagModel::countChanged);
    connect(this, &CheckableTagModel::modelReset, this, &CheckableTagModel::countChanged);
    connect(this, &CheckableTagModel::layoutChanged, this, &CheckableTagModel::countChanged);
    connect(parent, &Task::tagToggled, this, &CheckableTagModel::emitDataChanged);
}

QVariant CheckableTagModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!sourceModel() || !proxyIndex.isValid())
        return QVariant();

    QModelIndex sourceIndex = sourceModel()->index(proxyIndex.row(), 0);
    if (!sourceIndex.isValid())
        return QVariant();

    if (role == Qt::CheckStateRole) {
        Tag::Ptr tag = sourceIndex.data(Storage::TagPtrRole).value<Tag::Ptr>();
        if (!tag) {
            qWarning() << Q_FUNC_INFO <<"Unexpected null tag";
            return false;
        }

        return m_parentTask->containsTag(tag->name());
    } else if (role == CheckableRole) {
        return true;
    } else if (role == ItemTextRole)  {
        Tag::Ptr tag = sourceIndex.data(Storage::TagPtrRole).value<Tag::Ptr>();
        if (!tag) {
            qWarning() << Q_FUNC_INFO <<"Unexpected null tag";
            return QString();
        }

        return tag->name();
    }

    return sourceIndex.data(role);
}

QHash<int, QByteArray> CheckableTagModel::roleNames() const
{
    QHash<int, QByteArray> roles = QIdentityProxyModel::roleNames();
    roles.insert(Qt::CheckStateRole, "checkState");
    roles.insert(ItemTextRole, "itemText");
    roles.insert(CheckableRole, "checkable");
    return roles;
}

int CheckableTagModel::count() const
{
    return rowCount();
}

void CheckableTagModel::emitDataChanged(const QString &tagName)
{
    int count = rowCount();
    if (count == 0)
        return;

    for (int i = 0; i < count; ++i) {
        if (index(i, 0).data(Storage::TagPtrRole).value<Tag::Ptr>()->name() == tagName)
            emit dataChanged(index(i, 0), index(i, 0));
    }
}
