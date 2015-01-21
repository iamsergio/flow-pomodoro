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


#include "extendedtagsmodel.h"
#include "storage.h"

ExtendedTagsModel::ExtendedTagsModel(Storage *storage, QObject *parent)
    : QAbstractListModel(parent)
    , m_sourceModel(0)
    , m_storage(storage)
{
    connect(this, &ExtendedTagsModel::rowsInserted, this, &ExtendedTagsModel::countChanged);
    connect(this, &ExtendedTagsModel::rowsRemoved, this, &ExtendedTagsModel::countChanged);
    connect(this, &ExtendedTagsModel::modelReset, this, &ExtendedTagsModel::countChanged);
    connect(this, &ExtendedTagsModel::layoutChanged, this, &ExtendedTagsModel::countChanged);
}

ExtendedTagsModel::~ExtendedTagsModel()
{
}

void ExtendedTagsModel::setSourceModel(QAbstractItemModel *model)
{
    beginResetModel();
    if (m_sourceModel) {
        disconnect(m_sourceModel, 0, this, 0);
    }
    m_sourceModel = model;
    if (model) {
        connect(model, &QAbstractItemModel::modelAboutToBeReset,
                this, &ExtendedTagsModel::onSourceModelAboutToBeReset);
        connect(model, &QAbstractItemModel::modelReset,
                this, &ExtendedTagsModel::onSourceModelReset);
        connect(model, &QAbstractItemModel::rowsAboutToBeInserted,
                this, &ExtendedTagsModel::onSourceRowsAboutToBeInserted);
        connect(model, &QAbstractItemModel::rowsInserted,
                this, &ExtendedTagsModel::onSourceRowsInserted);
        connect(model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &ExtendedTagsModel::onSourceRowsAboutToBeRemoved);
        connect(model, &QAbstractItemModel::rowsRemoved,
                this, &ExtendedTagsModel::onSourceRowsRemoved);
    }
    endResetModel();
}

int ExtendedTagsModel::rowCount(const QModelIndex &parent) const
{
    return numExtraRows() + (m_sourceModel ? m_sourceModel->rowCount(parent) : 0);
}

QVariant ExtendedTagsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= rowCount(index.parent()) || !m_sourceModel)
        return QVariant();

    const bool isExtraRow = row < numExtraRows();
    Tag::Ptr tag = isExtraRow ? m_extraTags.at(row)
                              : m_sourceModel->data(mapToSource(index), Storage::TagPtrRole).value<Tag::Ptr>();

    switch (role) {
    case Storage::TagRole:
        return QVariant::fromValue<QObject*>(tag.data());
    case Storage::TagPtrRole:
        return QVariant::fromValue<Tag::Ptr>(tag);
    };

    return QVariant();
}

QHash<int, QByteArray> ExtendedTagsModel::roleNames() const
{
    return m_sourceModel ? m_sourceModel->roleNames() : QAbstractItemModel::roleNames();
}

int ExtendedTagsModel::count() const
{
    return rowCount(QModelIndex());
}

void ExtendedTagsModel::setExtraRows(const Tag::List &extraTags)
{
    beginResetModel();
    m_extraTags = extraTags;
    endResetModel();
}

int ExtendedTagsModel::indexOf(const Tag *tag) const
{
    const int count = rowCount(QModelIndex());
    for (int i = 0; i < count; ++i) {
        Tag *t = data(index(i, 0), Storage::TagRole).value<Tag*>();
        if (t == tag)
            return i;
    }
    return -1;
}

Tag::Ptr ExtendedTagsModel::at(int idx) const
{
    return data(index(idx, 0), Storage::TagPtrRole).value<Tag::Ptr>();
}

int ExtendedTagsModel::numExtraRows() const
{
    return m_extraTags.count();
}

QModelIndex ExtendedTagsModel::mapToSource(const QModelIndex &index) const
{
    if (!m_sourceModel)
        return QModelIndex();

    return m_sourceModel->index(index.row() - numExtraRows(), 0);
}

QModelIndex ExtendedTagsModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();

    return index(sourceIndex.row() + numExtraRows(), 0);
}

void ExtendedTagsModel::onSourceModelAboutToBeReset()
{
    beginResetModel();
}

void ExtendedTagsModel::onSourceModelReset()
{
    endResetModel();
}

void ExtendedTagsModel::onSourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    beginInsertRows(parent, start + numExtraRows(), end + numExtraRows());
}

void ExtendedTagsModel::onSourceRowsInserted(const QModelIndex &, int, int)
{
    endInsertRows();
}

void ExtendedTagsModel::onSourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    beginRemoveRows(parent, start + numExtraRows(), end + numExtraRows());
}

void ExtendedTagsModel::onSourceRowsRemoved(const QModelIndex &, int, int)
{
    endRemoveRows();
}

void ExtendedTagsModel::onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}
