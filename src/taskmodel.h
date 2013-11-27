/*
  This file is part of Flow.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

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

#ifndef _TASK_MODEL_H
#define _TASK_MODEL_H

#include "task.h"

#include <QAbstractListModel>
#include <QList>

class TaskModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:

    enum Roles {
        TextRole = Qt::UserRole + 1
    };

    explicit TaskModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    int count() const;

    Task at(int index) const; // Shortcut

    void addTask(const QString &taskText);
    void removeTask(int index);
    void updateTask(int index, const QString &newText);

Q_SIGNALS:
    void countChanged(int);

private:
    void populate();
    Task::List m_tasks;
};

#endif
