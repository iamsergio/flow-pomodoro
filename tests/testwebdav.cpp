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

#include "testwebdav.h"
#include "webdavsyncer.h"
#include "jsonstorage.h"

TestWebDav::TestWebDav()
    : TestBase()
    , m_webdav(0)
{
}

static Kernel * newKernel(const QString &name)
{
    RuntimeConfiguration config;
    config.setDataFileName("data_files/" + name + ".dat");
    config.setPluginsSupported(false);
    config.setSettings(new Settings(QString("%1.ini").arg(name)));
    config.setSaveEnabled(false);
    config.setWebDAVFileName("unit-test-flow.dat");

    Kernel *kernel = new Kernel(config);
    kernel->storage()->load();

    kernel->controller()->setIsHttps(false);
    kernel->controller()->setPort(55580);
    kernel->controller()->setHost("rpi");
    kernel->controller()->setPath("/uploads");
    kernel->controller()->setUser("admin");
    kernel->controller()->setPassword("12345");

    return kernel;
}

void TestWebDav::initTestCase()
{
    QFile::remove("unit-test-settings.ini");
    QFile::remove("kernelB.ini");

    createNewKernel("kernelA.dat");
    m_kernel->controller()->setIsHttps(false);
    m_kernel->controller()->setPort(55580);
    m_kernel->controller()->setHost("rpi");
    m_kernel->controller()->setPath("/uploads");
    m_kernel->controller()->setUser("admin");
    m_kernel->controller()->setPassword("12345");

    m_kernel2 = newKernel("kernelB");

    m_storage1 = m_kernel->storage();
    m_storage2 = m_kernel2->storage();

    m_storage1->setObjectName("m_storage1");
    m_storage2->setObjectName("m_storage2");

    m_syncer1 = m_kernel->webdavSyncer();
    m_syncer2 = m_kernel2->webdavSyncer();

    connect(m_syncer1, &WebDAVSyncer::testSettingsFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_syncer2, &WebDAVSyncer::testSettingsFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_syncer1, &WebDAVSyncer::syncFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_syncer2, &WebDAVSyncer::syncFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_syncer1, &WebDAVSyncer::removeFinished,
            this, &TestWebDav::onRemoveFinished);
}

void TestWebDav::cleanupTestCase()
{
    QFile::remove("kernelA.ini");
    QFile::remove("kernelB.ini");
}

void TestWebDav::testConnect()
{
    m_kernel->webdavSyncer()->testSettings();
    waitForIt();
}

struct SSyncable {
    typedef QList<SSyncable> List;
    QString uid;
    QString text;
    bool operator==(const SSyncable &other) const
    {
        return uid == other.uid && text == other.text;
    }
};

static void validateTasks(SSyncable::List expectedTasks, Storage *storage)
{
    qDebug() << "Tasks on storage:" << storage->objectName();
    foreach (const Task::Ptr &task, storage->tasks()) {
        qDebug() << task->uuid() << task->summary();
    }

    qDebug() << "Expected tasks:";
    foreach (const SSyncable &syncable, expectedTasks) {
        qDebug() << syncable.uid << syncable.text;
    }

    if (storage->tasks().count() != expectedTasks.count()) {
        QCOMPARE(storage->tasks().count(), expectedTasks.count());
    }

    foreach (const Task::Ptr &task, storage->tasks()) {
        const SSyncable actual = {task->uuid(), task->summary()};
        if (!expectedTasks.contains(actual)) {
            qDebug() << "Storage:" << storage->objectName() << "; culprit="
                     << task->summary() << "; revision=" << task->revision()
                     << "; revision on server=" << task->revisionOnWebDAVServer()
                     << "; uid=" << task->uuid();
            QVERIFY(false);
        }

        expectedTasks.removeOne(actual);
    }
}

static void validateTags(SSyncable::List expectedTags, Storage *storage)
{
    qDebug() << "Tags on storage:" << storage->objectName();
    foreach (const Tag::Ptr &tag, storage->tags())
        qDebug() << tag->uuid() << tag->name();

    qDebug() << "Expected tags:";
    foreach (const SSyncable &syncable, expectedTags)
        qDebug() << syncable.uid << syncable.text;

    if (storage->tags().count() != expectedTags.count())
        QCOMPARE(storage->tags().count(), expectedTags.count());


    foreach (const Tag::Ptr &tag, storage->tags()) {
        const SSyncable actual = {tag->uuid(), tag->name()};
        if (!expectedTags.contains(actual)) {
            qDebug() << "Storage:" << storage->objectName() << "; culprit="
                     << tag->name() << "; revision=" << tag->revision()
                     << "; revision on server=" << tag->revisionOnWebDAVServer()
                     << "; uid=" << tag->uuid();
            QVERIFY(false);
        }

        expectedTags.removeOne(actual);
    }
}

void TestWebDav::testSyncTasks()
{
    // Clean the previous one first, if any
    m_kernel->webdavSyncer()->remove("/" + m_kernel->runtimeConfiguration().webDAVFileName());
    waitForIt();
    m_kernel->webdavSyncer()->remove("/" + m_kernel->runtimeConfiguration().webDAVFileName() + ".locl");
    waitForIt();
    return;
    //--------------------------------------------------------------------------
    // Warm up
    m_syncer1->sync();
    waitForIt();

    m_syncer2->sync();
    waitForIt();
    //--------------------------------------------------------------------------
    // Case 1: client A creates task, syncs, B syncs and gets task.
    Task::Ptr task1 = m_storage1->addTask("task1");
    QString uid1 = task1->uuid();
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();

    SSyncable syncable;
    SSyncable::List expected;

    syncable.text = task1->summary();
    syncable.uid = uid1;
    expected << syncable;

    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);


    // Sync again, nothing should change
    m_syncer1->sync();
    waitForIt();
    validateTasks(expected, m_storage1);
    //--------------------------------------------------------------------------
    // Case 1a: Client A creates another task
    Task::Ptr task2 = m_storage1->addTask("task2");
    QString uid2 = task2->uuid();

    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();

    syncable.text = task2->summary();
    syncable.uid = uid2;

    expected << syncable;

    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Case 2: Client A removes a task
    m_storage1->removeTask(task2);
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();

    expected.removeLast();

    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Case 3: Client A creates a task, syncs, Client B creates a task before syncing, then syncs
    QString uuid3a = m_storage1->addTask("Task3A")->uuid();
    m_syncer1->sync();
    waitForIt();
    expected << SSyncable({uuid3a, "Task3A" });
    validateTasks(expected, m_storage1);

    QString uuid3b = m_storage2->addTask("Task3B")->uuid();
    m_syncer2->sync();
    waitForIt();
    expected << SSyncable({uuid3b, "Task3B"});
    validateTasks(expected, m_storage2);
    m_syncer1->sync();
    waitForIt();
    validateTasks(expected, m_storage1);
    //--------------------------------------------------------------------------
    // Case 4: Client B deletes all
    m_storage2->clearTasks();
    validateTasks(SSyncable::List(), m_storage2);

    m_syncer2->sync();
    waitForIt();
    m_syncer1->sync();
    waitForIt();
    validateTasks(SSyncable::List(), m_storage1);
    validateTasks(SSyncable::List(), m_storage2);
    //--------------------------------------------------------------------------
    // Add some tasks again
    uid1 = m_storage1->addTask("Task1")->uuid();
    uid2 = m_storage1->addTask("Task2")->uuid();
    QString uid3 = m_storage1->addTask("Task3")->uuid();
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();
    expected.clear();
    expected << SSyncable({uid1, "Task1"}) << SSyncable({uid2, "Task2"}) << SSyncable({uid3, "Task3"});
    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Case 5: Client A edits one task, client B edits another task
    task1 = m_storage1->taskAt(0);
    task2 = m_storage2->taskAt(1);
    QCOMPARE(task1->revision(), 0);
    task1->setSummary("Hello1");
    QCOMPARE(task1->revision(), 1);
    m_syncer1->sync();
    waitForIt();
    QCOMPARE(task1->revision(), 1);
    QCOMPARE(task2->revision(), 0);
    task2->setSummary("Hello2");
    QCOMPARE(task2->revision(), 1);
    m_syncer2->sync();
    waitForIt();
    QCOMPARE(task2->revision(), 1);
    m_syncer1->sync(); // Fetch client B's change
    waitForIt();
    expected.clear();
    expected << SSyncable({uid1, "Hello1"}) << SSyncable({uid2, "Hello2"}) << SSyncable({uid3, "Task3"});
    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Both remove the same task
    m_storage1->removeTask(m_storage1->taskAt(0));
    m_storage2->removeTask(m_storage2->taskAt(0));
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();
    expected.clear();
    expected << SSyncable({uid2, "Hello2"}) << SSyncable({uid3, "Task3"});
    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Both edit the same task now. We don't have a conflict dialog, first to sync wins.
    m_storage1->taskAt(0)->setSummary("Edited1");
    m_storage2->taskAt(0)->setSummary("Edited2");
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();
    m_syncer1->sync();
    waitForIt();
    expected.clear();
    expected << SSyncable({uid2, "Edited1"}) << SSyncable({uid3, "Task3"});
    validateTasks(expected, m_storage1);
    validateTasks(expected, m_storage2);
    //--------------------------------------------------------------------------
    // TODO: Test concurrency
    //--------------------------------------------------------------------------
    QVERIFY(checkStorageConsistency(m_storage1->tags().count() + m_storage2->tags().count()));
}

void TestWebDav::testSyncTags()
{
    return;
    //--------------------------------------------------------------------------
    // Warm up
    qDebug() << "Warm up";
    SSyncable::List expected = { { "{bb2ab284-8bb7-4aec-a452-084d64e85697}", "work" },
                                 { "{73533168-9a57-4fc0-ba9a-9120bbadcb6c}", "personal" },
                                 { "{4e81dd75-84c4-4359-912c-f3ead717f694}", "family" },
                                 { "{4b4ae5fb-f35d-4389-9417-96b7ddcb3b8f}", "bills" },
                                 { "{b2697470-f457-461c-9310-7d4b56aea395}", "books" },
                                 { "{387be44a-1eb7-4895-954a-cf5bc82d8f03}", "movies" } };

    validateTags(expected, m_storage1);
    validateTags(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Both client A and B create a TagX
    // Unlike tasks, we don't allow two tags with the same name, they will have to be merged.
    qDebug() << "Both creating tag";
    Tag::Ptr tag = m_storage1->createTag("TagX");
    m_storage2->createTag("TagX");
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();

    expected << SSyncable({tag->uuid(), tag->name()});

    validateTags(expected, m_storage1);
    validateTags(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Client A creates a tag
    qDebug() << "Creating tag";
    tag = m_storage1->createTag("Tag2");
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();
    m_syncer1->sync();
    waitForIt();
    expected << SSyncable({tag->uuid(), tag->name()});
    validateTags(expected, m_storage1);
    validateTags(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Client A renames a tag
    qDebug() << "Renaming tag";
    tag = m_storage1->tag("Tag2");
    QCOMPARE(tag->revision(), 0);
    m_storage1->renameTag("Tag2", "Tag2a");
    QCOMPARE(tag->revision(), 1);

    expected.removeLast();
    expected << SSyncable({tag->uuid(), "Tag2a"});
    validateTags(expected, m_storage1);

    m_syncer1->sync();
    waitForIt();

    validateTags(expected, m_storage1);

    m_syncer2->sync();
    waitForIt();

    validateTags(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Client A removes a tag
    m_storage1->removeTag("Tag2a");
    m_syncer1->sync();
    waitForIt();
    m_syncer2->sync();
    waitForIt();
    expected.removeLast();
    validateTags(expected, m_storage1);
    validateTags(expected, m_storage2);
    //--------------------------------------------------------------------------
    // Client A renames tag, Client B removes it
    m_storage1->renameTag("TagX", "TagX2");
    QVERIFY(m_storage2->removeTag("TagX"));
    m_syncer2->sync();
    waitForIt();

    m_syncer1->sync();
    waitForIt();

    expected.removeLast();
    validateTags(expected, m_storage1);
    validateTags(expected, m_storage2);
    //--------------------------------------------------------------------------
}

void TestWebDav::testTasksAndTags()
{
    m_storage1->clearTags();
    m_storage1->clearTasks();
    m_storage2->clearTags();
    m_storage2->clearTasks();

    m_syncer1->sync();
    waitForIt();

    validateSync("empty.dat");
    //--------------------------------------------------------------------------
    // Client A creates Task1, tagged with Tag1
    Task::Ptr task1 = m_storage1->addTask("Task1");
    task1->setUuid("{f6d334d3-57f0-418f-b44b-7513d8e5e087}");
    m_storage1->createTag("Tag1");
    task1->addTag("Tag1");

    m_syncer1->sync();
    waitForIt();

    m_syncer2->sync();
    waitForIt();
    validateSync("testSyncTags1.dat");

    QCOMPARE(task1, m_storage1->taskAt(0));
    QCOMPARE(static_cast<void*>(task1.data()), static_cast<void*>(m_storage1->taskAt(0).data()));
    QVERIFY(checkStorageConsistency(m_storage1->tags().count() + m_storage2->tags().count()));
    //--------------------------------------------------------------------------
    // Tag1 is removed
    QCOMPARE(task1->tags().count(), 1);
    QVERIFY(m_storage1->removeTag("Tag1"));
    QCOMPARE(task1->tags().count(), 0);

    QCOMPARE(m_storage1->tags().count(), 0);

    m_syncer1->sync();
    waitForIt();
    QCOMPARE(m_storage1->tags().count(), 0);

    m_syncer2->sync();
    waitForIt();

    validateSync("testSyncTags2.dat");
    QVERIFY(checkStorageConsistency(m_storage1->tags().count() + m_storage2->tags().count()));
    //--------------------------------------------------------------------------
    // Create 4 tags and remove task1
    m_storage1->createTag("Tag1");
    m_storage1->createTag("Tag2");
    m_storage2->createTag("Tag3");
    m_storage2->createTag("Tag4");
    m_storage1->removeTask(m_storage1->taskAt(0));
    QCOMPARE(m_storage1->tasks().count(), 0);
    task1.clear();

    m_syncer1->sync();
    waitForIt();

    m_syncer2->sync();
    waitForIt();

    m_syncer1->sync();
    waitForIt();

    QCOMPARE(m_storage1->tasks().count(), 0);
    validateSync("testSyncTags3.dat");
    QVERIFY(checkStorageConsistency(m_storage1->tags().count() + m_storage2->tags().count()));
    //--------------------------------------------------------------------------
    // Create 4 tasks
    task1 = m_storage1->addTask("Task1", "uid1");
    Task::Ptr task2 = m_storage1->addTask("Task2", "uid2");
    Task::Ptr task3 = m_storage2->addTask("Task3", "uid3");
    Task::Ptr task4 = m_storage2->addTask("Task4", "uid4");

    m_syncer1->sync();
    waitForIt();

    m_syncer2->sync();
    waitForIt();

    m_syncer1->sync();
    waitForIt();

    QCOMPARE(task1->uuid(), QString("uid1"));
    Q_UNUSED(task2);
    Q_UNUSED(task3);
    Q_UNUSED(task4);

    validateSync("testSyncTags4.dat");
    QVERIFY(checkStorageConsistency(m_storage1->tags().count() + m_storage2->tags().count()));
    //--------------------------------------------------------------------------
    // tag some tasks
    foreach (const Task::Ptr &task, m_storage1->tasks()) {
        if (task->summary() == "Task1") {
            task->addTag("Tag1");
            task->addTag("Tag2");
            break;
        }
    }

    foreach (const Task::Ptr &task, m_storage2->tasks()) {
        if (task->summary() == "Task3") {
            task->addTag("Tag1");
        } else if (task->summary() == "Task3") {
            task->addTag("Tag4");
        }
    }

    m_syncer1->sync();
    waitForIt();

    m_syncer2->sync();
    waitForIt();

    m_syncer1->sync();
    waitForIt();

    validateSync("testSyncTags5.dat");
    QVERIFY(checkStorageConsistency(m_storage1->tags().count() + m_storage2->tags().count()));
    //--------------------------------------------------------------------------

    JsonStorage *jsonStorage = static_cast<JsonStorage*>(m_storage1);
    qDebug() << "dummping: " << jsonStorage->serializeToJsonData(m_storage1->data());
    //--------------------------------------------------------------------------
}

void compareTasks(const Task::Ptr &task1, const Task::Ptr &task2)
{
    if (task1->summary() != task2->summary() || task1->uuid() != task2->uuid()) {
        qDebug() << "Compared tasks are not the same\n"
                 << task1 << "\n" << task2 << "\n" << task2->kernel()->storage();
        QVERIFY(false);
    }
}

void compareTags(const Tag::Ptr &tag1, const Tag::Ptr &tag2)
{
    if (!(*tag1.data() == *tag2)) {
        qDebug() << "Compared tags are not the same\n"
                 << tag1 << "\n" << tag2 << "\n" << tag2->kernel()->storage();

        QVERIFY(false);
    }
}

static bool tagLessThan(const Tag::Ptr tag1, const Tag::Ptr tag2)
{
    return tag1->name() < tag2->name();
}

static bool taskLessThan(const Task::Ptr task1, const Task::Ptr task2)
{
    return task1->summary() < task2->summary();
}

static Tag::List sortTags(TagList tags)
{
    qSort(tags.begin(), tags.end(), tagLessThan);
    return tags;
}

static Task::List sortTasks(TaskList tasks)
{
    qSort(tasks.begin(), tasks.end(), taskLessThan);
    return tasks;
}

void TestWebDav::validateSync(const QString &filename)
{
    qDebug() << "Validating sync for " << filename;
    QFile file("data_files/" + filename);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray serializedData = file.readAll();
    file.close();

    QString error;
    Storage::Data data =  JsonStorage::deserializeJsonData(serializedData, error, 0);
    QVERIFY(error.isEmpty());

    QCOMPARE(data.tags.count(), m_storage1->tags().count());
    QCOMPARE(data.tags.count(), m_storage2->tags().count());
    QCOMPARE(data.tasks.count(), m_storage1->tasks().count());
    QCOMPARE(data.tasks.count(), m_storage2->tasks().count());


    Tag::List sortedServerTags = sortTags(data.tags);
    Tag::List sortedStorage1Tags = sortTags(m_storage1->tags());
    Tag::List sortedStorage2Tags = sortTags(m_storage2->tags());

    Task::List sortedServerTasks = sortTasks(data.tasks);
    Task::List sortedStorage1Tasks = sortTasks(m_storage1->tasks());
    Task::List sortedStorage2Tasks = sortTasks(m_storage2->tasks());

    for (int i = 0; i < sortedServerTags.count(); ++i) {
        compareTags(sortedServerTags.at(i), sortedStorage1Tags.at(i));
        compareTags(sortedServerTags.at(i), sortedStorage2Tags.at(i));
    }

    for (int i = 0; i < sortedServerTasks.count(); ++i) {
        compareTasks(sortedServerTasks.at(i), sortedStorage1Tasks.at(i));
        compareTasks(sortedServerTasks.at(i), sortedStorage2Tasks.at(i));
    }
}

void TestWebDav::onSyncFinished(bool success, const QString &errorMsg)
{
    qDebug();
    if (!errorMsg.isEmpty()) {
        qWarning() << errorMsg;
        QVERIFY(false);
    }

    QVERIFY(success);
    stopWaiting();
}

void TestWebDav::onRemoveFinished(bool, const QString &)
{
    stopWaiting();
}
