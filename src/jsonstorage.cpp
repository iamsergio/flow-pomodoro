#include "jsonstorage.h"

#include <QJsonDocument>

JsonStorage::JsonStorage(QObject *parent)
    : Storage(parent)
{
}

void JsonStorage::load_impl()
{

}

void JsonStorage::save_impl()
{

}

QVariantMap JsonStorage::toJson() const
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

QString JsonStorage::toJsonString() const
{
    QJsonDocument document = QJsonDocument::fromVariant(toJson());
    return document.toJson();
}
