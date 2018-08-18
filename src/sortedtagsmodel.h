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

#ifndef SORTEDTAGSMODEL_H
#define SORTEDTAGSMODEL_H

#include <QSortFilterProxyModel>
class Tag;

class SortedTagsModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit SortedTagsModel(QAbstractItemModel *source, QObject *parent = nullptr);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    int count() const;
    Q_INVOKABLE Tag* at(int index) const; // qml convinience

Q_SIGNALS:
    void countChanged();
};

#endif
