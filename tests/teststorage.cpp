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

#include "teststorage.h"

TestStorage::TestStorage() : TestBase()
{
}

void TestStorage::initTestCase()
{
    m_storageSpy.listenTo(m_storage, &Storage::tagAboutToBeRemoved);
}

void TestStorage::cleanupTestCase()
{
}

void TestStorage::testCreateTag()
{

    TagList tags = m_storage->tags();
    QVERIFY(tags.isEmpty());

    m_storage->createTag("tag1");
    QVERIFY(m_storage->tags().count() == 1);
    m_storage->createTag("tag2");
    m_storage->createTag("tag3");
    QVERIFY(m_storage->tags().count() == 3);

    m_storage->createTag("tag3"); // duplicate!
    QVERIFY(m_storage->tags().count() == 3);

    m_storage->createTag(" Tag3 "); // duplicate! with spaces and uppercase
    QVERIFY(m_storage->tags().count() == 3);

    m_storage->createTag(""); // Empty
    QVERIFY(m_storage->tags().count() == 3);
}

void TestStorage::testDeleteTag()
{
    m_storageSpy.clear();
    m_storage->clearTags();
    QVERIFY(m_storage->tags().isEmpty());

    m_storage->createTag("t1");
    QCOMPARE(m_storage->tags().count(), 1);
    QVERIFY(m_storage->removeTag("t1"));
    QCOMPARE(m_storageSpy.at(0), QString("tagAboutToBeRemoved"));
    m_storageSpy.clear();
    QCOMPARE(m_storageSpy.count(), 0);
    QCOMPARE(m_storage->tags().count(), 0);

    m_storage->createTag("t1");
    QCOMPARE(m_storage->tags().count(), 1);
    QVERIFY(m_storage->removeTag("T1 ")); // upper case and whitespace
    QCOMPARE(m_storage->tags().count(), 0);

    m_storageSpy.clear();
    QVERIFY(!m_storage->removeTag("T1 ")); // Remove again, should not crash.
    QCOMPARE(m_storageSpy.count(), 0); // And should not emit
}

void TestStorage::testContainsTag()
{
    m_storage->clearTags();

    m_storage->createTag("testingContains1");
    QVERIFY(!m_storage->containsTag("testingContains"));
    QVERIFY(m_storage->containsTag("testingContains1"));
    QVERIFY(m_storage->containsTag(" testingCoNtains1 "));
    m_storage->removeTag("testingContains1");
    QVERIFY(!m_storage->containsTag("testingContains1"));
    QVERIFY(!m_storage->containsTag(" testingCoNtains1 "));
}

void TestStorage::testTag()
{
    m_storage->clearTags();
    m_storage->setCreateNonExistentTags(true);

    Tag::Ptr tag = m_storage->tag("tag1");
    QVERIFY(tag);
    QCOMPARE(m_storage->tags().count(), 1);

    tag = m_storage->tag("tag2");
    QVERIFY(tag);

    tag = m_storage->tag("tag3");
    QVERIFY(tag);
    QCOMPARE(m_storage->tags().count(), 3);

    Tag::Ptr tag2 = m_storage->tag("tag3"); // duplicate!
    QCOMPARE(tag, tag2);
    QCOMPARE(m_storage->tags().count(), 3);


    tag2 = m_storage->tag(" Tag3 "); // duplicate! with spaces and uppercase
    QCOMPARE(tag, tag2);
    QCOMPARE(m_storage->tags().count(), 3);

    tag = m_storage->tag(""); // Empty
    QVERIFY(!tag);
    QCOMPARE(m_storage->tags().count(), 3);

    m_storage->setCreateNonExistentTags(false);
    tag = m_storage->tag("Foo");
    QVERIFY(!tag);
    tag = m_storage->tag("tag2");
    QVERIFY(tag->name() == "tag2");
}

void TestStorage::testIndexOf()
{
   m_storage->clearTags();
   QCOMPARE(m_storage->indexOfTag("doesnt exist"), -1);
   m_storage->createTag("0");
   m_storage->createTag("1");
   m_storage->createTag("2");
   m_storage->createTag("3a");

   QCOMPARE(m_storage->indexOfTag("0"), 0);
   QCOMPARE(m_storage->indexOfTag("2"), 2);
   QCOMPARE(m_storage->indexOfTag("3A "), 3); // has space and uppercase
   QCOMPARE(m_storage->indexOfTag("4"), -1);
}

void TestStorage::testRenameTag()
{
    m_storage->clearTags();
    m_storage->createTag("0a");
    QVERIFY(!m_storage->renameTag("0a", " 0A"));
    QVERIFY(m_storage->renameTag("0A", "1"));
    QVERIFY(m_storage->containsTag("1"));
    QVERIFY(!m_storage->renameTag("non-existant", "2"));
    m_storage->createTag("2");
    QVERIFY(!m_storage->renameTag("1", "2")); // Already exists
}

void TestStorage::testAddTask()
{
    Task::Ptr task = m_storage->addTask("t1");
    QVERIFY(task);
    QCOMPARE(m_storage->tasks().count(), 1);
    task = m_storage->addTask("t2");
    QCOMPARE(m_storage->tasks().count(), 2);
    QVERIFY(task);
}

void TestStorage::testDeleteTask()
{
    m_storage->clearTasks();
    QCOMPARE(m_storage->tasks().count(), 0);

    Task::Ptr task1 = m_storage->addTask("t1");
    Task::Ptr task2 = m_storage->addTask("t2");
    QCOMPARE(m_storage->tasks().count(), 2);

    m_storage->removeTask(task1);
    QCOMPARE(m_storage->tasks().count(), 1);
    m_storage->removeTask(task2);
    QCOMPARE(m_storage->tasks().count(), 0);

    // Lets remove it again
    m_storage->removeTask(task2);
    QCOMPARE(m_storage->tasks().count(), 0);
}

void TestStorage::testPrependTask()
{
    m_storage->clearTasks();
    QCOMPARE(m_storage->tasks().count(), 0);

    Task::Ptr task0 = m_storage->prependTask("t1");
    Task::Ptr task1 = m_storage->prependTask("t2");
    Task::Ptr task2 = m_storage->prependTask("t3");

    QCOMPARE(task0, m_storage->taskAt(2));
    QCOMPARE(task1, m_storage->taskAt(1));
    QCOMPARE(task2, m_storage->taskAt(0));

    QCOMPARE(2, m_storage->indexOfItem(m_storage->tasks(), task0));
    QCOMPARE(1, m_storage->indexOfItem(m_storage->tasks(), task1));
    QCOMPARE(0, m_storage->indexOfItem(m_storage->tasks(), task2));
}

void TestStorage::testSetData()
{
    m_storage->setData(Storage::Data());
    QVERIFY(!m_storage->data().instanceId.isEmpty());
}
