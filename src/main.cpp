/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  Copyright (C) 2016 Sérgio Martins <iamsergio@gmail.com>

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

#include "storage.h"
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
#include <QSettings>

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

static QString logFile()
{
    static QString filename;
    if (filename.isEmpty()) {
        filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/flow.log";
        QFile::remove(filename);
    }

    return filename;
}

static bool acceptsWarning(const QString &warning)
{
    // False positive binding loop or a bug in QtQuick.Controls, ignore it.
    if (warning.contains(QStringLiteral("Binding loop detected for property")) && warning.contains(QStringLiteral("FlowCheckBox.qml")))
        return false;

    return true;
}

static bool logsDebugToFile()
{
    // Mobile and Windows send qDebug to a file

#if defined(DEVELOPER_MODE)
    // Here you're using qtcreator
    return false;
#endif

#if defined(Q_OS_WIN)
    return true;
#endif

    return Utils::platformRequiresMobileUI();
}

void flowMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    if (type == QtWarningMsg && !acceptsWarning(msg))
        return;

    QString level;
    switch (type) {
    case QtDebugMsg:
        level = QStringLiteral("Debug: ");
        break;
    case QtWarningMsg:
        level = QStringLiteral("Warning: ");
        break;
    case QtCriticalMsg:
        level = QStringLiteral("Critical: ");
        break;
    case QtInfoMsg:
        level = QStringLiteral("Info: ");
        break;
    case QtFatalMsg:
        level = QStringLiteral("Fatal: ");
    }

    if (logsDebugToFile()) {
        QFile file(logFile());
        file.open(QIODevice::Append | QIODevice::WriteOnly);
        QTextStream out(&file);
        out << level << msg << "\r\n";
    }

    {
        QTextStream out(stderr);
        out << level << msg << "\r\n";
    }

    if (type == QtFatalMsg)
        abort();
}

static QString defaultFlowDir()
{
    // Env variable takes priority
    const QByteArray env_path = qgetenv("FLOW_DIR");
    if (!env_path.isEmpty())
        return QString::fromLatin1(env_path);

#if defined(Q_OS_ANDROID)
    // The usual settings are in flow's directory, which the user can't easily edit.
    // Allow the user to create an override settings file in the sdcard so he can edit when connecting to a computer.
    // Only changing "defaultFlowDir" is supported for now.

    const QString filename = QStringLiteral("/sdcard/flow/settings.ini");
    if (QFileInfo::exists(filename)) {
        QSettings settingsInSDCard(filename, QSettings::IniFormat);
        QString flowDir = settingsInSDCard.value(QStringLiteral("flowDefaultDir")).toString();
        if (!flowDir.isEmpty())
            return flowDir;
    }
#endif

    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
}

static QString defaultDataFileName()
{
    const QString directory = defaultFlowDir();

    if (!QFile::exists(directory)) {
        QDir dir(directory);
        if (!dir.mkpath(directory)) {
            qWarning() << "Failed to create directory" << directory;
            qFatal("Bailing out...");
            return QString();
        }
    }

    return QDir::toNativeSeparators(QDir(directory).absoluteFilePath(QStringLiteral("flow.dat")));
}

/*
static void onFocusObjectChanged(QObject *obj)
{
    qDebug() << "Focus object changed to " << obj;
}
*/

int main(int argc, char *argv[])
{
    Utils::printTimeInfo(QStringLiteral("main"));

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
    QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling); // because Qt only supports integer factors.
    Application app(argc, argv);
    Utils::printTimeInfo(QStringLiteral("main: created QApplication"));
    app.setOrganizationName(QStringLiteral("KDAB"));
    app.setApplicationName(QStringLiteral("flow"));

    QTranslator translator;
    translator.load(QStringLiteral(":/translations/flow_%1").arg(QLocale::system().name())); // export LANG="pt_PT" to change
    app.installTranslator(&translator);
    Utils::printTimeInfo(QStringLiteral("main: installed QTranslator"));

    RuntimeConfiguration runtimeConfig;
    runtimeConfig.setDataFileName(defaultDataFileName());
    runtimeConfig.setMobileUI(Utils::platformRequiresMobileUI() || qApp->arguments().contains("--mobile"));
    runtimeConfig.setSaveEnabled(!qApp->arguments().contains("--read-only"));

    Kernel kernel(runtimeConfig);
    kernel.storage()->load();

    // app.connect(&app, &Application::focusObjectChanged, &onFocusObjectChanged);

    Utils::printTimeInfo(QStringLiteral("main: created Kernel::instance()"));
    QuickView window(&kernel);
    Utils::printTimeInfo(QStringLiteral("main: created QuickView"));
    initDBus(&kernel);
    Utils::printTimeInfo(QStringLiteral("main: initialized dbus"));

    if (runtimeConfig.isMobileUI()) {
        window.showMaximized(); // Don't use fullscreen on android
    } else {
        window.show();
    }

    if (logsDebugToFile()) { // Logging to file, so lets be a bit more verbose
        for (auto screen : QGuiApplication::screens()) {
            if (screen) {
                qDebug() << "Logical DPI=" << screen->logicalDotsPerInch()
                         << "; Physical DPI=" << screen->physicalDotsPerInch()
                         << "; Physical DPI X=" << screen->physicalDotsPerInchX()
                         << "; Resolution=" << screen->size()
                         << "; Name=" << screen->name();
            } else {
                qWarning() << "Null screen";
            }
        }
    }

    Utils::printTimeInfo(QStringLiteral("main: starting app.exec()"));
    return app.exec();
}
