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

#include "testcheckabletagmodel.h"

TestCheckableTagModel::TestCheckableTagModel()
    : TestBase()
{
}

void TestCheckableTagModel::initTestCase()
{
    m_storage->clearTags();
    m_storage->clearTasks();

    m_task1 = m_storage->addTask("task1");
    m_storage->createTag("tagA");
    m_storage->createTag("tagB");
    m_storage->createTag("tagC");
    m_task2 = m_storage->addTask("task2");

}

void TestCheckableTagModel::cleanupTestCase()
{
}

void TestCheckableTagModel::testHasTags()
{
    QCOMPARE(m_task1->tags().count(), 0);
    QCOMPARE(m_task2->tags().count(), 0);

    QCOMPARE(m_task1->checkableTagModel()->rowCount(), 3);
    QCOMPARE(m_task2->checkableTagModel()->rowCount(), 3);

    // Restart an instance
    createNewKernel("testcheckablemodeltest1.dat");
    QCOMPARE(m_storage->tags().count(), 2);
    QCOMPARE(m_storage->tasks().count(), 2);

    foreach (const Task::Ptr &task, m_storage->tasks()) {
        QIdentityProxyModel *model = qobject_cast<QIdentityProxyModel*>(task->checkableTagModel());
        Q_ASSERT(model);
        QVERIFY(model->sourceModel());
        QCOMPARE(task->checkableTagModel()->rowCount(), 2);
    }
}
