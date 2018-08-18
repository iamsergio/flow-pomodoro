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
#include "runtimeconfiguration.h"

class Kernel;

class JsonStorage : public Storage
{
    Q_OBJECT
public:
    explicit JsonStorage(Kernel *kernel, QObject *parent);
    ~JsonStorage();

    static Data deserializeJsonData(const QByteArray &serializedData, QString &error,
                                    Kernel *kernel);
    static QByteArray serializeToJsonData(const Storage::Data &, const TagList &tags);

protected:
    void load_impl() override;
    void save_impl() override;

private:
    static QVariantMap toJsonVariantMap(const Storage::Data &, const TagList &tags);
    const RuntimeConfiguration m_runtimeConfiguration;
};

#endif
