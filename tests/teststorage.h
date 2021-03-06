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
#include "signalspy.h"
#include <QtTest/QtTest>

class TestStorage: public TestBase
{
    Q_OBJECT
public:
    TestStorage();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCreateTag();
    void testDeleteTag();
    void testContainsTag();
    void testTag();
    void testIndexOf();
    void testRenameTag();

    void testAddTask();
    void testDeleteTask();
    void testPrependTask(); // indexOfItem, taskAt

    void testPreserveInstanceId();
    void testSaveCount();

private:
    SignalSpy m_storageSpy;
};
