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

#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include "task.h"

#include <QString>
#include <QList>
#include <QtPlugin>

class Controller;
class QObject;
class QQmlEngine;
class QQuickItem;
class QSettings;

class PluginInterface
{
public:
	typedef QList<PluginInterface*> List;

    virtual ~PluginInterface() {}

    virtual bool enabled() const = 0;
	virtual void setEnabled(bool enabled) = 0;
    virtual void setTaskStatus(TaskStatus status) = 0;
    virtual QString text() const = 0;
    virtual QString helpText() const = 0;
    virtual QObject *controller() = 0;
    virtual void setQmlEngine(QQmlEngine *) = 0;
    virtual QQuickItem* configureItem() const = 0;
    virtual void setSettings(QSettings *) = 0;
    virtual bool enabledByDefault() const = 0;
};

Q_DECLARE_INTERFACE(PluginInterface, "com.kdab.flow.PluginInterface/v0.9.3")

#endif
