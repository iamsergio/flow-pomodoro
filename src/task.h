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
#include "tagref.h"
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
    SerializerVersion1 = 100,
    SerializerVersion2 = 101 // Added Task::description()
};

namespace FunctionalModels {
class Transform;
};

class QAbstractListModel;

class Task : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QObject * tagModel READ tagModel CONSTANT)
    Q_PROPERTY(QObject * checkableTagModel READ checkableTagModel CONSTANT)
public:
    typedef QSharedPointer<Task> Ptr;
    typedef GenericListModel<Ptr> List;
    Task(const QString &name = QString());

    QString summary() const;
    void setSummary(const QString &text);

    QString description() const;
    void setDescription(const QString &text);

    bool containsTag(const QString &name) const;
    TagRef::List tags() const;
    void setTagList(const TagRef::List &);
    QAbstractItemModel *tagModel() const;
    QAbstractItemModel *checkableTagModel() const;

    Q_INVOKABLE void addTag(const QString &tagName);
    Q_INVOKABLE void removeTag(const QString &tagName);

Q_SIGNALS:
    void summaryChanged();
    void descriptionChanged();
    void tagsChanged();
    void changed();

private:
    QString m_summary;
    QString m_description;
    TagRef::List m_tags;
    FunctionalModels::Transform *m_checkableTagModel;
};

QDataStream &operator<<(QDataStream &out, const Task::Ptr &task);
QDataStream &operator>>(QDataStream &in, Task::Ptr &task);

Q_DECLARE_METATYPE(Task::Ptr)
Q_DECLARE_METATYPE(Task::List)

#endif
