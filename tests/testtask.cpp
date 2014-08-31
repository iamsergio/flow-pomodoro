#include "testtask.h"

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
    tags << TagRef(m_task1.data(), "tagA")
         << TagRef(m_task1.data(), "tagB")
         << TagRef(m_task1.data(), "tagC")
         << TagRef(m_task1.data(), "tagC") // Duplicated, won't be added
         << TagRef(m_task1.data(), " TagC "); // Also won't be added

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
}

void TestTask::testAddAndRemoveTag()
{
    m_task1->setTagList(TagRef::List());
    m_storage->clearTasks();
    m_storage->clearTags();
    m_storage->setCreateNonExistentTags(true);
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

    // And now remove them
    m_spy.clear();
    m_task1->removeTag("tagB");
    m_task1->removeTag("tagA");
    m_task1->removeTag("tagC");
    m_task1->removeTag("");
    m_task1->removeTag(" ");
    m_task1->removeTag("taga");
    QCOMPARE(m_task1->tags().count(), 0);
    expectedSignals = { "changed", "tagsChanged", "changed", "tagsChanged", "changed", "tagsChanged" };
    QCOMPARE(m_spy.caughtSignals(), expectedSignals);

    QCOMPARE(Tag::tagCount, m_storage->tags().count());
    QCOMPARE(Tag::tagCount, 3);
    m_storage->setCreateNonExistentTags(true);
}
