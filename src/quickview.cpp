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
#include "controller.h"
#include "tooltipcontroller.h"
#include "storage.h"
#include "webdavsyncer.h"
#include "kernel.h"
#include "utils.h"

#include <QStandardPaths>
#include <QQmlContext>
#include <QString>
#include <QScreen>
#include <QDebug>
#include <QQmlEngine>
#include <QQuickItem>
#include <QGuiApplication>

QuickView::QuickView(QQmlEngine *engine)
    : QQuickView(engine, 0)
    , m_controller(Kernel::instance()->controller())
#ifdef DEVELOPER_MODE
    , m_useqresources(!m_controller->isMobile())
#else
    , m_useqresources(false)
#endif
{
    rootContext()->setContextProperty("_toolTipController", new ToolTipController(this));

    createStyleComponent();
    if (m_useqresources) {
        // So that F5 reloads QML without having to restart the application
        setSource(QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/Main.qml"));
    } else {
        setSource(QUrl("qrc:/qml/Main.qml"));
    }
    printTimeInfo("QuickView: Set Source END");

#ifdef Q_OS_WIN

# if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
        // Qt 5.1 does not support frameless and translucent windows on Windows
        setColor(Qt::transparent);
        setDefaultAlphaBuffer(true);
# endif

#else
    setColor(Qt::transparent);
    setDefaultAlphaBuffer(true); // Because some drivers don't request it. QTBUG-41074
#endif
    QSize screenSize = qApp->primaryScreen()->size();
    if (m_controller->isMobile()) {
        setResizeMode(QQuickView::SizeRootObjectToView);
    } else {
        setFlags(flags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        setResizeMode(QQuickView::SizeViewToRootObject);
        setPosition(screenSize.width()/2 - width()/2, 0);
    }

    connect(m_controller, &Controller::requestActivateWindow, this, &QuickView::requestActivate);
    printTimeInfo("QuickView: CTOR END");
}

void QuickView::reloadQML()
{
    qDebug() << "Reloading QML ...";
    engine()->clearComponentCache();
    printTimeInfo("QuickView: cleared component cache");
    createStyleComponent();
    setSource(source());
    printTimeInfo("QuickView: setted Source");
}

QUrl QuickView::styleFileName() const
{
    const QString &dataDirectory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    const QString fileName = dataDirectory + "/Style.qml";

    if (QFile::exists(fileName)) {
        // User can override the default style by creating a Style.qml file in /home/<user>/.local/share/KDAB/flow/
        return QUrl::fromLocalFile(fileName);
    } else {
        const QString filename = m_controller->isMobile() ? "MobileStyle.qml" : "DefaultStyle.qml";
        // Developer mode doesn't use qrc:, so we can reload with F5
        return m_useqresources ? QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/" + filename)
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
    printTimeInfo("QuickView: created style component");
}

void QuickView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_useqresources && event->key() == Qt::Key_F5) {
        event->accept();
        reloadQML();
    } else if (m_useqresources && event->key() == Qt::Key_F4) {
        Kernel::instance()->storage()->dumpDebugInfo();
        qDebug() << "Active focus belongs to" << activeFocusItem();
        qDebug() << "TagEditStatus:" << m_controller->tagEditStatus();
    } else {
        QQuickView::keyReleaseEvent(event);
    }
}

