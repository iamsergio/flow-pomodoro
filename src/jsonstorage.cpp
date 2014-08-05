#include "jsonstorage.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTemporaryFile>

enum {
    JsonSerializerVersion1 = 1
};

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

void JsonStorage::load_impl()
{
    QFile file(dataFileName());
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open data file" << dataFileName();
        qFatal("Bailing out");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing json file" << dataFileName();
        qWarning() << "Error was" << jsonError.errorString();
        qFatal("Bailing out");
        return;
    }

    QVariantMap rootMap = document.toVariant().toMap();
    QVariantList tagList = rootMap.value("tags").toList();
    QVariantList taskList = rootMap.value("tasks").toList();

    int serializerVersion = rootMap.value("JsonSerializerVersion", JsonSerializerVersion1).toInt();
    Q_UNUSED(serializerVersion);

    m_tags.clear();
    m_tasks.clear();
    foreach (const QVariant &t, tagList) {
        Tag::Ptr tag = Tag::fromJson(t.toMap());
        if (tag)
            m_tags << tag;
    }

    foreach (const QVariant &t, taskList) {
        Task::Ptr task = Task::fromJson(t.toMap());
        if (task)
            m_tasks << task;
    }
}

void JsonStorage::save_impl()
{
    QByteArray data = toJson();

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

QVariantMap JsonStorage::toJsonVariantMap() const
{
    QVariantMap map;
    QVariantList tasksVariant;
    QVariantList tagsVariant;
    for (int i = 0; i < m_tags.count(); ++i) {
        tagsVariant << m_tags.at(i)->toJson();
    }

    for (int i = 0; i < m_tasks.count(); ++i) {
        tasksVariant << m_tasks.at(i)->toJson();
    }

    map.insert("tags", tagsVariant);
    map.insert("tasks", tasksVariant);
    map.insert("JsonSerializerVersion", JsonSerializerVersion1);

    return map;
}

QByteArray JsonStorage::toJson() const
{
    QJsonDocument document = QJsonDocument::fromVariant(toJsonVariantMap());
    return document.toJson();
}
