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

#include <QQuickItem>

TestUI::TestUI() : TestBase()
{
}

void TestUI::initTestCase()
{
    createInstanceWithUI();
    m_view->show();
}

void TestUI::cleanupTestCase()
{
}

void TestUI::testQueueSwitchButton()
{
    QQuickItem *globalMouseArea = m_view->itemByName("header");
    QVERIFY(globalMouseArea);
    QVERIFY(globalMouseArea->isVisible());
    QVERIFY(!m_controller->expanded());
    mouseClick(globalMouseArea);

    QQuickItem *switchItem = m_view->itemByName("switchItem");
    QVERIFY(switchItem);
    QVERIFY(switchItem->isVisible());
    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);
    mouseClick(switchItem); // Switch to later view

    QVERIFY(m_controller->expanded());
    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeArchive);
    mouseClick(switchItem); // Switch to today view

    QCOMPARE(m_controller->currentPage(), Controller::MainPage);
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);
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
    mouseClick("addIconItem");
    sendText("test task1");
    sendKey(Qt::Key_Enter);
    QCOMPARE(m_storage->taskCount(), 1);
    Task::Ptr task1 = m_storage->tasks().at(0);
    QCOMPARE(task1->summary(), QString("test task1"));
    QQuickItem *contextMenu = m_view->itemByName("taskContextMenu");
    QVERIFY(contextMenu->isVisible());
    sendKey(Qt::Key_Escape);
    QVERIFY(!contextMenu->isVisible());
    QVERIFY(m_controller->expanded());

    QQuickItem *listView = m_view->itemByName("stagedView");
    QVERIFY(listView);

    QList<QQuickItem*> items = m_view->itemsByName("taskItem");
    QCOMPARE(items.count(), 1);

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
    QQuickItem *todayView = m_view->itemByName("stagedView");
    QCOMPARE(todayView->property("count").toInt(), 0);
    QQuickItem *archiveView = m_view->itemByName("archiveView");
    QCOMPARE(archiveView->property("count").toInt(), 1);

    QQuickItem *switchItem = m_view->itemByName("switchItem");
    QVERIFY(switchItem);
    mouseClick(switchItem); // Go to archive view
    QList<QQuickItem*> items = m_view->itemsByName("taskItem", archiveView);
    QCOMPARE(items.count(), 1);
    taskItem = items.first();
    moveMouseTo(taskItem);
    archiveIcon = m_view->itemByName("archiveIcon");
    playIcon = m_view->itemByName("playIcon");
    QVERIFY(playIcon);
    QVERIFY(!playIcon->isVisible());
    mouseClick(archiveIcon); // Stage

    QCOMPARE(todayView->property("count").toInt(), 1);
    QCOMPARE(archiveView->property("count").toInt(), 0);
    mouseClick(switchItem); // Go to today view
    QCOMPARE(m_controller->queueType(), Controller::QueueTypeToday);
    QVERIFY(todayView->isVisible());
    items = m_view->itemsByName("taskItem", todayView);
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

    QQuickItem *todayView = m_view->itemByName("stagedView");
    QCOMPARE(todayView->property("count").toInt(), 0);
    Task::Ptr task1 = m_storage->tasks().at(0);
    QCOMPARE(task1->status(), TaskStarted);

    QQuickItem *progressIndicator = m_view->itemByName("circularProgressIndicator");
    QVERIFY(progressIndicator);
    QVERIFY(progressIndicator->isVisible());
    mouseClick(progressIndicator); // Stop
    QCOMPARE(todayView->property("count").toInt(), 1);
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
    mouseClick("addIconItem");
    sendText("test task2");
    sendKey(Qt::Key_Enter);
    QCOMPARE(m_storage->taskCount(), 2);

    // Check that menu is visible
    QVERIFY(m_view->itemByName("taskContextMenu")->isVisible());
    sendKey(Qt::Key_Escape); // Dismiss
    QVERIFY(!m_view->itemByName("taskContextMenu")->isVisible());

    // Go to configure and change setting
    mouseClick("configureIcon");
    mouseClick("showContextMenuAfterAddCheckbox");
    QVERIFY(!m_settings->showContextMenuAfterAdd());
    mouseClick("configureIcon"); // Back to today view

    // Add new task
    mouseClick("addIconItem");
    sendText("test task3");
    sendKey(Qt::Key_Enter);
    QCOMPARE(m_storage->taskCount(), 3);
    QVERIFY(!m_view->itemByName("taskContextMenu")->isVisible()); // It's not visible
}

void TestUI::testAddUntaggedBug()
{
    m_storage->clearTasks();

    // Bug: Adding a tag in the untagged area shouldn't create a tag with "Untagged" tag
    QQuickItem *switchItem = m_view->itemByName("switchItem");
    QVERIFY(switchItem);
    mouseClick(switchItem); // Go to archive view

    // Add new task
    mouseClick("addIconItem");
    sendText("test task1");
    sendKey(Qt::Key_Enter);
    QCOMPARE(m_storage->taskCount(), 1);
    QVERIFY(m_storage->tasks().at(0)->tags().isEmpty());
    QCOMPARE(m_view->itemByName("archiveView")->property("count").toInt(), 1);
}
