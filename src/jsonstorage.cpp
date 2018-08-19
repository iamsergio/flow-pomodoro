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
#include "kernel.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QTemporaryFile>

JsonStorage::JsonStorage(Kernel *kernel, QObject *parent)
    : Storage(kernel, parent)
    , m_runtimeConfiguration(kernel->runtimeConfiguration())
{
}

JsonStorage::~JsonStorage()
{
    if (saveScheduled() && m_runtimeConfiguration.saveEnabled())
        save_impl();
}

Storage::Data JsonStorage::deserializeJsonData(const QByteArray &serializedData,
                                               QString &errorMsg, Kernel *kernel)
{
    Data result;
    errorMsg.clear();
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(serializedData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        errorMsg = jsonError.errorString();
        return result;
    }

    QVariantMap rootMap = document.toVariant().toMap();
    QVariantList tagList = rootMap.value(QStringLiteral("tags")).toList();
    QVariantList taskList = rootMap.value(QStringLiteral("tasks")).toList();
    result.instanceId = rootMap.value(QStringLiteral("instanceId")).toByteArray();
    if (result.instanceId.isEmpty())
        result.instanceId = QUuid::createUuid().toByteArray();

    int serializerVersion = rootMap.value(QStringLiteral("JsonSerializerVersion"), JsonSerializerVersion1).toInt();
    if (serializerVersion > result.serializerVersion) {
        errorMsg = QStringLiteral("Found serializer version %1 which is bigger than %2. Update your application").arg(serializerVersion).arg(result.serializerVersion);
        return result;
    }

    foreach (const QVariant &t, tagList)
        kernel->tagManager()->createTag(t.toMap());

    foreach (const QVariant &t, taskList) {
        Task::Ptr task = Task::createTask(kernel);
        Q_ASSERT(task);
        task->fromJson(t.toMap());
        if (task)
            result.tasks << task;
    }

    return result;
}

void JsonStorage::load_impl()
{
    const QString dataFileName = dataFile();
    qDebug() << "JsonStorage::load_impl Loading from" << dataFileName;
    if (!QFile::exists(dataFileName)) { // Nothing to load
        qDebug() << "JsonStorage::load_impl():" << dataFileName << "does not exist yet.";
        return;
    }

    QFile file(dataFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open data file" << dataFileName
                   << "; error=" << file.errorString() << file.error();
        qFatal("Bailing out");
        return;
    }

    QByteArray serializedData = file.readAll();
    file.close();

    QString errorMsg;
    Data data = deserializeJsonData(serializedData, errorMsg, m_kernel);

    if (!errorMsg.isEmpty()) {
        qWarning() << "Error parsing json file" << dataFileName;
        qWarning() << "Error was" << errorMsg;
        qFatal("Bailing out");
        return;
    }

    m_data.tags = data.tags;
    m_data.instanceId = data.instanceId;

    m_data.tasks.clear();
    for (const auto &task : data.tasks) {
        addTask(task); // don't add to m_tasks directly. addTask() does some connects
    }
}

void JsonStorage::save_impl()
{
    QByteArray serializedData = serializeToJsonData(m_data, m_kernel->tagManager()->tags());
    const QString dataFileName = dataFile();
    const QString tmpDataFileName = dataFileName + "~";

    QFile temporaryFile(tmpDataFileName); // not using QTemporaryFile so the backup stays next to the main one
    if (!temporaryFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open" << tmpDataFileName << "for writing"
                   << temporaryFile.errorString() << temporaryFile.error();
        return;
    }

    temporaryFile.write(serializedData, serializedData.count());
    if (QFile::exists(dataFileName) && !QFile::remove(dataFileName)) {
        qWarning() << "Could not update (remove error)" << dataFileName
                   << "backup file is at" << tmpDataFileName;
        return;
    }

    if (!temporaryFile.copy(dataFileName)) {
        qWarning() << "Could not update" << dataFileName
                   << "backup file is at" << tmpDataFileName;
        return;
    }
}

QVariantMap JsonStorage::toJsonVariantMap(const Data &data, const TagList &tags)
{
    QVariantMap map;
    QVariantList tasksVariant;
    QVariantList tagsVariant;

    tagsVariant.reserve(tags.count());
    for (const auto &tag : tags) {
        tagsVariant << tag->toJson();
    }

    tasksVariant.reserve(data.tasks.count());
    for (const auto &task : data.tasks) {
        tasksVariant << task->toJson();
    }

    map.insert(QStringLiteral("instanceId"), data.instanceId);
    map.insert(QStringLiteral("tags"), tagsVariant);
    map.insert(QStringLiteral("tasks"), tasksVariant);
    map.insert(QStringLiteral("JsonSerializerVersion"), data.serializerVersion);

    return map;
}

QByteArray JsonStorage::serializeToJsonData(const Data &data, const TagList &tags)
{
    QJsonDocument document = QJsonDocument::fromVariant(toJsonVariantMap(data, tags));
    return document.toJson();
}
