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

#ifndef _TASK_H_
#define _TASK_H_

#include <QList>
#include <QString>
#include <QMetaType>
#include <QDataStream>


enum TaskStatus {
    TaskStopped,
    TaskPaused,
    TaskStarted
};

struct Task {
    typedef QList<Task> List;
    QString text;
};

Q_DECLARE_METATYPE(Task)
Q_DECLARE_METATYPE(Task::List)

#endif
