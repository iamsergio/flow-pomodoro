/*
  This file is part of Flow.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef PIDGIN_PLUGIN_H
#define PIDGIN_PLUGIN_H

#include "plugininterface.h"
#include "task.h"

#include <QtPlugin>
#include <QObject>

class PidginPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.kdab.flow.PluginInterface")
    Q_INTERFACES(PluginInterface)

public:
    PidginPlugin();

    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    bool enabled() const Q_DECL_OVERRIDE;

    void setTaskStatus(TaskStatus status) Q_DECL_OVERRIDE;
    QString text() const Q_DECL_OVERRIDE;

private:
    void update(bool enable);
    bool m_enabled;
};

#endif
