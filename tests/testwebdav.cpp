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

static void validateSync(QStringList tasks, Storage *storage)
{
    if (storage->tasks().count() != tasks.count()) {
        qDebug() << "Storage:" << storage->objectName();
        QCOMPARE(storage->tasks().count(), tasks.count());
    }
    foreach (const Task::Ptr &task, storage->tasks()) {
        if (!tasks.contains(task->summary())) {
            qDebug() << "Storage:" << storage->objectName();
            QVERIFY(false);
        }
        tasks.removeOne(task->summary());
    }
}

void TestWebDav::testSync()
{
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
    storage1->addTask("task1");
    syncer1->sync();
    waitForIt();
    syncer2->sync();
    waitForIt();

    validateSync(QStringList{ "task1" }, storage1);
    validateSync(QStringList{ "task1" }, storage2);



    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
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
