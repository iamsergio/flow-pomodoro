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

#include "assertingproxymodel.h"

AssertingProxyModel::AssertingProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_rowCountSourceToldUsAbout(0)
    , m_ongoingRowDelta(0)
{
    connect(this, &AssertingProxyModel::modelReset, this, &AssertingProxyModel::onModelReset);
    connect(this, &AssertingProxyModel::layoutChanged, this, &AssertingProxyModel::onLayoutChanged);
    connect(this, &AssertingProxyModel::modelAboutToBeReset, this, &AssertingProxyModel::onModelAboutToBeReset);
    connect(this, &QAbstractItemModel::layoutAboutToBeChanged, this, &AssertingProxyModel::onLayoutAboutToBeChanged);
    connect(this, &AssertingProxyModel::rowsAboutToBeInserted, this, &AssertingProxyModel::onRowsAboutToBeInserted);
    connect(this, &AssertingProxyModel::rowsAboutToBeRemoved, this, &AssertingProxyModel::onRowsAboutToBeRemoved);
    connect(this, &AssertingProxyModel::rowsInserted, this, &AssertingProxyModel::onRowsInserted);
    connect(this, &AssertingProxyModel::rowsRemoved, this, &AssertingProxyModel::onRowsRemoved);
    connect(this, &AssertingProxyModel::dataChanged, this, &AssertingProxyModel::onDataChanged);
}

void AssertingProxyModel::onModelAboutToBeReset()
{
    Q_ASSERT(m_expectingSignal.isEmpty());
    m_expectingSignal = "modelReset";
    assertRowCount();
}

void AssertingProxyModel::onModelReset()
{
    Q_ASSERT(m_expectingSignal == QLatin1String("modelReset"));
    m_expectingSignal.clear();
    m_rowCountSourceToldUsAbout = sourceModel()->rowCount();
}

void AssertingProxyModel::onLayoutAboutToBeChanged()
{
    Q_ASSERT(m_expectingSignal.isEmpty());
    m_expectingSignal = "layoutChanged";
    assertRowCount();
}

void AssertingProxyModel::onLayoutChanged()
{
    Q_ASSERT(m_expectingSignal == QLatin1String("layoutChanged"));
    m_expectingSignal.clear();
    m_rowCountSourceToldUsAbout = sourceModel()->rowCount();
}

void AssertingProxyModel::onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_ASSERT(m_expectingSignal.isEmpty());
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(start <= end);
    Q_ASSERT(start >= 0);
    Q_ASSERT(start <= sourceModel()->rowCount());

    m_expectingSignal = "rowsInserted";
    assertRowCount();
    m_ongoingRowDelta = end - start + 1;
}

void AssertingProxyModel::onRowsInserted()
{
    Q_ASSERT(QLatin1String("rowsInserted") == m_expectingSignal);
    m_expectingSignal.clear();
    m_rowCountSourceToldUsAbout += m_ongoingRowDelta;
    assertRowCount();
}

void AssertingProxyModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_ASSERT(m_expectingSignal.isEmpty());
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(start <= end);
    Q_ASSERT(start >= 0);
    Q_ASSERT(end < sourceModel()->rowCount());

    m_expectingSignal = "rowsRemoved";
    assertRowCount();
    m_ongoingRowDelta = end - start + 1;
}

void AssertingProxyModel::onRowsRemoved()
{
    Q_ASSERT(QLatin1String("rowsRemoved") == m_expectingSignal);
    m_expectingSignal.clear();
    m_rowCountSourceToldUsAbout -= m_ongoingRowDelta;
    assertRowCount();
}

void AssertingProxyModel::onDataChanged(const QModelIndex &left, const QModelIndex &right)
{
    Q_UNUSED(left);
    Q_UNUSED(right);

    Q_ASSERT(m_expectingSignal.isEmpty());
    Q_ASSERT(left.isValid());
    Q_ASSERT(right.isValid());
    Q_ASSERT(left.row() <= right.row());
    Q_ASSERT(right.row() < sourceModel()->rowCount());
    assertRowCount();
}

void AssertingProxyModel::assertRowCount()
{
    if (sourceModel())
        Q_ASSERT(m_rowCountSourceToldUsAbout == sourceModel()->rowCount());
}
