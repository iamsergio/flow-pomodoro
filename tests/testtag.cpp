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

    m_storage->removeTask(task1);
    QCOMPARE(m_storage->tasks().count(), 1);
    QCOMPARE(m_tagA->taskCount(), 1);

    checkStorageConsistency();
}
