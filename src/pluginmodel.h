/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

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

#ifndef PLUGIN_MODEL_H
#define PLUGIN_MODEL_H

#include "plugininterface.h"

#include <QAbstractListModel>

class Kernel;

class PluginModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:

    enum Roles {
        TextRole = Qt::UserRole + 1,
        HelpTextRole,
        EnabledRole,
        ObjectRole,
        ConfigItemRole
    };

    explicit PluginModel(Kernel *kernel, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int,QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int count() const;
    PluginInterface *at(int index) const; //shortcut
    void addPlugin(PluginInterface *plugin);
    PluginInterface::List plugins() const;

    Q_INVOKABLE void setPluginEnabled(bool enabled, int index);

Q_SIGNALS:
    void countChanged();

private:
    PluginInterface::List m_plugins;
    Kernel *m_kernel;
};

#endif
