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

#include "quickview.h"
#include "dbus/flow.h"

#include <QGuiApplication>

#include <QDebug>

#ifdef FLOW_DBUS
# include <QDBusConnection>
# include <QDBusError>
#endif


void initDBus(Controller *controller)
{
#ifdef FLOW_DBUS
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "Cannot connect to the D-Bus session bus.\n"
                      "To start it, run:\n"
                      "\teval `dbus-launch --auto-syntax`\n";
        exit(-1);
    }

    if (!QDBusConnection::sessionBus().registerService("com.kdab.flow-pomodoro.FlowInterface")) {
        qWarning() << QDBusConnection::sessionBus().lastError();
        exit(-2);
    }

    Flow *flowDBusInterface = new Flow(controller, qApp);
    QDBusConnection::sessionBus().registerObject("/", flowDBusInterface, QDBusConnection::ExportScriptableSlots);
#endif
}


int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    qputenv("QT_QPA_PLATFORM","windows:fontengine=freetype");
#endif
    QGuiApplication app(argc, argv);
    app.setOrganizationName("KDAB");
    app.setApplicationName("flow");

    QuickView window;
    initDBus(window.controller());
    window.show();

    return app.exec();
}
