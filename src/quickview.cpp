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
#include "settings.h"

#include <QStandardPaths>
#include <QQmlContext>
#include <QString>
#include <QScreen>
#include <QDebug>
#include <QQmlEngine>
#include <QQuickItem>
#include <QGuiApplication>
#ifdef QT_WIDGETS_LIB
# include <QMenu>
#endif

QuickView::QuickView(Kernel *kernel)
    : QQuickView(kernel->qmlEngine(), 0)
    , m_controller(kernel->controller())
#ifdef DEVELOPER_MODE
    , m_useqresources(!Utils::isMobile())
#else
    , m_useqresources(false)
#endif
    , m_kernel(kernel)
    , m_contractedWidth(0)
    , m_contractedHeight(0)
{
    rootContext()->setContextProperty("_window", this);
    rootContext()->setContextProperty("_toolTipController", new ToolTipController(this));
    createStyleComponent();
    readInitialPosition();
    readGeometryType();

    setIcon(QIcon(":/img/icon.png"));

    connect(m_controller, &Controller::stickyWindowChanged, this, &QuickView::setupWindowFlags);

    QString main = Utils::isMobile() ? "LoadingScreen.qml" : "MainDesktop.qml";

    Utils::printTimeInfo("QuickView: Set Source START");
    if (m_useqresources) {
        // So that F5 reloads QML without having to restart the application
        setSource(QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/" + main));
    } else {
        setSource(QUrl("qrc:/qml/" + main));
    }
    Utils::printTimeInfo("QuickView: Set Source END");

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

    if (Utils::isMobile()) {
        setResizeMode(QQuickView::SizeRootObjectToView);
    } else {
        setupWindowFlags();
        setResizeMode(QQuickView::SizeViewToRootObject);
        setupGeometry();
    }

    connect(m_controller, &Controller::requestActivateWindow, this, &QuickView::requestActivate);
    Utils::printTimeInfo("QuickView: CTOR END");

    connect(kernel, &Kernel::systrayLeftClicked, this, &QuickView::toggleVisible);
}

QuickView::~QuickView()
{
    // qDebug() << "~QuickView";
    if (m_initialPosition == PositionLast) {
        m_kernel->settings()->setValue("windowLastPositionX", x());
        m_kernel->settings()->setValue("windowLastPositionY", y());
        m_kernel->settings()->scheduleSync();
    }
}

QuickView::Position QuickView::initialPosition() const
{
    return m_initialPosition;
}

void QuickView::setInitialPosition(QuickView::Position position)
{
    if (m_initialPosition != position) {
        m_initialPosition = position;
        m_kernel->settings()->setValue("windowInitialPosition", position);
        m_kernel->settings()->scheduleSync();
    }
}

void QuickView::reloadQML()
{
    qDebug() << "Reloading QML ...";
    engine()->clearComponentCache();
    Utils::printTimeInfo("QuickView: cleared component cache");
    createStyleComponent();
    setSource(source());
    Utils::printTimeInfo("QuickView: setted Source");
}

void QuickView::setupGeometry()
{
    if (Utils::isMobile())
        return;

    QSize screenSize = qApp->primaryScreen()->size();

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    switch (m_geometryType) {
    case GeometryStandard:
    case GeometryCustom:
        width = 400;
        height = 50;
        break;
    case GeometryThin:
        width = 400;
        height = 20;
        break;
    case GeometrySmallSquare:
        width = 40;
        height = 40;
        break;
    case MaxGeometryTypes:
        Q_ASSERT(false);
        return;
    }

    setContractedWidth(width * Utils::dpiFactor());
    setContractedHeight(height * Utils::dpiFactor());

    const int maxX = screenSize.width() - m_contractedWidth;
    const int maxY = screenSize.height() - m_contractedHeight;
    const int centerX = screenSize.width() / 2 - m_contractedWidth / 2;

    switch (m_initialPosition) {
    case PositionNone:
        return;
    case PositionLast:
        x = qBound(0, m_kernel->settings()->value("windowLastPositionX").toInt(), maxX);
        y = qBound(0, m_kernel->settings()->value("windowLastPositionY").toInt(), maxY);
        break;
    case PositionTop:
        y = 0;
        x = centerX;
        break;
    case PositionTopLeft:
        x = 0;
        y = 0;
        break;
    case PositionTopRight:
        x = maxX;
        y = 0;
        break;
    case PositionBottom:
        x = centerX;
        y = maxY;
        break;
    case PositionBottomLeft:
        x = 0;
        y = maxY;
        break;
    case PositionBottomRight:
        x = maxX;
        y = maxY;
        break;
    case MaxPositions:
        Q_ASSERT(false); // Doesn't happen
        return;
    }

    if (this->x() != x || this->y() != y)
        setPosition(x, y);
    // width and height isn't set because we use SizeViewToRoot (so we can animate expand/collapse from QML)
}

void QuickView::readInitialPosition()
{
    const Position defaultPosition = PositionTop;
    m_initialPosition = static_cast<Position>(m_kernel->settings()->value("windowInitialPosition", defaultPosition).toInt());
    if (m_initialPosition < PositionNone || m_initialPosition >= MaxPositions) {
        setInitialPosition(defaultPosition);
    }
}

void QuickView::readGeometryType()
{
    const GeometryType defaultGeometry = GeometryStandard;
    m_geometryType = static_cast<GeometryType>(m_kernel->settings()->value("windowGeometryType", defaultGeometry).toInt());
    if (m_geometryType < GeometryStandard || m_geometryType >= MaxGeometryTypes) {
        setGeometryType(defaultGeometry);
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
        const QString filename = Utils::isMobile() ? "MobileStyle.qml" : "DefaultStyle.qml";
        // Developer mode doesn't use qrc:, so we can reload with F5
        return m_useqresources ? QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/" + filename)
                               : QUrl("qrc:/qml/" + filename);
    }
}

void QuickView::createStyleComponent()
{
    Utils::printTimeInfo("QuickView: create style component START");
    QQmlComponent *styleComponent = new QQmlComponent(engine(),
                                                      styleFileName(),
                                                      QQmlComponent::PreferSynchronous,
                                                      this);
    Utils::printTimeInfo("QuickView: create style component END");
    QObject *styleObject = styleComponent->create();
    Utils::printTimeInfo("QuickView: created style item");

    if (styleObject) {
        QQuickItem *item = qobject_cast<QQuickItem*>(styleObject);
        Q_ASSERT(item);
        rootContext()->setContextProperty("_style", styleObject);
        item->setParent(this);
    } else {
        qWarning() << styleComponent->errorString();
        Q_ASSERT(false);
    }
    Utils::printTimeInfo("QuickView: createStyleComponent END");
}

void QuickView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_useqresources && event->key() == Qt::Key_F5) {
        event->accept();
        reloadQML();
    } else if (m_useqresources && event->key() == Qt::Key_F4) {
        m_kernel->storage()->dumpDebugInfo();
        qDebug() << "Active focus belongs to" << activeFocusItem();
        qDebug() << "TagEditStatus:" << m_controller->tagEditStatus();
    } else {
        QQuickView::keyReleaseEvent(event);
    }
}

void QuickView::toggleVisible()
{
    setVisible(!isVisible());
}

void QuickView::setupWindowFlags()
{
    QFlags<Qt::WindowType> f = Qt::FramelessWindowHint;
    if (m_controller->stickyWindow()) {
        f |= Qt::WindowStaysOnTopHint;
#ifdef Q_OS_OSX
	f |= Qt::Window;
#else
      f |= Qt::Tool;
#endif
    } else {
	f |= Qt::Window;
    }

    setFlags(f);
}

void QuickView::setGeometryType(GeometryType geometryType)
{
    if (geometryType != m_geometryType) {
        m_geometryType = geometryType;
        m_kernel->settings()->setValue("windowGeometryType", geometryType);
        m_kernel->settings()->scheduleSync();
        emit geometryTypeChanged();
        setupGeometry();
    }
}

QuickView::GeometryType QuickView::geometryType() const
{
    return m_geometryType;
}

void QuickView::setContractedWidth(int contractedWidth)
{
    if (contractedWidth != m_contractedWidth) {
        m_contractedWidth = contractedWidth;
        emit contractedWidthChanged();
    }
}

int QuickView::contractedWidth() const
{
    return m_contractedWidth;
}

void QuickView::setContractedHeight(int contractedHeight)
{
    if (contractedHeight != m_contractedHeight) {
        m_contractedHeight = contractedHeight;
        emit contractedHeightChanged();
    }
}

int QuickView::contractedHeight() const
{
    return m_contractedHeight;
}

void QuickView::showWidgetContextMenu(const QPoint &pos)
{
#ifdef QT_WIDGETS_LIB

    // Widget context menu because the QML one has OpenGL artifacts on many cards I've tested

    static QMenu *contextMenu = 0;
    if (contextMenu) {
        // Protect against re-entrancy
        contextMenu->deleteLater();
        contextMenu = 0;
        return;
    }

    contextMenu = new QMenu();
    QAction *pauseAction = m_controller->currentTask()->running() ? new QAction(tr("Pause"), contextMenu)
                                                                  : new QAction(tr("Resume"), contextMenu);
    QAction *stopAction = new QAction(tr("Stop"), contextMenu);
    QAction *aboutAction = new QAction(tr("About"), contextMenu);
    QAction *quitAction = new QAction(tr("Quit"), contextMenu);

    if (!m_controller->rightClickedTask() && !m_controller->currentTask()->stopped()) {
        contextMenu->addAction(pauseAction);
        contextMenu->addAction(stopAction);
    }

    if (m_controller->currentPage() != Controller::AboutPage)
        contextMenu->addAction(aboutAction);

    contextMenu->addAction(quitAction);

    QAction *action = contextMenu->exec(mapToGlobal(pos));
    contextMenu->deleteLater();
    contextMenu = 0;

    if (action == aboutAction) {
        m_controller->setExpanded(true);
        m_controller->setCurrentPage(Controller::AboutPage);
    } else if (action == quitAction) {
        qApp->quit();
    } else if (action == pauseAction) {
        m_controller->pausePomodoro();
    } else if (action == stopAction) {
        m_controller->stopPomodoro();
    }

#else
    Q_UNUSED(pos);
#endif
}


#ifdef UNIT_TEST_RUN
QQuickItem * QuickView::itemByName(const QString &name)
{
    return rootObject() ? rootObject()->findChild<QQuickItem*>(name)
                        : nullptr;
}

QList<QQuickItem*> QuickView::itemsByName(const QString &name)
{
    return rootObject() ? rootObject()->findChildren<QQuickItem*>(name)
                        : QList<QQuickItem*>();
}

void QuickView::mouseClick(QQuickItem *item)
{
    if (!item) {
        qWarning() << "Invalid item";
        Q_ASSERT(false);
        return;
    }
    QPointF scenePos = item->parentItem()->mapToScene(item->position() + QPointF(item->width() / 2, item->height() / 2));
    QPointF globalPos = mapToGlobal(scenePos.toPoint());
    QCursor::setPos(globalPos.toPoint());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, scenePos, scenePos, globalPos, Qt::LeftButton, Qt::LeftButton, 0);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, scenePos, scenePos, globalPos, Qt::LeftButton, 0, 0);
    mousePressEvent(&pressEvent);
    mouseReleaseEvent(&releaseEvent);
}

void QuickView::mouseClick(const QString &objectName)
{
    mouseClick(itemByName(objectName));
}

#endif
