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

#ifndef TASKFILTERPROXYMODEL_H
#define TASKFILTERPROXYMODEL_H

#include "task.h"
#include <QSortFilterProxyModel>

class TaskFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit TaskFilterProxyModel(QObject *parent = 0);

    int count() const;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;
    void setTagName(const QString &);
    void setFilterUntagged(bool filter);
    void setFilterDueDated(bool filter);
    void invalidateFilter();
    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void countChanged(int count, int previousCount);
private Q_SLOTS:
    void onSourceCountChanged();

private:
    bool defaultLessThan(const Task::Ptr &leftTask, const Task::Ptr &rightTask) const;
    QString m_tagText;
    bool m_filterUntagged;
    int m_previousCount;
    bool m_filterDueDated;
};

#endif
