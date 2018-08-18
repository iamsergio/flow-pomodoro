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

#ifndef SORTED_TASK_CONTEXT_MENU_MODEL
#define SORTED_TASK_CONTEXT_MENU_MODEL

#include <QSortFilterProxyModel>

class SortedTaskContextMenuModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:

    explicit SortedTaskContextMenuModel(QObject *parent = 0);
    ~SortedTaskContextMenuModel();
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    int count() const;
    void toggleTag(int index);

Q_SIGNALS:
    void countChanged();
};

#endif
