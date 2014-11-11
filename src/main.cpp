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
#include <QFontDatabase>

#ifdef FLOW_DBUS
# include <QDBusConnection>
# include <QDBusError>
#endif

#include <QStandardPaths>
#include <QTranslator>
#include <QScreen>
#include <QDir>

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
#else
    Q_UNUSED(controller);
#endif
}

static QString logFile()
{
    static QString filename;
    if (filename.isEmpty()) {
        filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/flow.log";
        QFile::remove(filename);
    }

    return filename;
}

void windowsMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file(logFile());
    file.open(QIODevice::Append | QIODevice::WriteOnly);
    QTextStream out(&file);

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

    out << level << msg << "(" << context.file << ":" << context.line << " " << context.function << ")\r\n";
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

int main(int argc, char *argv[])
{
    Utils::printTimeInfo("main");

#ifdef Q_OS_WIN
    qputenv("QT_QPA_PLATFORM","windows:fontengine=freetype");
    qInstallMessageHandler(windowsMessageHandler);
#endif
    Application app(argc, argv);
    Utils::printTimeInfo("main: created QApplication");
    app.setOrganizationName("KDAB");
    app.setApplicationName("flow");

    QFontDatabase::addApplicationFont(":/fonts/fontawesome-webfont.ttf");
    QFontDatabase::addApplicationFont(":/fonts/open-sans/OpenSans-Regular.ttf");
    app.setFont(QFont("Open Sans"));

    QTranslator translator;
    translator.load(QString(":/translations/flow_%1").arg(QLocale::system().name())); // export LANG="pt_PT" to change
    app.installTranslator(&translator);
    Utils::printTimeInfo("main: installed QTranslator");

    RuntimeConfiguration defaultConfig;
    defaultConfig.setDataFileName(defaultDataFileName());
    Kernel kernel(defaultConfig);

    Utils::printTimeInfo("main: created Kernel::instance()");
    QuickView window(&kernel);
    Utils::printTimeInfo("main: created QuickView");
    initDBus(kernel.controller());
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
