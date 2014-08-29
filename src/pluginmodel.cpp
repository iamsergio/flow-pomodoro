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

#include "pluginmodel.h"
#include "settings.h"
#include "kernel.h"

PluginModel::PluginModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(this, &PluginModel::rowsInserted, this, &PluginModel::countChanged);
    connect(this, &PluginModel::rowsRemoved, this, &PluginModel::countChanged);
    connect(this, &PluginModel::modelReset, this, &PluginModel::countChanged);
    connect(this, &PluginModel::layoutChanged, this, &PluginModel::countChanged);
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_plugins.count();
}

QHash<int, QByteArray> PluginModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(TextRole, "textRole");
    roles.insert(EnabledRole, "enabledRole");

    return roles;
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_plugins.count()) {
        return QVariant();
    }

    PluginInterface *plugin = m_plugins[index.row()];

    switch (role) {
    case TextRole:
        return plugin->text();
    case EnabledRole:
        return plugin->enabled();
    }

    return QVariant();
}

int PluginModel::count() const
{
    return m_plugins.count();
}

PluginInterface *PluginModel::at(int index) const
{
    Q_ASSERT(index >= 0 && index < count());
    return m_plugins.at(index);
}

void PluginModel::addPlugin(PluginInterface *plugin)
{
    beginInsertRows(QModelIndex(), m_plugins.count(), m_plugins.count());
    m_plugins.append(plugin);
    endInsertRows();
}

PluginInterface::List PluginModel::plugins() const
{
    return m_plugins;
}

void PluginModel::setPluginEnabled(bool enabled, int i)
{
    Q_ASSERT(i >= 0 && i < count());
    PluginInterface *plugin = m_plugins.at(i);
    plugin->setEnabled(enabled);
    const QString pluginName = dynamic_cast<QObject*>(plugin)->metaObject()->className();
    Kernel::instance()->settings()->beginGroup("plugins");
    Kernel::instance()->settings()->setValue(pluginName + ".enabled", enabled);
    Kernel::instance()->settings()->endGroup();
    plugin->setEnabled(enabled);

    emit dataChanged(index(0, 0), index(rowCount()-1, 0));
}
