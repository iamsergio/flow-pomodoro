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

#include "jsonstorage.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTemporaryFile>

static QString dataFileName()
{
    static QString filename;
    if (filename.isEmpty()) {
        QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        if (!QFile::exists(directory)) {
            QDir dir(directory);
            if (!dir.mkpath(directory)) {
                qWarning() << "Failed to create directory" << directory;
                qFatal("Bailing out...");
                return QString();
            }
        }

        filename += directory + "/flow.dat";
    }

    return filename;
}

static QString tmpDataFileName()
{
    return dataFileName() + "~";
}

JsonStorage::JsonStorage(QObject *parent)
    : Storage(parent)
{
}

QPair<TagList, TaskList> JsonStorage::deserializeJsonData(const QByteArray &data)
{
    QPair<TagList, TaskList> result;
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing json file" << dataFileName();
        qWarning() << "Error was" << jsonError.errorString();
        qFatal("Bailing out");
        return result;
    }

    QVariantMap rootMap = document.toVariant().toMap();
    QVariantList tagList = rootMap.value("tags").toList();
    QVariantList taskList = rootMap.value("tasks").toList();

    int serializerVersion = rootMap.value("JsonSerializerVersion", JsonSerializerVersion1).toInt();
    Q_UNUSED(serializerVersion);

    TaskList tasks;
    TagList tags;

    foreach (const QVariant &t, tagList) {
        Tag::Ptr tag = Tag::fromJson(t.toMap());
        if (tag)
            tags << tag;
    }

    foreach (const QVariant &t, taskList) {
        Task::Ptr task = Task::fromJson(t.toMap());
        if (task)
            tasks << task;
    }

    result.first = tags;
    result.second = tasks;
    return result;
}

void JsonStorage::load_impl()
{
    if (!QFile::exists(dataFileName())) // Nothing to load
        return;

    QFile file(dataFileName());
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open data file" << dataFileName()
                   << "; error=" << file.errorString() << file.error();
        qFatal("Bailing out");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QPair<TagList, TaskList> deserializedData = deserializeJsonData(data);
    m_tags = deserializedData.first;

    m_tasks.clear();
    for (int i = 0; i < deserializedData.second.count(); ++i) {
        addTask(deserializedData.second.at(i)); // don't add to m_tasks directly. addTask() does some connects
    }
}

void JsonStorage::save_impl()
{
    QByteArray data = serializeToJsonData(QPair<TagList,TaskList>(m_tags, m_tasks));

    QFile temporaryFile(tmpDataFileName()); // not using QTemporaryFile so the backup stays next to the main one
    if (!temporaryFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open" << tmpDataFileName() << "for writing"
                   << temporaryFile.errorString() << temporaryFile.error();
        return;
    }

    QTextStream out(&temporaryFile);
    out << data;
    if (QFile::exists(dataFileName()) && !QFile::remove(dataFileName())) {
        qWarning() << "Could not update (remove error)" << dataFileName()
                   << "backup file is at" << tmpDataFileName();
        return;
    }

    if (!temporaryFile.copy(dataFileName())) {
        qWarning() << "Could not update" << dataFileName()
                   << "backup file is at" << tmpDataFileName();
        return;
    }
}

QVariantMap JsonStorage::toJsonVariantMap(const QPair<TagList,TaskList> &list)
{
    QVariantMap map;
    QVariantList tasksVariant;
    QVariantList tagsVariant;
    for (int i = 0; i < list.first.count(); ++i) {
        tagsVariant << list.first.at(i)->toJson();
    }

    for (int i = 0; i < list.second.count(); ++i) {
        tasksVariant << list.second.at(i)->toJson();
    }

    map.insert("tags", tagsVariant);
    map.insert("tasks", tasksVariant);
    map.insert("JsonSerializerVersion", JsonSerializerVersion1);

    return map;
}

QByteArray JsonStorage::serializeToJsonData(const QPair<TagList,TaskList> &dataPair)
{
    QJsonDocument document = QJsonDocument::fromVariant(toJsonVariantMap(dataPair));
    return document.toJson();
}
