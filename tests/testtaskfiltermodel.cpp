/*
  This file is part of Flow.

  Copyright (C) 2014-2015 Sérgio Martins <iamsergio@gmail.com>

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

#include "testtaskfiltermodel.h"
#include "controller.h"
#include "storage.h"
#include "taskfilterproxymodel.h"
#include <QtTest/QtTest>

TestTaskFilterModel::TestTaskFilterModel()
    : TestBase()
{
}

void TestTaskFilterModel::initTestCase()
{
}

void TestTaskFilterModel::cleanupTestCase()
{
}

void TestTaskFilterModel::testDueDateFiltering()
{
    Storage::Data emptyData;
    m_storage->setData(emptyData);
    m_controller->setQueueType(Controller::QueueTypeArchive);
    m_controller->setCurrentTag(m_controller->dueDateTasksTag());
    m_controller->addTask("task2", false);
    m_controller->addTask("task1", false);

    Task::Ptr task1 = m_storage->tasks().at(0);
    Task::Ptr task2 = m_storage->tasks().at(1);

    QCOMPARE(task1->summary(), QString("task1"));
    QCOMPARE(task2->summary(), QString("task2"));

    TaskFilterProxyModel *proxyModel = m_storage->dueDateTasksModel();
    Task::Ptr taskAt0 = proxyModel->data(proxyModel->index(0, 0), Storage::TaskPtrRole).value<Task::Ptr>();
    Task::Ptr taskAt1 = proxyModel->data(proxyModel->index(1, 0), Storage::TaskPtrRole).value<Task::Ptr>();

    QCOMPARE(task1, taskAt0);
    QCOMPARE(task2, taskAt1);

    QDate today = QDate::currentDate();
    task1->setDueDate(today.addDays(2));
    task2->setDueDate(today.addDays(1));

    taskAt0 = proxyModel->data(proxyModel->index(0, 0), Storage::TaskPtrRole).value<Task::Ptr>();
    taskAt1 = proxyModel->data(proxyModel->index(1, 0), Storage::TaskPtrRole).value<Task::Ptr>();

    QCOMPARE(task1, taskAt1);
    QCOMPARE(task2, taskAt0);
}
