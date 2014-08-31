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

#include <QStandardPaths>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPluginLoader>
#include <QDir>

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

    qmlRegisterType<CircularProgressIndicator>("com.kdab.flowpomodoro", 1, 0, "CircularProgressIndicator");
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

Kernel *Kernel::instance()
{
    static Kernel *kernel = new Kernel(qApp);
    return kernel;
}

Kernel::~Kernel()
{
    if (m_storage->saveScheduled())
        m_storage->save();
}

Kernel::Kernel(QObject *parent)
    : QObject(parent)
    , m_storage(new JsonStorage(this))
    , m_qmlEngine(new QQmlEngine(this))
    , m_settings(new Settings(this))
    , m_controller(new Controller(m_qmlEngine->rootContext(), m_storage, m_settings, this))
    , m_pluginModel(new PluginModel(this))
{
    m_runtimeConfiguration.setDataFileName(defaultDataFileName());
    registerQmlTypes();
    qmlContext()->setContextProperty("_controller", m_controller);
    qmlContext()->setContextProperty("_storage", m_storage);
    qmlContext()->setContextProperty("_pluginModel", m_pluginModel);

    connect(m_controller, &Controller::currentTaskChanged, this, &Kernel::onTaskStatusChanged);
    connect(m_qmlEngine, &QQmlEngine::quit, qGuiApp, &QGuiApplication::quit);
    QMetaObject::invokeMethod(m_storage, "load", Qt::QueuedConnection); // Schedule a load. Don't do it directly, it will deadlock in instance()
    QMetaObject::invokeMethod(this, "maybeLoadPlugins", Qt::QueuedConnection);
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

void Kernel::setRuntimeConfiguration(const RuntimeConfiguration &config)
{
    m_runtimeConfiguration = config;
}

RuntimeConfiguration Kernel::runtimeConfiguration() const
{
    return m_runtimeConfiguration;
}

void Kernel::notifyPlugins(TaskStatus newStatus)
{
    PluginInterface::List plugins = m_pluginModel->plugins();
    foreach (PluginInterface *plugin, plugins) {
        plugin->setTaskStatus(newStatus);
    }
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

        qDebug() << "Looking for plugins in " << candidatePath;
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
