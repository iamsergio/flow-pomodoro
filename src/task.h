/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "tag.h"
#include "genericlistmodel.h"

#include <QString>
#include <QMetaType>
#include <QDataStream>
#include <QSharedPointer>

enum TaskStatus {
    TaskStopped,
    TaskPaused,
    TaskStarted
};

enum SerializerVersion {
    SerializerVersion1 = 100
};

class QAbstractListModel;

class Task : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QObject * tagModel READ tagModel CONSTANT)
public:
    typedef QSharedPointer<Task> Ptr;
    typedef GenericListModel<Ptr> List;
    Task(const QString &name = QString());

    QString text() const;
    void setText(const QString &text);

    TagRef::List tags() const;
    void setTagList(const TagRef::List &);
    QAbstractListModel* tagModel() const;

    Q_INVOKABLE void addTag(const QString &tagName);
    Q_INVOKABLE void removeTag(const QString &tagName);

Q_SIGNALS:
    void textChanged();
    void tagsChanged();
    void changed();

private:
    QString m_text;
    TagRef::List m_tags;
};

QDataStream &operator<<(QDataStream &out, const Task::Ptr &task);
QDataStream &operator>>(QDataStream &in, Task::Ptr &task);

Q_DECLARE_METATYPE(Task::Ptr)
Q_DECLARE_METATYPE(Task::List)

#endif
