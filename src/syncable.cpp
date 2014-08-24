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

void Syncable::fromJson(const QVariantMap &map)
{
    QString uuid = map.value("uuid").toString();
    if (uuid.isEmpty())
        uuid = QUuid::createUuid().toString();
    setUuid(uuid);
    setRevision(map.value("revision", 0).toInt());
    setRevisionOnWebDAVServer(map.value("revisionOnWebDAVServer", -1).toInt());
}

void Syncable::setUuid(const QString &uuid)
{
    m_uuid = uuid;
}

void Syncable::setRevision(int revision)
{
    m_revision = revision;
}

QVariantMap Syncable::toJson() const
{
    QVariantMap map;
    map.insert("revision", m_revision);
    map.insert("revisionOnWebDAVServer", m_revisionOnWebDAVServer);
    map.insert("uuid", uuid());
    return map;
}
