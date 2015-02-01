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

#include "testtask.h"
#include "storage.h"

TestTask::TestTask() : TestBase()
{
}

void TestTask::initTestCase()
{
    m_storage->clearTags();
    m_storage->clearTasks();
    m_task1 = m_storage->addTask("task1");
    m_task2 = m_storage->addTask("task2");

    m_storage->createTag("tagA");
    m_storage->createTag("tagB");
    m_storage->createTag("tagC");

    m_spy.listenTo(m_task1.data(), &Task::summaryChanged);
    m_spy.listenTo(m_task1.data(), &Task::descriptionChanged);
    m_spy.listenTo(m_task1.data(), &Task::tagsChanged);
    m_spy.listenTo(m_task1.data(), &Task::statusChanged);
    m_spy.listenTo(m_task1.data(), &Task::changed);
}

void TestTask::cleanupTestCase()
{
}

void TestTask::testSetTags()
{
    m_spy.clear();
    TagRef::List tags;
    tags << TagRef(m_task1.data(), "tagA", m_storage)
         << TagRef(m_task1.data(), "tagB", m_storage)
         << TagRef(m_task1.data(), "tagC", m_storage)
         << TagRef(m_task1.data(), "tagC", m_storage) // Duplicated, won't be added
         << TagRef(m_task1.data(), " TagC ", m_storage); // Also won't be added

    m_task1->setTagList(tags);
    QCOMPARE(3, m_task1->tags().count());
    QStringList expectedSignals = { "changed", "tagsChanged", "changed", "tagsChanged", "changed", "tagsChanged" };
    QCOMPARE(m_spy.caughtSignals(), expectedSignals);

    // Task::containsTag()
    QVERIFY(m_task1->containsTag("tagA"));
    QVERIFY(m_task1->containsTag("tagB"));
    QVERIFY(m_task1->containsTag("tagC"));
    QVERIFY(m_task1->containsTag("tAgC ")); // Should exist
    QVERIFY(!m_task1->containsTag("NOT"));
    QVERIFY(!m_task1->containsTag(""));

    // Task::indexOfTag()
    QCOMPARE(m_task1->indexOfTag(""), -1);
    QCOMPARE(m_task1->indexOfTag("tagA"), 0);
    QCOMPARE(m_task1->indexOfTag("tagB"), 1);
    QCOMPARE(m_task1->indexOfTag("tagC"), 2);
    QCOMPARE(m_task1->indexOfTag("tAgC "), 2);
    QCOMPARE(m_task1->indexOfTag("tag"), -1);

    QVERIFY(checkStorageConsistency());
}

void TestTask::testAddAndRemoveTag()
{
    m_task1->setTagList(TagRef::List());
    m_storage->clearTasks();
    m_storage->clearTags();
    m_spy.clear();
    QCOMPARE(Tag::tagCount, 0);
    m_task1->addTag("tagA");
    m_task1->addTag("tagB");
    m_task1->addTag("tagC");
    m_task1->addTag("tagC"); // dup
    m_task1->addTag("tAgC "); // dup
    m_task1->addTag(""); // empty

    QCOMPARE(m_task1->tags().count(), 3);
    QStringList expectedSignals = { "changed", "tagsChanged", "changed", "tagsChanged", "changed", "tagsChanged" };
    QCOMPARE(m_spy.caughtSignals(), expectedSignals);
    QVERIFY(checkStorageConsistency());

    // And now remove them
    m_spy.clear();
    m_task1->removeTag("tagB");
    m_task1->removeTag("tagA");
    m_task1->removeTag("tagC");
    m_task1->removeTag("");
    m_task1->removeTag(" ");
    m_task1->removeTag("taga");
    QCOMPARE(m_task1->tags().count(), 0);
    expectedSignals = QStringList({ "changed", "tagsChanged", "changed", "tagsChanged", "changed", "tagsChanged" });
    QCOMPARE(m_spy.caughtSignals(), expectedSignals);

    QCOMPARE(Tag::tagCount, m_storage->tags().count());
    QCOMPARE(Tag::tagCount, 3);
    QVERIFY(checkStorageConsistency());
}

void TestTask::testJson()
{
    Task::Ptr task = Task::createTask(m_kernel, "test task");
    task->setDescription("some description");
    task->setStaged(true);
    TagRef::List tags;
    tags << TagRef(m_task1.data(), "tagA", m_storage);
    task->setTagList(tags);

    QVariantMap map = task->toJson();
    Task::Ptr task2 = Task::createTask(m_kernel, "another task");
    task2->fromJson(map);

    QCOMPARE(task->summary(), task2->summary());
    QCOMPARE(task->description(), task2->description());
    QCOMPARE(task->staged(), task2->staged());
    QCOMPARE(task->tags().count(), task2->tags().count());
    QCOMPARE(task->tags().at(0).tagName(), task2->tags().at(0).tagName());
}

void TestTask::testToggleTag()
{
    Task::Ptr task = Task::createTask(m_kernel, "testToggleTag");
    QCOMPARE(task->tags().count(), 0);
    task->addTag("tagA");
    QVERIFY(task->containsTag("tagA"));
    task->toggleTag("tagA");
    QVERIFY(!task->containsTag("tagA"));
}

void TestTask::testDueDate()
{
    Task::Ptr task = Task::createTask(m_kernel, "testDueDate");
    QCOMPARE(task->dueDateString(), QString());
    task->setDueDate(QDate::currentDate());
    QCOMPARE(task->dueDateString(), QDate::currentDate().toString());
    task->removeDueDate();
    QCOMPARE(task->dueDateString(), QString());
}
