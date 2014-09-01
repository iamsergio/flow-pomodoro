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

#include "testtag.h"

TestTag::TestTag() : TestBase()
{
}

void TestTag::initTestCase()
{
    m_storage->clearTags();
    m_storage->clearTasks();

    m_tagA = m_storage->createTag("tagA");
    m_tagB = m_storage->createTag("tagB");
    m_tagC = m_storage->createTag("tagC");

    m_spyA.listenTo(m_tagA.data(), &Tag::taskCountChanged);
    m_spyA.listenTo(m_tagA.data(), &Tag::nameChanged);
    m_spyA.listenTo(m_tagA.data(), &Tag::beingEditedChanged);

    QCOMPARE(m_tagA->taskCount(), 0);
    QCOMPARE(m_tagB->taskCount(), 0);
}

void TestTag::cleanupTestCase()
{
}

void TestTag::testSetName()
{
    m_spyA.clear();
    m_tagA->setName(" tAgA ");
    QCOMPARE(m_spyA.count(), 0);

    m_tagA->setName("");
    QCOMPARE(m_spyA.count(), 0);

    m_tagA->setName("X");
    QCOMPARE(m_spyA.caughtSignals(), {"nameChanged"});
    m_spyA.clear();
    m_tagA->setName("tagA ");
    QCOMPARE(m_tagA->name(), QString("tagA"));
    QCOMPARE(m_spyA.caughtSignals(), {"nameChanged"});
}

void TestTag::testTaskCount()
{
    m_spyA.clear();
    m_storage->clearTasks();

    Task::Ptr task1 = m_storage->addTask("task1");
    Task::Ptr task2 = m_storage->addTask("task2");

    task1->addTag("tagA");
    task1->addTag("tagB");
    task2->addTag("tagA");

    QCOMPARE(m_tagA->taskCount(), 2);
    QCOMPARE(m_tagB->taskCount(), 1);
    QCOMPARE(m_tagC->taskCount(), 0);
    QStringList expectedSignals = {"taskCountChanged", "taskCountChanged"};
    QCOMPARE(m_spyA.caughtSignals(), expectedSignals);
    checkStorageConsistency();

    // Remove task and see if count changes
    m_spyA.clear();
    m_storage->removeTask(task1);
    QCOMPARE(m_storage->tasks().count(), 1);
    QCOMPARE(m_tagA->taskCount(), 1);

    // Remove tag
    QCOMPARE(task2->tags().count(), 1);
    m_storage->removeTag(m_tagA->name());
    QCOMPARE(task2->tags().count(), 0);
    checkStorageConsistency();
}

void TestTag::testJson()
{
    Tag::Ptr tag1 = Tag::Ptr(new Tag("tag1"));
    Tag::Ptr tag2 = Tag::Ptr(new Tag("tag2"));

    QVariantMap map = tag1->toJson();
    tag2->fromJson(map);

    QCOMPARE(tag1->name(), tag2->name());
}
