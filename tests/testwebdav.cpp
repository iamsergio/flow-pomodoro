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

    connect(m_kernel->webdavSyncer(), &WebDAVSyncer::testSettingsFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_kernel2->webdavSyncer(), &WebDAVSyncer::testSettingsFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_kernel->webdavSyncer(), &WebDAVSyncer::syncFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_kernel2->webdavSyncer(), &WebDAVSyncer::syncFinished,
            this, &TestWebDav::onSyncFinished);
    connect(m_kernel->webdavSyncer(), &WebDAVSyncer::removeFinished,
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

static void validateSync(SSyncable::List expectedTasks, Storage *storage)
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

void TestWebDav::testSyncTasks()
{
    // Clean the previous one first, if any
    m_kernel->webdavSyncer()->remove("/" + m_kernel->runtimeConfiguration().webDAVFileName());
    waitForIt();
    m_kernel->webdavSyncer()->remove("/" + m_kernel->runtimeConfiguration().webDAVFileName() + ".locl");
    waitForIt();

    WebDAVSyncer *syncer1 = m_kernel->webdavSyncer();
    WebDAVSyncer *syncer2 = m_kernel2->webdavSyncer();
    Storage *storage1 = m_kernel->storage();
    Storage *storage2 = m_kernel2->storage();
    storage1->setObjectName("storage1");
    storage2->setObjectName("storage2");

    //--------------------------------------------------------------------------
    // Warm up
    syncer1->sync();
    waitForIt();

    syncer2->sync();
    waitForIt();
    //--------------------------------------------------------------------------
    // Case 1: client A creates task, syncs, B syncs and gets task.
    Task::Ptr task1 = storage1->addTask("task1");
    QString uid1 = task1->uuid();
    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();

    SSyncable syncable;
    SSyncable::List expected;

    syncable.text = task1->summary();
    syncable.uid = uid1;
    expected << syncable;

    validateSync(expected, storage1);
    validateSync(expected, storage2);


    // Sync again, nothing should change
    syncer1->sync();
    waitForIt();
    validateSync(expected, storage1);
    //--------------------------------------------------------------------------
    // Case 1a: Client A creates another task
    Task::Ptr task2 = storage1->addTask("task2");
    QString uid2 = task2->uuid();

    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();

    syncable.text = task2->summary();
    syncable.uid = uid2;

    expected << syncable;

    validateSync(expected, storage1);
    validateSync(expected, storage2);
    //--------------------------------------------------------------------------
    // Case 2: Client A removes a task
    storage1->removeTask(task2);
    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();

    expected.removeLast();

    validateSync(expected, storage1);
    validateSync(expected, storage2);
    //--------------------------------------------------------------------------
    // Case 3: Client A creates a task, syncs, Client B creates a task before syncing, then syncs
    QString uuid3a = storage1->addTask("Task3A")->uuid();
    syncer1->sync();
    waitForIt();
    expected << SSyncable({uuid3a, "Task3A" });
    validateSync(expected, storage1);

    QString uuid3b = storage2->addTask("Task3B")->uuid();
    syncer2->sync();
    waitForIt();
    expected << SSyncable({uuid3b, "Task3B"});
    validateSync(expected, storage2);
    syncer1->sync();
    waitForIt();
    validateSync(expected, storage1);
    //--------------------------------------------------------------------------
    // Case 4: Client B deletes all
    storage2->clearTasks();
    validateSync(SSyncable::List(), storage2);

    syncer2->sync();
    waitForIt();
    syncer1->sync();
    waitForIt();
    validateSync(SSyncable::List(), storage1);
    validateSync(SSyncable::List(), storage2);
    //--------------------------------------------------------------------------
    // Add some tasks again
    uid1 = storage1->addTask("Task1")->uuid();
    uid2 = storage1->addTask("Task2")->uuid();
    QString uid3 = storage1->addTask("Task3")->uuid();
    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();
    expected.clear();
    expected << SSyncable({uid1, "Task1"}) << SSyncable({uid2, "Task2"}) << SSyncable({uid3, "Task3"});
    validateSync(expected, storage1);
    validateSync(expected, storage2);
    //--------------------------------------------------------------------------
    // Case 5: Client A edits one task, client B edits another task
    task1 = storage1->taskAt(0);
    task2 = storage2->taskAt(1);
    QCOMPARE(task1->revision(), 0);
    task1->setSummary("Hello1");
    QCOMPARE(task1->revision(), 1);
    syncer1->sync();
    waitForIt();
    QCOMPARE(task1->revision(), 1);
    QCOMPARE(task2->revision(), 0);
    task2->setSummary("Hello2");
    QCOMPARE(task2->revision(), 1);
    syncer2->sync();
    waitForIt();
    QCOMPARE(task2->revision(), 1);
    syncer1->sync(); // Fetch client B's change
    waitForIt();
    expected.clear();
    expected << SSyncable({uid1, "Hello1"}) << SSyncable({uid2, "Hello2"}) << SSyncable({uid3, "Task3"});
    validateSync(expected, storage1);
    validateSync(expected, storage2);
    //--------------------------------------------------------------------------
    // Both remove the same task
    storage1->removeTask(storage1->taskAt(0));
    storage2->removeTask(storage2->taskAt(0));
    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();
    expected.clear();
    expected << SSyncable({uid2, "Hello2"}) << SSyncable({uid3, "Task3"});
    validateSync(expected, storage1);
    validateSync(expected, storage2);
    //--------------------------------------------------------------------------
    // Both edit the same task now. We don't have a conflict dialog, first to sync wins.
    storage1->taskAt(0)->setSummary("Edited1");
    storage2->taskAt(0)->setSummary("Edited2");
    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();
    syncer1->sync();
    waitForIt();
    expected.clear();
    expected << SSyncable({uid2, "Edited1"}) << SSyncable({uid3, "Task3"});
    validateSync(expected, storage1);
    validateSync(expected, storage2);
    //--------------------------------------------------------------------------
    // TODO: Test concurrency
    //--------------------------------------------------------------------------
    checkStorageConsistency();
}

void TestWebDav::testSyncTags()
{
    Storage *storage1 = m_kernel->storage();
    //Storage *storage2 = m_kernel2->storage();
    foreach ( const Tag::Ptr &tag, storage1->tags()) {
        qDebug() << "Tag: " << tag->name();
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
