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

#ifndef FLOW_TAG_H
#define FLOW_TAG_H

#include "genericlistmodel.h"

#include <QString>
#include <QSharedPointer>

class TaskFilterProxyModel;

class Tag : public QObject
{
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(int taskCount READ taskCount NOTIFY taskCountChanged STORED false)
    Q_PROPERTY(int archivedTaskCount READ archivedTaskCount NOTIFY archivedTaskCountChanged STORED false)
    Q_PROPERTY(bool beingEdited READ beingEdited NOTIFY beingEditedChanged STORED false)
    Q_PROPERTY(QAbstractItemModel* taskModel READ taskModel CONSTANT)
    Q_OBJECT
public:
    typedef QSharedPointer<Tag> Ptr;
    typedef GenericListModel<Tag::Ptr> List;

    explicit Tag(const QString &name);
    ~Tag();

    int taskCount() const;
    void setTaskCount(int count);
    int archivedTaskCount() const;
    void setArchivedTaskCount(int count);
    QString name() const;
    void setName(const QString &name);
    bool beingEdited() const;
    void setBeingEdited(bool);

    QAbstractItemModel* taskModel();
    QVariantMap toJson() const;

public Q_SLOTS:
    void onTaskStagedChanged();

Q_SIGNALS:
    void nameChanged();
    void taskCountChanged(int oldValue, int newValue);
    void archivedTaskCountChanged(int oldValue, int newValue);
    void beingEditedChanged();

private:
    Tag();
    Tag(const Tag &other);
    QString m_name;
    int m_taskCount;
    int m_archivedTaskCount;
    bool m_beingEdited;
    TaskFilterProxyModel *m_taskModel; // All unstaged tasks with this tag
};

#endif
