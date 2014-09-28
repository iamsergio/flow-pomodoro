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
#include "syncable.h"

#include <QString>
#include <QSharedPointer>

class TaskFilterProxyModel;
class Kernel;

class Tag : public QObject, public Syncable
{
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(int taskCount READ taskCount NOTIFY taskCountChanged STORED false)
    Q_PROPERTY(bool beingEdited READ beingEdited NOTIFY beingEditedChanged STORED false)
    Q_PROPERTY(QAbstractItemModel* taskModel READ taskModel CONSTANT)
    Q_OBJECT
public:
    typedef QSharedPointer<Tag> Ptr;
    typedef GenericListModel<Tag::Ptr> List;

    explicit Tag(Kernel *kernel, const QString &name);
    ~Tag();

    int taskCount() const;
    void incrementTaskCount(int increment);
    QString name() const;
    void setName(const QString &name);
    bool beingEdited() const;
    void setBeingEdited(bool);

    QAbstractItemModel* taskModel();
    QVariantMap toJson() const;
    void fromJson(const QVariantMap &) Q_DECL_OVERRIDE;

    bool operator==(const Tag &other) const;
    Kernel *kernel() const;
    void setKernel(Kernel *kernel);

#if defined(QT_TESTLIB_LIB)
    static int tagCount;
#endif

public Q_SLOTS:
    void onTaskStagedChanged();

Q_SIGNALS:
    void nameChanged();
    void taskCountChanged(int oldValue, int newValue);
    void beingEditedChanged();

private:
    Tag();
    Tag(const Tag &other);
    QString m_name;
    int m_taskCount;
    bool m_beingEdited;
    TaskFilterProxyModel *m_taskModel; // All unstaged tasks with this tag
    Kernel *m_kernel;
    bool m_dontUpdateRevision;
};

bool operator==(const Tag::Ptr &, const Tag::Ptr &);

inline QDebug operator<<(QDebug dbg, const Tag::Ptr &tag)
{
    if (!tag)
        return dbg;

    dbg.nospace() << "Tag: uuid=" << tag->uuid()
                  << "; name=" << tag->name()
                  << "; rev=" << tag->revision();
    return dbg.space();
}

#endif
