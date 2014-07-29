/*
  This file is part of Flow.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TAGSTORAGE_H
#define TAGSTORAGE_H

#include "tag.h"
#include "genericlistmodel.h"
#include <QObject>
#include <QTimer>

class SortedTagsModel;

class TagStorage : public QObject
{
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_OBJECT
public:
    enum {
        TagRole = Qt::UserRole + 1,
        TagPtrRole,
        LastRole
    };

    static TagStorage *instance();

    /**
     * Calls saveTags() in the next event loop processing.
     */
    void scheduleSaveTags();

    /**
     * Reads tags from disk.
     */
    void loadTags();

    Q_INVOKABLE bool removeTag(const QString &tagName);
    Q_INVOKABLE Tag::Ptr createTag(const QString &tagName);

    Tag::Ptr tag(const QString &name, bool create = true);
    bool contains(const QString &name) const;

    QAbstractItemModel *model() const;

    QString deletedTagName() const;

    void monitorTag(Tag *tag);

Q_SIGNALS:
    void tagAboutToBeRemoved(const QString &name);

public Q_SLOTS:
    bool renameTag(const QString &oldName, const QString &newName);

protected:
    explicit TagStorage(QObject *parent = 0);
    void setTags(const Tag::List &);
    GenericListModel<Tag::Ptr> m_tags;

    virtual void saveTags_impl() = 0;
    virtual void loadTags_impl() = 0;

private:
    int indexOf(const QString &name) const;
    void saveTags();

    QTimer m_scheduleTimer;
    SortedTagsModel *m_sortModel;
    QString m_deletedTagName;
    bool m_savingDisabled;
};

#endif
