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
}
