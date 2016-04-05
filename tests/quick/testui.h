/*
  This file is part of Flow.

  Copyright (C) 2015-2016 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef TEST_UI_H
#define TEST_UI_H

#include "testbase.h"
#include "task.h"
#include <QObject>

class QQuickItem;
class TestUI : public TestBase
{
    Q_OBJECT
public:
    explicit TestUI();

private:
    void gotoLater();
    void gotoToday();
    void expandFlow();
    void expectedTodayTasks(int num);
    void expectedArchivedTasks(int num);
    void newTask(bool dismissMenu = false);
    QQuickItem *taskContextMenu() const;
    QQuickItem *newTagDialog() const;
    QQuickItem *menuIndicator(int index) const;
    QQuickItem *taskEditor() const;
    QQuickItem *dueDateExpander() const;
    void refreshEditItem();
    void clickEditItem();
    void okTaskEditor();
    void clickMenuIndicator(int index);
    QList<QQuickItem*> taskItems(QQuickItem* view) const;
    Task::List tasksForItems(QList<QQuickItem*> items) const;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testQueueSwitchButton();
    void testGoToConfigurePage();
    void testGoToAboutPage();
    void testNewTask();
    void testArchiveTask();
    void testPlayTask();
    void testKeyBindings();
    void testShowMenuAfterAddTask();
    void testAddUntaggedBug();
    void testEnterDismissMenu();
    void testNewTagDialog();
    void testDueDate();
    void testPriority();
    void testRecurrence();
private:
    QQuickItem *m_archiveView = nullptr;
    QQuickItem *m_stagedView = nullptr;
    QQuickItem *m_editItem = nullptr;
};

#endif
