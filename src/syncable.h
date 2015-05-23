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

#ifndef FLOW_SYNCABLE_H
#define FLOW_SYNCABLE_H

#include <QString>
#include <QVariantMap>

class Syncable
{
public:
    Syncable();
    virtual ~Syncable();

    int revision() const;
    int revisionOnWebDAVServer() const;
    void setRevisionOnWebDAVServer(int);
    QString uuid() const;
    void setUuid(const QString &uuid);

    virtual void fromJson(const QVariantMap &);

protected:
    virtual QVector<QString> supportedFields() const;
    bool equals(Syncable *) const;
    void setRevision(int);
    virtual QVariantMap toJson() const;
    QVariantMap m_unknownFieldsMap;
    int m_revision;
    int m_revisionOnWebDAVServer;
    mutable QString m_uuid;

private:
    void parseUnknownFields(const QVariantMap &);
};

#endif
