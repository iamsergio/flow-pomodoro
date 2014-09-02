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

#include "modelsignalspy.h"

#include <QDebug>
#include <QAbstractItemModel>
#include <QMetaMethod>
#include <QtTest/QtTest>

ModelSignalSpy::ModelSignalSpy(QAbstractItemModel *model)
    : QObject()
    , m_model(model)
{
    connect(model, &QAbstractItemModel::modelReset, this, &ModelSignalSpy::onModelReset);
    connect(model, &QAbstractItemModel::layoutChanged, this, &ModelSignalSpy::onLayoutChanged);
    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &ModelSignalSpy::onModelAboutToBeReset);
    connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this, &ModelSignalSpy::onLayoutAboutToBeChanged);
    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &ModelSignalSpy::onRowsAboutToBeInserted);
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ModelSignalSpy::onRowsAboutToBeRemoved);
    connect(model, &QAbstractItemModel::rowsInserted, this, &ModelSignalSpy::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &ModelSignalSpy::onRowsRemoved);
}

void ModelSignalSpy::clear()
{
    m_caughtSignals.clear();
}

int ModelSignalSpy::count() const
{
    return m_caughtSignals.count();
}

CaughtSignal::List ModelSignalSpy::caughtSignals() const
{
    return m_caughtSignals;
}

void ModelSignalSpy::dumpDebugInfo()
{
    foreach (const CaughtSignal &signal, m_caughtSignals) {
        qDebug() << signal.name << signal.args;
    }
}

void ModelSignalSpy::onModelReset()
{
    QCOMPARE(QString("modelReset"), QString(m_expectingSignal));
    m_expectingSignal.clear();
    CaughtSignal signal;
    signal.name = "modelReset";
    m_caughtSignals << signal;
}

void ModelSignalSpy::onModelAboutToBeReset()
{
    QCOMPARE(QString(), QString(m_expectingSignal));
    CaughtSignal signal;
    signal.name = "modelAboutToBeReset";
    m_caughtSignals << signal;
    m_expectingSignal = "modelReset";
}

void ModelSignalSpy::onLayoutChanged()
{
    QCOMPARE(QString("layoutChanged"), QString(m_expectingSignal));
    m_expectingSignal.clear();
    CaughtSignal signal;
    signal.name = "layoutChanged";
    m_caughtSignals << signal;
}

void ModelSignalSpy::onLayoutAboutToBeChanged()
{
    QCOMPARE(QString(), QString(m_expectingSignal));
    CaughtSignal signal;
    signal.name = "layoutAboutToBeChanged";
    m_caughtSignals << signal;
    m_expectingSignal = "layoutChanged";
}

void ModelSignalSpy::onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    QCOMPARE(QString(), QString(m_expectingSignal));
    QVERIFY(!parent.isValid());
    if (start < 0 || start > m_model->rowCount()) { // can be equal to model, rowcount wasn't updated yet
        qDebug() << start << end << m_model->rowCount();
        QVERIFY(false);
    }
    QVERIFY(end >= 0 && end <= m_model->rowCount());
    QVERIFY(start <= end);

    CaughtSignal signal;
    signal.name = "rowsAboutToBeInserted";
    signal.args << parent.isValid() << start << end;
    m_caughtSignals << signal;
    m_expectingSignal = "rowsInserted";
}

void ModelSignalSpy::onRowsInserted()
{
    QCOMPARE(QString("rowsInserted"), QString(m_expectingSignal));
    m_expectingSignal.clear();
    CaughtSignal signal;
    signal.name = "rowsInserted";
    m_caughtSignals << signal;
}

void ModelSignalSpy::onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QCOMPARE(QString(), QString(m_expectingSignal));
    QVERIFY(!parent.isValid());
    if (start < 0 || start > m_model->rowCount()) { // can be equal to model, rowcount wasn't updated yet
        qDebug() << start << end << m_model->rowCount();
        QVERIFY(false);
    }
    QVERIFY(end >= 0 && end <= m_model->rowCount());
    QVERIFY(start <= end);

    CaughtSignal signal;
    signal.name = "rowsAboutToBeRemoved";
    signal.args << parent.isValid() << start << end;
    m_caughtSignals << signal;
    m_expectingSignal = "rowsRemoved";
}

void ModelSignalSpy::onRowsRemoved()
{
    QCOMPARE(QString("rowsRemoved"), QString(m_expectingSignal));
    m_expectingSignal.clear();
    CaughtSignal signal;
    signal.name = "rowsRemoved";
    m_caughtSignals << signal;
}
