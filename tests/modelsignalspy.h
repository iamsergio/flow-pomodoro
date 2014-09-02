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

#ifndef MODEL_SIGNALSPY_H
#define MODEL_SIGNALSPY_H

// Better version of QSignalSpy, which supports order and multiple signals

#include <QObject>
#include <QVariant>

class QAbstractItemModel;

struct CaughtSignal {
    typedef QList<CaughtSignal> List;
    QString name;
    QVariantList args;
};

class ModelSignalSpy : public QObject
{
    Q_OBJECT
public:
    explicit ModelSignalSpy(QAbstractItemModel *model);

    int count() const;
    CaughtSignal::List caughtSignals() const;
    void clear();

    void dumpDebugInfo();

private Q_SLOTS:
    void onModelReset();
    void onModelAboutToBeReset();
    void onLayoutChanged();
    void onLayoutAboutToBeChanged();
    void onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void onRowsInserted();
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void onRowsRemoved();

private:
    CaughtSignal::List m_caughtSignals;
    QAbstractItemModel *m_model;
    QString m_expectingSignal;
};

#endif
