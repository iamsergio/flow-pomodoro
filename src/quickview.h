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

#ifndef _QUICK_WINDOW_H_
#define _QUICK_WINDOW_H_

#include "task.h"

#include <QQuickView>
#include <QUrl>

class Controller;
class QKeyEvent;

class PluginModel;
class Storage;

class QuickView : public QQuickView {
    Q_OBJECT
public:
    explicit QuickView(QWindow *parent = 0);
    Controller *controller() const;

protected:
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onTaskStatusChanged();

private:
    void loadPlugins();
    void reloadQML();
    void notifyPlugins(TaskStatus newStatus);
    QUrl styleFileName() const;
    void createStyleComponent();

    Storage *m_storage;
    Controller *m_controller;
    PluginModel *m_pluginModel;
    bool m_developerMode;
};

#endif
