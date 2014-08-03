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

#include "storage.h"
#include "tagstorageqsettings.h"
#include "taskstorageqsettings.h"
#include <QJsonDocument>

enum {
    JsonSerializerVersion1 = 1
};

Storage::Storage(QObject *parent)
    : QObject(parent)
    , m_tagStorage(nullptr)
    , m_taskStorage(nullptr)
{
}

Storage *Storage::instance()
{
    static Storage *storage = new Storage(qApp);
    return storage;
}

TagStorage *Storage::tagStorage()
{
    if (!m_tagStorage) { // due to deadlock in instance()
        m_tagStorage = new TagStorageQSettings(this);
        m_tagStorage->loadTags();
        if (m_tagStorage->model()->rowCount() == 0) {
            // Create default tags
            m_tagStorage->createTag(tr("work"));
            m_tagStorage->createTag(tr("personal"));
            m_tagStorage->createTag(tr("family"));
            m_tagStorage->createTag(tr("bills"));
            m_tagStorage->createTag(tr("books"));
            m_tagStorage->createTag(tr("movies"));
        }
    }

    return m_tagStorage;
}

TaskStorage *Storage::taskStorage()
{
    if (!m_taskStorage) {
        m_taskStorage = new TaskStorageQSettings(this);
        m_taskStorage->loadTasks();
    }

    return m_taskStorage;
}

QVariantMap Storage::toJson() const
{
    QVariantMap map;
    QVariantList tasksVariant;
    QVariantList tagsVariant;
    auto tags = m_tagStorage->tags();
    for (int i = 0; i < tags.count(); ++i) {
        tagsVariant << tags.at(i)->toJson();
    }

    auto tasks = m_taskStorage->tasks();
    for (int i = 0; i < tasks.count(); ++i) {
        tasksVariant << tasks.at(i)->toJson();
    }

    map.insert("tags", tagsVariant);
    map.insert("tasks", tasksVariant);
    map.insert("JsonSerializerVersion", JsonSerializerVersion1);

    return map;
}

QString Storage::toJsonString() const
{
    QJsonDocument document = QJsonDocument::fromVariant(toJson());
    return document.toJson();
}
