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
#include "task.h"
#include "storage.h"
#include "kernel.h"
#include "modelsignalspy.h"
#include <QtTest/QtTest>

TestCheckableTagModel::TestCheckableTagModel()
    : TestBase()
{
}

void TestCheckableTagModel::initTestCase()
{
}

void TestCheckableTagModel::cleanupTestCase()
{
    delete m_kernel;
}

void TestCheckableTagModel::testHasTags()
{
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

    Task::Ptr task1 = m_storage->tasks().at(0);
    ModelSignalSpy modelSpy(task1->checkableTagModel());
    m_storage->createTag("tagZZZ");

    QCOMPARE(modelSpy.count(), 2);
    QCOMPARE(modelSpy.caughtSignals().at(0).name, QString("rowsAboutToBeInserted"));
    QCOMPARE(modelSpy.caughtSignals().at(1).name, QString("rowsInserted"));
    modelSpy.clear();
    m_storage->removeTag("tagZZZ");
    QCOMPARE(modelSpy.count(), 2);
    QCOMPARE(modelSpy.caughtSignals().at(0).name, QString("rowsAboutToBeRemoved"));
    QCOMPARE(modelSpy.caughtSignals().at(1).name, QString("rowsRemoved"));
}

void TestCheckableTagModel::testDataChanged()
{
    // Here we test if it reacts to taggs being toggled
    m_storage->clearTags();
    m_storage->clearTasks();
    Task::Ptr task1 = m_storage->addTask("task1");
    m_storage->createTag("tagZZZ");
    QCOMPARE(task1->checkableTagModel()->rowCount(), 1);
    QVERIFY(!task1->checkableTagModel()->index(0, 0).data(Qt::CheckStateRole).toBool());
    ModelSignalSpy modelSpy(task1->checkableTagModel());
    task1->addTag("tagZZZ");
    QCOMPARE(task1->tags().count(), 1);
    QCOMPARE(modelSpy.count(), 1);
    QCOMPARE(task1->checkableTagModel()->rowCount(), 1);
    QVERIFY(task1->checkableTagModel()->index(0, 0).data(Qt::CheckStateRole).toBool());
    modelSpy.clear();
    task1->removeTag("tagZZZ");
    QCOMPARE(modelSpy.count(), 1);
    QVERIFY(!task1->checkableTagModel()->index(0, 0).data(Qt::CheckStateRole).toBool());
    modelSpy.dumpDebugInfo();

}
