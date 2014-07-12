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

#include "task.h"
#include "settings.h"

#include <QQmlEngine>

#include <functional>
#include <algorithm>

Task::Task(const QString &name)
    : QObject()
    , m_summary(name.isEmpty() ? tr("New Task") : name)
{
    m_tags.insertRole("tag", [&](int i) { return QVariant::fromValue<Tag*>(m_tags.at(i).m_tag.data()); });

    connect(this, &Task::summaryChanged, &Task::changed);
    connect(this, &Task::tagsChanged, &Task::changed);
    connect(this, &Task::descriptionChanged, &Task::changed);
    connect(m_tags, &QAbstractListModel::modelReset, this, &Task::tagsChanged);
    connect(m_tags, &QAbstractListModel::rowsInserted, this, &Task::tagsChanged);
    connect(m_tags, &QAbstractListModel::rowsRemoved, this, &Task::tagsChanged);
    connect(m_tags, &QAbstractListModel::layoutChanged, this, &Task::tagsChanged);
    connect(m_tags, &QAbstractListModel::dataChanged, this, &Task::tagsChanged);

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

QString Task::summary() const
{
    return m_summary;
}

void Task::setSummary(const QString &text)
{
    if (m_summary != text) {
        m_summary = text.isEmpty() ? tr("New Task") : text;
        emit summaryChanged();
    }
}

QString Task::description() const
{
    return m_description;
}

void Task::setDescription(const QString &text)
{
    if (m_description != text) {
        m_description = text;
        emit descriptionChanged();
    }
}

TagRef::List Task::tags() const
{
    return m_tags;
}

void Task::setTagList(const TagRef::List &list)
{
    m_tags = list;
}

QAbstractListModel *Task::tagModel() const
{
    return m_tags;
}

void Task::addTag(const QString &tagName)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty())
        return;

    auto it = std::find_if(m_tags.cbegin(), m_tags.cend(),
                           [&](const TagRef &ref) { return ref.m_tag->name() == trimmedName; });

    if (it == m_tags.cend()) {
        m_tags.append(TagRef(this, trimmedName));
    }
}

void Task::removeTag(const QString &tagName)
{
    auto it = std::find_if(m_tags.cbegin(), m_tags.cend(),
                           [&](const TagRef &ref) { return ref.m_tag->name() == tagName; });

    if (it != m_tags.cend()) {
        m_tags.removeAt(it - m_tags.cbegin());
    }
}

QDataStream &operator<<(QDataStream &out, const Task::Ptr &task)
{
    Q_ASSERT(task);
    quint32 version = SerializerVersion2;
    out << version << task->summary();

    TagRef::List tags = task->tags();
    out << tags.count();
    for (int i = 0; i < tags.count(); ++i)
        out << tags.at(i).m_tag->name();

    out << task->description();
    return out;
}

QDataStream &operator>>(QDataStream &in, Task::Ptr &task)
{
    if (!task) {
        task = Task::Ptr(new Task());
    }

    quint32 version = 0;
    in >> version;

    QString summary;
    QString description;
    TagRef::List tags;

    switch (version) {
    case SerializerVersion1:
    case SerializerVersion2:
        int tagCount;
        in >> summary >> tagCount;
        for (int i = 0; i < tagCount; i++) {
            QString name;
            in >> name;
            tags << TagRef(task.data(), name);
        }
        break;

    default:
        Q_ASSERT(false);
    }

    if (version == SerializerVersion2) {
        in >> description;
    }

    task->setSummary(summary);
    task->setDescription(description);
    task->setTagList(tags);
    return in;
}
