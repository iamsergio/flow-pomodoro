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

#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include "storage.h"

class JsonStorage : public Storage
{
    Q_OBJECT
public:
    explicit JsonStorage(QObject *parent);

    static Data deserializeJsonData(const QByteArray &serializedData, QString &error, Storage *storage = 0);
    static QByteArray serializeToJsonData(const Storage::Data &);

protected:
    void load_impl() Q_DECL_OVERRIDE;
    void save_impl() Q_DECL_OVERRIDE;

private:
    static QVariantMap toJsonVariantMap(const Storage::Data &);
};

#endif
