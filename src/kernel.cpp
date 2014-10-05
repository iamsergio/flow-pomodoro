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
#include "checkbox.h"
#include "webdavsyncer.h"
#include "taskcontextmenumodel.h"

#include <QStandardPaths>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPluginLoader>
#include <QDir>
#include <QWindow>

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
{
    registerQmlTypes();
    qmlContext()->setContextProperty("_controller", m_controller);
    qmlContext()->setContextProperty("_storage", m_storage);
    qmlContext()->setContextProperty("_pluginModel", m_pluginModel);
#ifndef NO_WEBDAV
    qmlContext()->setContextProperty("_webdavSync", m_webDavSyncer);
#endif

    connect(m_controller, &Controller::currentTaskChanged, this, &Kernel::onTaskStatusChanged);
    connect(m_qmlEngine, &QQmlEngine::quit, qGuiApp, &QGuiApplication::quit);
    QMetaObject::invokeMethod(m_storage, "load", Qt::QueuedConnection); // Schedule a load. Don't do it directly, it will deadlock in instance()
    QMetaObject::invokeMethod(this, "maybeLoadPlugins", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_controller, "updateWebDavCredentials", Qt::QueuedConnection);

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
#ifdef QT_WIDGETS_LIB
    m_systrayIcon = new QSystemTrayIcon(QIcon(":/img/icon.png"), this);
    m_systrayIcon->show();
    connect(m_systrayIcon, &QSystemTrayIcon::activated, this, &Kernel::onSystrayActivated);
#endif
}

void Kernel::loadPlugins()
{
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
            if (pluginObject) {
                PluginInterface *pluginInterface = qobject_cast<PluginInterface*>(pluginObject);
                if (pluginInterface) {
                    pluginInterface->setTaskStatus(TaskStopped);
                    const QString pluginName = pluginObject->metaObject()->className();
                    m_settings->beginGroup("plugins");
                    const bool enabled = m_settings->value(pluginName + ".enabled", /**defaul=*/true).toBool();
                    m_settings->endGroup();
                    pluginInterface->setEnabled(enabled);
                    m_pluginModel->addPlugin(pluginInterface);
                }
            }
        }
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
    if (!m_controller->isMobile() && m_runtimeConfiguration.pluginsSupported())
        loadPlugins();
}

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
