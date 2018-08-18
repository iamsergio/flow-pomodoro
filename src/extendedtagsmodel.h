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

#ifndef EXTENDED_TAGS_MODEL_H
#define EXTENDED_TAGS_MODEL_H

#include "tag.h"

#include <QAbstractListModel>
#include <QList>

class Storage;

// This is a SortedTagsModel that also shows "All" and "Untagged" at index 0 and 1

class ExtendedTagsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit ExtendedTagsModel(Storage *, QObject *parent = nullptr);
    ~ExtendedTagsModel();
    void setSourceModel(QAbstractItemModel *);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int count() const;
    void setExtraRows(const Tag::List &);
    int indexOf(const Tag *) const;
    Tag::Ptr at(int index) const;

Q_SIGNALS:
    void countChanged();
private:
    int numExtraRows() const;
    void updateExtraRows();
    QModelIndex mapToSource(const QModelIndex &) const;
    QModelIndex mapFromSource(const QModelIndex &) const;
    void onSourceModelAboutToBeReset();
    void onSourceModelReset();
    void onSourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void onSourceRowsInserted(const QModelIndex &parent, int start, int end);
    void onSourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void onSourceRowsRemoved(const QModelIndex &parent, int start, int end);
    void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex & bottomRight);

    QAbstractItemModel *m_sourceModel;
    Storage *const m_storage;
    Tag::List m_extraTags;
};

#endif
