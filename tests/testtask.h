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

#include "testbase.h"
#include "task.h"
#include "signalspy.h"
#include <QtTest/QtTest>

class TestTask: public TestBase
{
    Q_OBJECT
public:
    TestTask();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testSetTags();
    void testAddAndRemoveTag();
    void testJson();
    void testToggleTag();
    void testDueDate();
private:
    Task::Ptr m_task1;
    Task::Ptr m_task2;
    SignalSpy m_spy;
};
