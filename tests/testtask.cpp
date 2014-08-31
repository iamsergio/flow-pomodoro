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
}

void TestTask::cleanupTestCase()
{
}

void TestTask::testSetTags()
{
    TagRef::List tags;
    tags << TagRef(m_task1.data(), "tagA")
         << TagRef(m_task1.data(), "tagB")
         << TagRef(m_task1.data(), "tagC")
         << TagRef(m_task1.data(), "tagC"); // Duplicated, won't be added

    m_task1->setTagList(tags);
    QCOMPARE(3, m_task1->tags().count());
}
