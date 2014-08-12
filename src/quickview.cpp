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
#include "taskfilterproxymodel.h"
#include "pluginmodel.h"
#include "plugininterface.h"
#include "settings.h"
#include "controller.h"
#include "archivedtasksfiltermodel.h"
#include "tooltipcontroller.h"
#include "storage.h"
#include "imageprovider.h"
#include "circularprogressindicator.h"
#include "webdavsyncer.h"

#include <QStandardPaths>
#include <QQmlContext>
#include <QString>
#include <QScreen>
#include <QDebug>
#include <QQmlEngine>
#include <QDir>
#include <QPluginLoader>
#include <QQuickItem>
#include <QGuiApplication>

QuickView::QuickView(QWindow *parent)
    : QQuickView(parent)
    , m_storage(Storage::instance())
    , m_controller(new Controller(this))
    , m_pluginModel(new PluginModel(this))
    , m_developerMode(qApp->arguments().contains("-d"))
{
    m_storage->load();
    rootContext()->setContextProperty("_toolTipController", new ToolTipController(this));
    rootContext()->setContextProperty("_controller", m_controller);
    rootContext()->setContextProperty("_pluginModel", m_pluginModel);
    rootContext()->setContextProperty("_storage", Storage::instance());
    rootContext()->setContextProperty("_window", this);

    createStyleComponent();
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

    engine()->addImageProvider("icons", new ImageProvider());

    if (m_developerMode) {
        // So that F5 reloads QML without having to restart the application
        setSource(QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/Main.qml"));
    } else {
        setSource(QUrl("qrc:/qml/Main.qml"));
    }


#ifdef Q_OS_WIN

# if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
        // Qt 5.1 does not support frameless and translucent windows on Windows
        setColor(Qt::transparent);
# endif

#else
    setColor(Qt::transparent);
#endif
    QSize screenSize = qApp->primaryScreen()->size();
    if (controller()->isMobile()) {
        setResizeMode(QQuickView::SizeRootObjectToView);
    } else {
        setFlags(flags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        setResizeMode(QQuickView::SizeViewToRootObject);
        setPosition(screenSize.width()/2 - width()/2, 0);
        loadPlugins();
    }

    connect(m_controller, &Controller::currentTaskChanged, this, &QuickView::onTaskStatusChanged);
    connect(engine(), &QQmlEngine::quit, qApp, &QGuiApplication::quit);
}

Controller *QuickView::controller() const
{
    return m_controller;
}

void QuickView::onTaskStatusChanged()
{
    notifyPlugins(m_controller->currentTask()->status());
}

void QuickView::reloadQML()
{
    qDebug() << "Reloading QML ...";
    engine()->clearComponentCache();
    createStyleComponent();
    setSource(source());
}

void QuickView::notifyPlugins(TaskStatus newStatus)
{
    PluginInterface::List plugins = m_pluginModel->plugins();
    foreach (PluginInterface *plugin, plugins) {
        plugin->setTaskStatus(newStatus);
    }
}

QUrl QuickView::styleFileName() const
{
    const QString &dataDirectory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    const QString fileName = dataDirectory + "/Style.qml";

    if (QFile::exists(fileName)) {
        // User can override the default style by creating a Style.qml file in /home/<user>/.local/share/KDAB/flow/
        return QUrl::fromLocalFile(fileName);
    } else {
        const QString filename = controller()->isMobile() ? "MobileStyle.qml" : "DefaultStyle.qml";
        // Developer mode doesn't use qrc:, so we can reload with F5
        return m_developerMode ? QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/" + filename)
                               : QUrl("qrc:/qml/" + filename);
    }
}

void QuickView::createStyleComponent()
{
    QQmlComponent *styleComponent = new QQmlComponent(engine(),
                                                      styleFileName(),
                                                      QQmlComponent::PreferSynchronous,
                                                      this);
    QObject *styleObject = styleComponent->create();

    if (styleObject) {
        QQuickItem *item = qobject_cast<QQuickItem*>(styleObject);
        Q_ASSERT(item);
        rootContext()->setContextProperty("_style", styleObject);
        item->setParent(this);
    } else {
        qWarning() << styleComponent->errorString();
        Q_ASSERT(false);
    }
}

void QuickView::loadPlugins()
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
        QSettings *settings = Settings::instance();
        foreach (const QString &fileName, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *pluginObject = loader.instance();
            if (pluginObject) {
                PluginInterface *pluginInterface = qobject_cast<PluginInterface*>(pluginObject);
                if (pluginInterface) {
                    pluginInterface->setTaskStatus(TaskStopped);
                    const QString pluginName = pluginObject->metaObject()->className();
                    settings->beginGroup("plugins");
                    const bool enabled = settings->value(pluginName + ".enabled", /**defaul=*/true).toBool();
                    settings->endGroup();
                    pluginInterface->setEnabled(enabled);
                    m_pluginModel->addPlugin(pluginInterface);
                }
            }
        }
    }

    const int count = m_pluginModel->rowCount();
    qDebug() << "Loaded" << count << (count == 1 ? "plugin" : "plugins");
}

void QuickView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_developerMode && event->key() == Qt::Key_F5) {
        event->accept();
        reloadQML();
    } else if (m_developerMode && event->key() == Qt::Key_F4) {
        m_storage->dumpDebugInfo();
        qDebug() << "Active focus belongs to" << activeFocusItem();
        qDebug() << "TagEditStatus:" << m_controller->tagEditStatus();
    } else {
        QQuickView::keyReleaseEvent(event);
    }
}

