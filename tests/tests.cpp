#include "tests.h"
#include "kernel.h"
#include "tag.h"
#include "task.h"
#include "storage.h"
#include "runtimeconfiguration.h"

void Tests::initTestCase()
{
    m_kernel = Kernel::instance();
    m_storage = m_kernel->storage();
    RuntimeConfiguration config;
    config.setDataFileName("data.dat");
    m_kernel->setRuntimeConfiguration(config);
}

void Tests::cleanupTestCase()
{
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
    m_storage->clearTags();
    QVERIFY(m_storage->tags().isEmpty());

    m_storage->createTag("t1");
    QCOMPARE(m_storage->tags().count(), 1);
    QVERIFY(m_storage->removeTag("t1"));
    QCOMPARE(m_storage->tags().count(), 0);

    m_storage->createTag("t1");
    QCOMPARE(m_storage->tags().count(), 1);
    QVERIFY(m_storage->removeTag("T1 ")); // upper case and whitespace
    QCOMPARE(m_storage->tags().count(), 0);

    QVERIFY(!m_storage->removeTag("T1 ")); // Remove again, should not crash.
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

QTEST_MAIN(Tests)
