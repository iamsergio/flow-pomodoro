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

#ifndef ASSERTINGPROXYMODEL_H
#define ASSERTINGPROXYMODEL_H

#include <QIdentityProxyModel>

class AssertingProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit AssertingProxyModel(QObject *parent = 0);

    void setSourceModel(QAbstractItemModel *model) override;

private Q_SLOTS:
    void onModelReset();
    void onModelAboutToBeReset();
    void onLayoutChanged();
    void onLayoutAboutToBeChanged();
    void onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void onRowsInserted();
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void onRowsRemoved();
    void onDataChanged(const QModelIndex &left, const QModelIndex &right);

private:
    void assertRowCount();
    QString m_expectingSignal;
    int m_rowCountSourceToldUsAbout;
    int m_ongoingRowDelta;
};

#endif
