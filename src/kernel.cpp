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

#include "kernel.h"
#include "controller.h"
#include "jsonstorage.h"
#include "pluginmodel.h"
#include "plugininterface.h"
#include "settings.h"
#include "circularprogressindicator.h"
#include "taskfilterproxymodel.h"
#include "archivedtasksfiltermodel.h"
#include "task.h"
#include "tag.h"
#include "settings.h"
#include "utils.h"
#include "checkbox.h"
#include "loadmanager.h"
#include "webdavsyncer.h"
#include "quickview.h"
#include "taskcontextmenumodel.h"

#include <QStandardPaths>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPluginLoader>
#include <QDir>
#include <QWindow>

#ifdef QT_WIDGETS_LIB
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#endif

#ifdef FLOW_STATIC_BUILD
Q_IMPORT_PLUGIN(ShellScriptPlugin)
#endif

static void registerQmlTypes()
{
    qmlRegisterUncreatableType<QAbstractItemModel>("Controller",
                                                   1, 0, "QAbstractItemModel",
                                                   "QAbstractItemModel is not creatable");

    qmlRegisterUncreatableType<QAbstractListModel>("Controller",
                                                   1, 0, "QAbstractListModel",
                                                   "QAbstractListModel is not creatable");

    qmlRegisterUncreatableType<TaskFilterProxyModel>("Controller",
                                                     1, 0, "TaskFilterProxyModel",
                                                     "TaskFilterProxyModel is not creatable");

    qmlRegisterUncreatableType<ArchivedTasksFilterModel>("Controller",
                                                         1, 0, "ArchivedTasksFilterModel",
                                                         "ArchivedTasksFilterModel is not creatable");

    qmlRegisterUncreatableType<Controller>("Controller",
                                           1, 0, "Controller",
                                           "Controller is not creatable");

#ifndef UNIT_TEST_RUN
    qmlRegisterUncreatableType<QuickView>("Controller",
                                          1, 0, "QuickView",
                                          "QuickView is not creatable");
#endif
    qmlRegisterUncreatableType<LoadManager>("Controller",
                                            1, 0, "LoadManager",
                                            "LoadManager is not creatable");

    qmlRegisterUncreatableType<Task>("Controller",
                                     1, 0, "Task_",
                                     "Task is not creatable");

    qmlRegisterUncreatableType<Tag>("Controller",
                                    1, 0, "Tag_",
                                    "Tag is not creatable");

    qmlRegisterUncreatableType<TaskContextMenuModel>("Controller",
                                                     1, 0, "TaskContextMenuModel",
                                                     "TaskContextMenuModel is not creatable");

    qmlRegisterType<CircularProgressIndicator>("com.kdab.flowpomodoro", 1, 0, "CircularProgressIndicator");
    qmlRegisterType<CheckBoxImpl>("com.kdab.flowpomodoro", 1, 0, "CheckBoxPrivate");
}

Kernel::~Kernel()
{
    destroySystray();
    delete m_settings;
}

Kernel::Kernel(const RuntimeConfiguration &config, QObject *parent)
    : QObject(parent)
    , m_runtimeConfiguration(config)
    , m_storage(new JsonStorage(this, this))
    , m_qmlEngine(new QQmlEngine(0)) // leak the engine, no point in wasting shutdown time. Also we get a qmldebug server crash if it's parented to qApp, which Kernel is
    , m_settings(config.settings() ? config.settings() : new Settings(this))
    , m_controller(new Controller(m_qmlEngine->rootContext(), this, m_storage, m_settings, this))
    , m_pluginModel(new PluginModel(this))
#ifndef NO_WEBDAV
    , m_webDavSyncer(new WebDAVSyncer(this))
#endif
#if defined(QT_WIDGETS_LIB) && !defined(QT_NO_SYSTRAY)
    , m_systrayIcon(0)
    , m_trayMenu(0)
#endif
{
    m_qmlEngine->rootContext()->setObjectName("QQmlContext"); // GammaRay convenience
    registerQmlTypes();
    qmlContext()->setContextProperty("_controller", m_controller);
    qmlContext()->setContextProperty("_storage", m_storage);
    qmlContext()->setContextProperty("_pluginModel", m_pluginModel);
    qmlContext()->setContextProperty("_loadManager", m_controller->loadManager());
#ifndef NO_WEBDAV
    qmlContext()->setContextProperty("_webdavSync", m_webDavSyncer);
#endif

    connect(m_controller, &Controller::currentTaskChanged, this, &Kernel::onTaskStatusChanged);
    connect(m_qmlEngine, &QQmlEngine::quit, qGuiApp, &QGuiApplication::quit);
    QMetaObject::invokeMethod(m_storage, "load", Qt::QueuedConnection); // Schedule a load. Don't do it directly, it will deadlock in instance()
    QMetaObject::invokeMethod(this, "maybeLoadPlugins", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_controller, "updateWebDavCredentials", Qt::QueuedConnection);

    if (m_controller->useSystray())
        setupSystray();
}

Storage *Kernel::storage() const
{
    return m_storage;
}

Controller *Kernel::controller() const
{
    return m_controller;
}

QQmlContext *Kernel::qmlContext() const
{
    return m_qmlEngine->rootContext();
}

QQmlEngine *Kernel::qmlEngine() const
{
    return m_qmlEngine;
}

Settings *Kernel::settings() const
{
    return m_settings;
}

RuntimeConfiguration Kernel::runtimeConfiguration() const
{
    return m_runtimeConfiguration;
}

#ifndef NO_WEBDAV
WebDAVSyncer *Kernel::webdavSyncer() const
{
    return m_webDavSyncer;
}
#endif

void Kernel::notifyPlugins(TaskStatus newStatus)
{
    PluginInterface::List plugins = m_pluginModel->plugins();
    foreach (PluginInterface *plugin, plugins) {
        plugin->setTaskStatus(newStatus);
    }
}

void Kernel::setupSystray()
{
#if defined(QT_WIDGETS_LIB) && !defined(QT_NO_SYSTRAY)
    m_systrayIcon = new QSystemTrayIcon(QIcon(":/img/icon.png"), this);

# if !defined(Q_OS_MAC)
    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QGuiApplication::quit);
    m_trayMenu = new QMenu();
    m_trayMenu->addAction(quitAction);
    m_systrayIcon->setContextMenu(m_trayMenu);
# endif
    m_systrayIcon->show();
    connect(m_systrayIcon, &QSystemTrayIcon::activated, this, &Kernel::onSystrayActivated);
#endif
}

void Kernel::destroySystray()
{
#if defined(QT_WIDGETS_LIB) && !defined(QT_NO_SYSTRAY)
    delete m_systrayIcon;
    delete m_trayMenu;
    m_systrayIcon = 0;
    m_trayMenu = 0;
#endif
}

void Kernel::loadPlugins()
{
    if (Utils::isMobile())
        return;

    QObjectList plugins;

#ifdef FLOW_STATIC_BUILD
    foreach (QObject *pluginObject, QPluginLoader::staticInstances()) {
        plugins << pluginObject;
    }
#else
    QStringList paths = QCoreApplication::libraryPaths();

    foreach (const QString &path, paths) {
        QString candidatePath = path;
        if (path == qApp->applicationDirPath()) {
            candidatePath += QStringLiteral("/plugins/");
        } else {
            candidatePath += QStringLiteral("/flow/");
        }

        // qDebug() << "Looking for plugins in " << candidatePath;
        QDir pluginsDir = QDir(candidatePath);
        foreach (const QString &fileName, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *pluginObject = loader.instance();
            if (pluginObject)
                plugins << pluginObject;
        }
    }
#endif

    foreach (QObject *pluginObject, plugins) {
        PluginInterface *pluginInterface = qobject_cast<PluginInterface*>(pluginObject);
        if (!pluginInterface)
            continue;
        pluginInterface->setTaskStatus(TaskStopped);
        const QString pluginName = pluginObject->metaObject()->className();
        m_settings->beginGroup("plugins");
        const bool enabled = m_settings->value(pluginName + ".enabled", /**defaul=*/true).toBool();
        m_settings->endGroup();
        pluginInterface->setEnabled(enabled);
        pluginInterface->setSettings(m_settings);
        pluginInterface->setQmlEngine(m_qmlEngine);
        m_pluginModel->addPlugin(pluginInterface);
    }

    const int count = m_pluginModel->rowCount();
    qDebug() << "Loaded" << count << (count == 1 ? "plugin" : "plugins");
}

void Kernel::onTaskStatusChanged()
{
    notifyPlugins(m_controller->currentTask()->status());
}

void Kernel::maybeLoadPlugins()
{
    if (!Utils::isMobile() && m_runtimeConfiguration.pluginsSupported())
        loadPlugins();
}

#if defined(QT_WIDGETS_LIB) && !defined(QT_NO_SYSTRAY)
void Kernel::onSystrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        QWindow *window = QGuiApplication::topLevelWindows().value(0);
        if (!window) {
            qWarning() << "Kernel::onSystrayActivated() window not found";
            return;
        }

        emit systrayLeftClicked();
    }
}
#endif
