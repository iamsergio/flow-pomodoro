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

#include "syncable.h"
#include <QUuid>
#include <QVariantMap>
#include <QVector>
#include <QDebug>

Syncable::Syncable()
    : m_revision(0)
    , m_revisionOnWebDAVServer(-1)
{
}

Syncable::~Syncable()
{
}

int Syncable::revision() const
{
    return m_revision;
}

int Syncable::revisionOnWebDAVServer() const
{
    return m_revisionOnWebDAVServer;
}

void Syncable::setRevisionOnWebDAVServer(int revision)
{
    m_revisionOnWebDAVServer = revision;
}

QString Syncable::uuid() const
{
    if (m_uuid.isEmpty()) // Delayed creation, since it can be expensive and we don't need to create it in CTOR because it's going to be set when loading
        m_uuid = QUuid::createUuid().toString();

    return m_uuid;
}

void Syncable::parseUnknownFields(const QVariantMap &map)
{
    m_unknownFieldsMap.clear();
    QVector<QString> goodFields = supportedFields();
    QVariantMap::const_iterator it = map.cbegin();
    QVariantMap::const_iterator end = map.cend();
    for (; it != end; ++it) {
        if (!goodFields.contains(it.key())) {
            m_unknownFieldsMap.insert(it.key(), it.value());
            qWarning() << "Syncable::parseUnknownFields() Unknown field:" << it.key() << "; will be read-only";
        }
    }
}

void Syncable::fromJson(const QVariantMap &map)
{
    parseUnknownFields(map);

    QString uuid = map.value(QStringLiteral("uuid")).toString();
    if (uuid.isEmpty())
        uuid = QUuid::createUuid().toString();
    setUuid(uuid);
    setRevision(map.value(QStringLiteral("revision"), 0).toInt());
    setRevisionOnWebDAVServer(map.value(QStringLiteral("revisionOnWebDAVServer"), -1).toInt());
}

void Syncable::setUuid(const QString &uuid)
{
    m_uuid = uuid;
}

bool Syncable::equals(Syncable *other) const
{
    return other && m_uuid == other->uuid();
}

void Syncable::setRevision(int revision)
{
    m_revision = revision;
}

QVariantMap Syncable::toJson() const
{
    QVariantMap map;
    map.insert(QStringLiteral("revision"), m_revision);
    map.insert(QStringLiteral("revisionOnWebDAVServer"), m_revisionOnWebDAVServer);
    map.insert(QStringLiteral("uuid"), uuid());

    QVariantMap::const_iterator it = m_unknownFieldsMap.cbegin();
    QVariantMap::const_iterator end = m_unknownFieldsMap.cend();
    for (; it != end; ++it)
        map.insert(it.key(), it.value());

    return map;
}

QVector<QString> Syncable::supportedFields() const
{
    static QVector<QString> fields;
    if (fields.isEmpty()) {
        fields << QStringLiteral("uuid")                     // since 0.9
               << QStringLiteral("revision")                 // since 0.9
               << QStringLiteral("revisionOnWebDAVServer");  // since 0.9
    }

    return fields;
}
