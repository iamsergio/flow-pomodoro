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
