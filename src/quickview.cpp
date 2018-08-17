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

#include "quickview.h"
#include "taskfilterproxymodel.h"
#include "controller.h"
#include "tooltipcontroller.h"
#include "storage.h"
#include "kernel.h"
#include "utils.h"
#include "settings.h"
#include "gitupdater.h"

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
#include <QFileInfo>
#include <QDir>

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
    rootContext()->setContextProperty(QStringLiteral("_window"), this);
    rootContext()->setContextProperty(QStringLiteral("_gitUpdater"), kernel->gitUpdater());
    rootContext()->setContextProperty(QStringLiteral("_toolTipController"), new ToolTipController(this));
    createStyleComponent();

    setIcon(QIcon(":/img/icon.png"));

    connect(m_kernel->settings(), &Settings::geometryTypeChanged, this, &QuickView::onWindowGeometryTypeChanged);
    connect(m_kernel->settings(), &Settings::stickyWindowChanged, this, &QuickView::setupWindowFlags);

    QString main = Utils::isMobile() ? QStringLiteral("LoadingScreen.qml")
                                     : QStringLiteral("MainDesktop.qml");

    Utils::printTimeInfo(QStringLiteral("QuickView: Set Source START"));
    if (m_useqresources) {
        // So that F5 reloads QML without having to restart the application
        setSource(QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/" + main));
    } else {
        setSource(QUrl("qrc:/qml/" + main));
    }
    Utils::printTimeInfo(QStringLiteral("QuickView: Set Source END"));

    setColor(Qt::transparent);
    setDefaultAlphaBuffer(true); // Because some drivers don't request it. QTBUG-41074

    if (Utils::isMobile()) {
        setResizeMode(QQuickView::SizeRootObjectToView);
    } else {
        setupWindowFlags();
        setResizeMode(QQuickView::SizeViewToRootObject);
        setupGeometry();
    }

    connect(m_controller, &Controller::requestActivateWindow, this, &QuickView::requestActivate);
    Utils::printTimeInfo(QStringLiteral("QuickView: CTOR END"));

    connect(kernel, &Kernel::systrayLeftClicked, this, &QuickView::toggleVisible);
    m_controller->setWindow(this);

    if (!Utils::isMobile())
        connect(this, &QWindow::screenChanged, this, &QuickView::setupSize);

    connect(kernel, &Kernel::dumpDebugInfoRequested, this, &QuickView::dumpDebug);
}

QuickView::~QuickView()
{
    // qDebug() << "~QuickView";
    Settings *settings = m_kernel->settings();
    if (settings->initialPosition() == Settings::PositionLast) {
        settings->setValue(QStringLiteral("windowLastPositionX"), x());
        settings->setValue(QStringLiteral("windowLastPositionY"), y());
        settings->scheduleSync();
    }
}

void QuickView::reloadQML()
{
    qDebug() << "Reloading QML ...";
    engine()->clearComponentCache();
    Utils::printTimeInfo(QStringLiteral("QuickView: cleared component cache"));
    createStyleComponent();
    setSource(source());
    Utils::printTimeInfo(QStringLiteral("QuickView: setted Source"));
}

void QuickView::setupGeometry()
{
    if (Utils::isMobile())
        return;

    setupSize();

    int x = 0;
    int y = 0;
    QScreen *s = screen() ? screen() : qApp->primaryScreen();
    QSize screenSize = s->size();

    const int maxX = screenSize.width() - m_contractedWidth;
    const int maxY = screenSize.height() - m_contractedHeight;
    const int centerX = screenSize.width() / 2 - m_contractedWidth / 2;

    switch (m_kernel->settings()->initialPosition()) {
    case Settings::PositionNone:
        return;
    case Settings::PositionLast:
        x = qBound(0, m_kernel->settings()->value(QStringLiteral("windowLastPositionX")).toInt(), maxX);
        y = qBound(0, m_kernel->settings()->value(QStringLiteral("windowLastPositionY")).toInt(), maxY);
        break;
    case Settings::PositionTop:
        y = 0;
        x = centerX;
        break;
    case Settings::PositionTopLeft:
        x = 0;
        y = 0;
        break;
    case Settings::PositionTopRight:
        x = maxX;
        y = 0;
        break;
    case Settings::PositionBottom:
        x = centerX;
        y = maxY;
        break;
    case Settings::PositionBottomLeft:
        x = 0;
        y = maxY;
        break;
    case Settings::PositionBottomRight:
        x = maxX;
        y = maxY;
        break;
    case Settings::MaxPositions:
        Q_ASSERT(false); // Doesn't happen
        return;
    }

    if (this->x() != x || this->y() != y)
        setPosition(x, y);
    // width and height isn't set because we use SizeViewToRoot (so we can animate expand/collapse from QML)
}

void QuickView::setupSize()
{
    int width = 0;
    int height = 0;

    switch (m_kernel->settings()->geometryType()) {
    case Settings::GeometryStandard:
    case Settings::GeometryCustom:
        width = 400;
        height = 50;
        break;
    case Settings::GeometryThin:
        width = 400;
        height = 20;
        break;
    case Settings::GeometrySmallSquare:
        width = 40;
        height = 40;
        break;
    case Settings::MaxGeometryTypes:
        Q_ASSERT(false);
        return;
    }

    setContractedWidth(width * Utils::dpiFactor(screen()));
    setContractedHeight(height * Utils::dpiFactor(screen()));
}

QUrl QuickView::styleFileName() const
{
    QFileInfo info(m_kernel->runtimeConfiguration().dataFileName());
    QDir dataDir = info.absoluteDir();
    const QString customStyleFileName = dataDir.absoluteFilePath(QStringLiteral("Style.qml"));

    if (QFile::exists(customStyleFileName)) {
        // User can override the default style by creating a Style.qml file in /home/<user>/.local/share/KDAB/flow/, or in FLOW_DIR (env variable)
        return QUrl::fromLocalFile(customStyleFileName);
    } else {
        const QString filename = Utils::isMobile() ? QStringLiteral("MobileStyle.qml")
                                                   : QStringLiteral("DefaultStyle.qml");
        // Developer mode doesn't use qrc:, so we can reload with F5
        return m_useqresources ? QUrl::fromLocalFile(qApp->applicationDirPath() + "/src/qml/" + filename)
                               : QUrl("qrc:/qml/" + filename);
    }
}

void QuickView::createStyleComponent()
{
    Utils::printTimeInfo(QStringLiteral("QuickView: create style component START"));
    QQmlComponent *styleComponent = new QQmlComponent(engine(),
                                                      styleFileName(),
                                                      QQmlComponent::PreferSynchronous,
                                                      this);
    Utils::printTimeInfo(QStringLiteral("QuickView: create style component END"));
    QObject *styleObject = styleComponent->create();
    Utils::printTimeInfo(QStringLiteral("QuickView: created style item"));

    if (styleObject) {
        auto *item = qobject_cast<QQuickItem*>(styleObject);
        Q_ASSERT(item);
        rootContext()->setContextProperty(QStringLiteral("_style"), styleObject);
        item->setParent(this);
    } else {
        qWarning() << styleComponent->errorString();
        Q_ASSERT(false);
    }
    Utils::printTimeInfo(QStringLiteral("QuickView: createStyleComponent END"));
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
    if (m_kernel->settings()->stickyWindow()) {
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

void QuickView::onWindowGeometryTypeChanged()
{
    setupGeometry();
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

void QuickView::showWidgetContextMenu(QPoint pos)
{
#ifdef QT_WIDGETS_LIB

    if (!Utils::platformSupportsWidgets())
        return;

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

// Since QObject::findChildren() doesn't know about parentItem()/childItems()
static void findItems(QQuickItem *root, const QString &name, QList<QQuickItem*> &list)
{
    if (root->objectName() == name)
        list << root;

    foreach (QQuickItem *child, root->childItems()) {
        findItems(child, name, list);
    }
}

QQuickItem * QuickView::itemByName(const QString &name, QQuickItem *root)
{
    auto items = itemsByName(name, root);
    return items.isEmpty() ? nullptr : items.first();
}

QList<QQuickItem*> QuickView::itemsByName(const QString &name, QQuickItem *root)
{
    QList<QQuickItem*> items;
    findItems(root ? root : rootObject(), name, items);
    return items;
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
    moveMouseTo(item);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, scenePos, scenePos, globalPos, Qt::LeftButton, Qt::LeftButton, 0);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, scenePos, scenePos, globalPos, Qt::LeftButton, 0, 0);
    mousePressEvent(&pressEvent);
    mouseReleaseEvent(&releaseEvent);
}

void QuickView::mouseClick(const QString &objectName)
{
    mouseClick(itemByName(objectName));
}

void QuickView::sendKey(int key, const QString &text, Qt::KeyboardModifiers modifiers)
{
    // Go through qApp instead of calling key event handlers directly, so event filters are processed
    QKeyEvent pressEvent(QKeyEvent::KeyPress, key, modifiers, text);
    qApp->sendEvent(this, &pressEvent);
    QKeyEvent releaseEvent(QKeyEvent::KeyRelease, key, modifiers, text);
    qApp->sendEvent(this, &releaseEvent);
}

void QuickView::sendText(const QString &text)
{
    for (int i = 0; i < text.count(); ++i) {
        int key = Qt::Key_A + (text.at(i).unicode() - QChar('a').unicode());
        sendKey(key, text.at(i));
    }
}

void QuickView::moveMouseTo(QQuickItem *item)
{
    QPointF scenePos = item->parentItem()->mapToScene(item->position() + QPointF(item->width() / 2, item->height() / 2));
    QPointF globalPos = mapToGlobal(scenePos.toPoint());
    QCursor::setPos(globalPos.toPoint());

    // Also send a move event so that MouseArea's detect mouse entering
    QMouseEvent moveEvent(QEvent::MouseMove, scenePos, scenePos, globalPos, Qt::LeftButton, Qt::LeftButton, 0);
    mouseMoveEvent(&moveEvent);
}

#endif

void QuickView::dumpDebug()
{
    qDebug() << "QQuickView:";
    qDebug() << "    geo=" << geometry() << "; isVisible=" << isVisible();
}
