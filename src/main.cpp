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

#ifdef FLOW_DBUS
# include <QDBusConnection> // Keep before quickview.h because some mingw header #defines interface struct
# include <QDBusError>
#endif

#include "quickview.h"
#include "controller.h"
#include "kernel.h"
#include "dbus/flow.h"
#include "utils.h"
#include "runtimeconfiguration.h"

#ifdef QT_WIDGETS_LIB
# include <QApplication>
typedef QApplication Application;
#else
# include <QGuiApplication>
typedef QGuiApplication Application;
#endif

#include <QDebug>

#include <QStandardPaths>
#include <QTranslator>
#include <QScreen>
#include <QDir>

void initDBus(Kernel *kernel)
{
#ifdef FLOW_DBUS
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "Cannot connect to the D-Bus session bus.\n"
                      "To start it, run:\n"
                      "\teval `dbus-launch --auto-syntax`\n";
        return;
    }

    if (!QDBusConnection::sessionBus().registerService("com.kdab.flow-pomodoro.FlowInterface")) {
        qWarning() << QDBusConnection::sessionBus().lastError();
        return;
    }

    Flow *flowDBusInterface = new Flow(kernel, qApp);
    QDBusConnection::sessionBus().registerObject("/", flowDBusInterface, QDBusConnection::ExportScriptableSlots);
#else
    Q_UNUSED(kernel);
#endif
}

#ifdef Q_OS_WIN
static QString logFile()
{
    static QString filename;
    if (filename.isEmpty()) {
        filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/flow.log";
        QFile::remove(filename);
    }

    return filename;
}
#endif

static bool acceptsWarning(const QString &warning)
{
    // False positive binding loop or a bug in QtQuick.Controls, ignore it.
    if (warning.contains("Binding loop detected for property") && warning.contains("FlowCheckBox.qml"))
        return false;

    return true;
}

void flowMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    if (type == QtWarningMsg && !acceptsWarning(msg))
        return;

    QString level;
    switch (type) {
    case QtDebugMsg:
        level = "Debug: ";
        break;
    case QtWarningMsg:
        level = "Warning: ";
        break;
    case QtCriticalMsg:
        level = "Critical: ";
        break;
    case QtFatalMsg:
        level = "Fatal: ";
        abort();
    }

#if defined(Q_OS_WIN)
    QFile file(logFile());
    file.open(QIODevice::Append | QIODevice::WriteOnly);
    QTextStream out(&file);
#else
    QTextStream out(stderr);
#endif
    out << level << msg << "\r\n";
}

static QString defaultDataFileName()
{
#if defined(Q_OS_ANDROID) && defined(DEVELOPER_MODE)
    return "/storage/sdcard0/flow.dat";
#endif
    QString filename;
    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (!QFile::exists(directory)) {
        QDir dir(directory);
        if (!dir.mkpath(directory)) {
            qWarning() << "Failed to create directory" << directory;
            qFatal("Bailing out...");
            return QString();
        }
    }

    filename += directory + "/flow.dat";
    return filename;
}

/*
static void onFocusObjectChanged(QObject *obj)
{
    qDebug() << "Focus object changed to " << obj;
}
*/

int main(int argc, char *argv[])
{
    Utils::printTimeInfo("main");

    // qputenv("QML_DISABLE_DISTANCEFIELD", "1");
#ifdef Q_OS_WIN
    // qputenv("QT_QPA_PLATFORM","windows:fontengine=freetype");
#endif
#ifndef Q_OS_ANDROID
    qInstallMessageHandler(flowMessageHandler);
#endif
#ifdef FLOW_STATIC_BUILD
    Q_INIT_RESOURCE(shellscriptplugin);
    Q_INIT_RESOURCE(hostsplugin);
#endif
    Application app(argc, argv);
    Utils::printTimeInfo("main: created QApplication");
    app.setOrganizationName("KDAB");
    app.setApplicationName("flow");

    QTranslator translator;
    translator.load(QString(":/translations/flow_%1").arg(QLocale::system().name())); // export LANG="pt_PT" to change
    app.installTranslator(&translator);
    Utils::printTimeInfo("main: installed QTranslator");

    RuntimeConfiguration defaultConfig;
    defaultConfig.setDataFileName(defaultDataFileName());
    Kernel kernel(defaultConfig);

    // app.connect(&app, &Application::focusObjectChanged, &onFocusObjectChanged);

    Utils::printTimeInfo("main: created Kernel::instance()");
    QuickView window(&kernel);
    Utils::printTimeInfo("main: created QuickView");
    initDBus(&kernel);
    Utils::printTimeInfo("main: initialized dbus");

    if (Utils::isMobile()) {
        window.showMaximized(); // Don't use fullscreen on android

        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            qDebug() << "Logical DPI=" << screen->logicalDotsPerInch()
                     << "; Physical DPI=" << screen->physicalDotsPerInch()
                     << "; Resolution=" << screen->size();
        } else {
            qWarning() << "Null screen";
        }
    } else {
        window.show();
    }

    Utils::printTimeInfo("main: starting app.exec()");
    return app.exec();
}
