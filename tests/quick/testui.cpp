/*
  This file is part of Flow.

  Copyright (C) 2015 Sérgio Martins <iamsergio@gmail.com>

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

#include "testui.h"
#include "quickview.h"
#include "storage.h"
#include "settings.h"
#include "kernel.h"
#include "controller.h"
#include <QQuickItem>
#include <QtTest/QtTest>

TestUI::TestUI() : TestBase()
{
}

void TestUI::initTestCase()
{
    createInstanceWithUI();
    m_view->show();
    expandFlow();
    m_stagedView = m_view->itemByName("stagedView");
    m_archiveView = m_view->itemByName("archiveView");
    QVERIFY(m_stagedView);
    QVERIFY(m_archiveView);
}

void TestUI::expandFlow()
{
    QQuickItem *globalMouseArea = m_view->itemByName("header");
    QVERIFY(globalMouseArea);
    QVERIFY(globalMouseArea->isVisible());
    QVERIFY(!m_controller->expanded());
    mouseClick(globalMouseArea);
    QVERIFY(m_controller->expanded());
}

void TestUI::expectedTodayTasks(int num)
{
    QCOMPARE(m_stagedView->property("count").toInt(), num);
}

void TestUI::expectedArchivedTasks(int num)
{
    QCOMPARE(m_archiveView->property("count").toInt(), num);
}

void TestUI::newTask()
{
    mouseClick("addIconItem");
    sendText("test task1");
    sendKey(Qt::Key_Enter);
}

QQuickItem *TestUI::taskContextMenu() const
{
    return m_view->itemByName("taskContextMenu");
}

QQuickItem *TestUI::newTagDialog() const
{
    return m_view->itemByName("newTagDialog");
}

void TestUI::gotoLater()
{
    if (m_controller->queueType() == Controller::QueueTypeArchive)
        return;

    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    QQuickItem *switchItem = m_view->itemByName("switchItem");
    QVERIFY(switchItem);
    QVERIFY(switchItem->isVisible());

    mouseClick(switchItem); // Click the switch

    QCOMPARE(m_controller->queueType(), Controller::QueueTypeArchive);
}

void TestUI::gotoToday()
{
    if (m_controller->queueType() == Controller::QueueTypeToday)
        return;

    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    QQuickItem *switchItem = m_view->itemByName("switchItem");
    QVERIFY(switchItem);
    QVERIFY(switchItem->isVisible());

    mouseClick(switchItem); // Click the switch

    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);
}

void TestUI::cleanupTestCase()
{
}

void TestUI::testQueueSwitchButton()
{
    gotoLater();
    gotoToday();
}

void TestUI::testGoToConfigurePage()
{
    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    mouseClick("configureIcon");
    QCOMPARE(m_controller->currentPage(), Controller::ConfigurePage);
    mouseClick("configureIcon");
    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    mouseClick("configureIcon"); // Open configure again
    QCOMPARE(m_controller->currentPage(), Controller::ConfigurePage);
    mouseClick("configurePageOKButton"); // Dismiss by clicking OK button
    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
}

void TestUI::testGoToAboutPage()
{

}

static void printObjectTree(QObject *root)
{
    static int level = 0;
    qDebug() << QString().fill(' ', level) << root << ";" << root;
    level++;
    foreach (QObject *child, root->children()) {
        printObjectTree(child);
    }
    level--;
}

static void printItemTree(QQuickItem *root)
{
    static int level = 0;
    qDebug() << QString().fill(' ', level) << root << "; parentObject=" << root->parent() << "; parentItem=" << root->parentItem();
    level++;
    foreach (QQuickItem *child, root->childItems()) {
        printItemTree(child);
    }
    level--;
}

void TestUI::testNewTask()
{
    newTask();
    QCOMPARE(m_storage->taskCount(), 1);
    Task::Ptr task1 = m_storage->tasks().at(0);
    QCOMPARE(task1->summary(), QString("test task1"));
    QQuickItem *contextMenu = taskContextMenu();
    QVERIFY(contextMenu->isVisible());
    sendKey(Qt::Key_Escape);
    QVERIFY(!contextMenu->isVisible());
    QVERIFY(m_controller->expanded());

    expectedTodayTasks(1);

    QQuickItem *taskTextItem = m_view->itemByName("taskTextItem");
    QVERIFY(taskTextItem);
    QCOMPARE(taskTextItem->property("text").toString(), QString("test task1"));
}

void TestUI::testArchiveTask()
{
    QQuickItem *archiveIcon = m_view->itemByName("archiveIcon");
    QQuickItem *playIcon = m_view->itemByName("playIcon");
    QVERIFY(archiveIcon);
    QVERIFY(!archiveIcon->isVisible());
    QQuickItem* taskItem = m_view->itemByName("taskItem");
    moveMouseTo(taskItem);
    QVERIFY(archiveIcon->isVisible());
    QVERIFY(playIcon->isVisible());
    mouseClick(archiveIcon);
    expectedTodayTasks(0);
    expectedArchivedTasks(1);
    gotoLater();
    QList<QQuickItem*> items = m_view->itemsByName("taskItem", m_archiveView);
    QCOMPARE(items.count(), 1);
    taskItem = items.first();
    moveMouseTo(taskItem);
    archiveIcon = m_view->itemByName("archiveIcon");
    playIcon = m_view->itemByName("playIcon");
    QVERIFY(playIcon);
    QVERIFY(!playIcon->isVisible());
    mouseClick(archiveIcon); // Stage

    expectedTodayTasks(1);
    expectedArchivedTasks(0);
    gotoToday();
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);
    QVERIFY(m_stagedView->isVisible());
    items = m_view->itemsByName("taskItem", m_stagedView);
    QCOMPARE(items.count(), 1);
}

void TestUI::testPlayTask()
{
    QQuickItem *playIcon = m_view->itemByName("playIcon");
    QQuickItem* taskItem = m_view->itemByName("taskItem");
    moveMouseTo(taskItem);
    mouseClick(playIcon);
    QVERIFY(!m_controller->expanded());

    QQuickItem *globalMouseArea = m_view->itemByName("header");
    mouseClick(globalMouseArea);
    QVERIFY(m_controller->expanded());
    expectedTodayTasks(0);
    Task::Ptr task1 = m_storage->tasks().at(0);
    QCOMPARE(task1->status(), TaskStarted);

    QQuickItem *progressIndicator = m_view->itemByName("circularProgressIndicator");
    QVERIFY(progressIndicator);
    QVERIFY(progressIndicator->isVisible());
    mouseClick(progressIndicator); // Stop
    expectedTodayTasks(1);
    QVERIFY(!progressIndicator->isVisible());
}

void TestUI::testKeyBindings()
{
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);
    sendKey(Qt::Key_Tab, "", Qt::ControlModifier);
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeArchive);
    sendKey(Qt::Key_Tab, "", Qt::ControlModifier);
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);

    // TODO: Test more bindings
}

void TestUI::testShowMenuAfterAddTask()
{
    QVERIFY(m_settings->showContextMenuAfterAdd());

    // Add task
    newTask();
    QCOMPARE(m_storage->taskCount(), 2);

    // Check that menu is visible
    QVERIFY(taskContextMenu()->isVisible());
    sendKey(Qt::Key_Escape); // Dismiss
    QVERIFY(!taskContextMenu()->isVisible());

    // Go to configure and change setting
    mouseClick("configureIcon");
    mouseClick("showContextMenuAfterAddCheckbox");
    QVERIFY(!m_settings->showContextMenuAfterAdd());
    mouseClick("configureIcon"); // Back to today view

    // Add new task
    newTask();
    QCOMPARE(m_storage->taskCount(), 3);
    QVERIFY(!taskContextMenu()->isVisible()); // It's not visible

    // Restore behaviour
    m_kernel->settings()->setShowContextMenuAfterAdd(true);
}

void TestUI::testAddUntaggedBug()
{
    m_storage->clearTasks();

    // Bug: Adding a tag in the untagged area shouldn't create a tag with "Untagged" tag
    gotoLater();

    // Add new task
    newTask();
    QCOMPARE(m_storage->taskCount(), 1);
    QVERIFY(m_storage->tasks().at(0)->tags().isEmpty());
    expectedArchivedTasks(1);
}

void TestUI::testEnterDismissMenu()
{
    gotoToday();
    newTask();

    auto menu = taskContextMenu();
    QVERIFY(menu->isVisible());
    sendKey(Qt::Key_Enter);
    QVERIFY(!menu->isVisible());
}

void TestUI::testNewTagDialog()
{
    // Test Esc dismisses
    newTask();
    auto menu = taskContextMenu();
    QVERIFY(menu->isVisible());

    auto newTagItem = m_view->itemsByName("taskMenuChoice").at(0); // 0 is the header
    qDebug() << newTagItem << "foo" << newTagItem->property("fontAwesomeIconCode");
    QVERIFY(newTagItem);
    QTest::qWait(400); // otherwise test fails for some reason
    mouseClick(newTagItem);
    auto tagDialog = newTagDialog();
    QVERIFY(tagDialog->isVisible());
    sendKey(Qt::Key_Escape);
    QVERIFY(!tagDialog->isVisible());
    QVERIFY(menu->isVisible());
    sendKey(Qt::Key_Escape);
    QVERIFY(!menu->isVisible());

    // Test Enter dismisses if no text
    newTask();
    QVERIFY(menu->isVisible());

    const int tagCount = m_storage->tags().count();

    newTagItem = m_view->itemsByName("taskMenuChoice").at(0); // 0 is the header
    qDebug() << newTagItem << "foo" << newTagItem->property("fontAwesomeIconCode");
    QVERIFY(newTagItem);
    QTest::qWait(400); // otherwise test fails for some reason
    mouseClick(newTagItem);
    QVERIFY(tagDialog->isVisible());
    sendKey(Qt::Key_Enter);
    QVERIFY(!tagDialog->isVisible());
    QCOMPARE(tagCount, m_storage->tags().count());
    QVERIFY(menu->isVisible());
    newTagItem = m_view->itemsByName("taskMenuChoice").at(0); // 0 is the header
    mouseClick(newTagItem);
    sendText("Hello");
    sendKey(Qt::Key_Enter);
    sendKey(Qt::Key_Escape);
    QVERIFY(!menu->isVisible());
    QCOMPARE(tagCount + 1, m_storage->tags().count());
    QVERIFY(m_storage->containsTag("Hello"));
}
