/*
  This file is part of Flow.

  Copyright (C) 2016 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_TAGMANAGER_H
#define FLOW_TAGMANAGER_H

#include "tag.h"

#include <QObject>

typedef GenericListModel<Tag::Ptr> TagList;
class Kernel;

class TagManager : public QObject
{
    Q_OBJECT
public:
    enum TagModelRole {
        TagRole = Qt::UserRole + 1,
        TagPtrRole,
        LastRole
    };
    explicit TagManager(Kernel *kernel, QObject *parent = nullptr);
    Tag::Ptr createTag(const QString &tagName, const QString &uid = QString());
    void createTag(const QVariantMap &);
    int indexOfTag(const QString &name) const;
    Tag::Ptr tag(const QString &name, bool create);
    bool removeTag(const QString &tagName);
    const TagList& tags() const;
    void maybeCreateDefaultTags();
    bool containsTag(const QString &name) const;
    void clearTags();
    void setTags(const QStringList &tagNames);
signals:
    void tagAboutToBeRemoved(const QString &name);
private:
    TagList m_tags;
    Kernel *const m_kernel;
};

#endif
