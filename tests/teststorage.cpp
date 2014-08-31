#include "teststorage.h"
#include "kernel.h"
#include "tag.h"
#include "task.h"
#include "storage.h"
#include "signalspy.h"
#include "runtimeconfiguration.h"

void Tests::initTestCase()
{
    m_kernel = Kernel::instance();
    m_storage = m_kernel->storage();
    RuntimeConfiguration config;
    config.setDataFileName("data.dat");
    m_kernel->setRuntimeConfiguration(config);

    m_storageSpy.listenTo(m_storage, &Storage::tagAboutToBeRemoved);
}

void Tests::cleanupTestCase()
{
    qDebug() << Q_FUNC_INFO << "signal count" << m_storageSpy.count();
    delete m_kernel;
    m_kernel = 0;
}

void Tests::testCreateTag()
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

void Tests::testDeleteTag()
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

void Tests::testContainsTag()
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

void Tests::testTag()
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

void Tests::testIndexOf()
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

void Tests::testRenameTag()
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

void Tests::testAddTask()
{
    Task::Ptr task = m_storage->addTask("t1");
    QVERIFY(task);
    QCOMPARE(m_storage->tasks().count(), 1);
    task = m_storage->addTask("t2");
    QCOMPARE(m_storage->tasks().count(), 2);
    QVERIFY(task);
}

void Tests::testDeleteTask()
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

void Tests::testPrependTask()
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

QTEST_MAIN(Tests)
