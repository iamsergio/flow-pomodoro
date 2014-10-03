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

#ifndef FLOW_TEST_BASE_H
#define FLOW_TEST_BASE_H

#include "signalspy.h"
#include "modelsignalspy.h"
#include "storage.h"
#include "kernel.h"
#include "runtimeconfiguration.h"
#include "tag.h"
#include "task.h"
#include "webdavsyncer.h"
#include "controller.h"
#include "tagref.h"
#include "settings.h"
#include <QtTest/QtTest>

class TestBase : public QObject
{
public:

    TestBase()
    {
        RuntimeConfiguration config;
        config.setDataFileName("data.dat");
        config.setPluginsSupported(false);
        config.setSettings(new Settings("unit-test-settings.ini"));
        config.setSaveEnabled(false);
        config.setWebDAVFileName("unit-test-flow.dat");
        m_kernel = new Kernel(config, this);
        m_storage = m_kernel->storage();
        m_controller = m_kernel->controller();

        if (Storage::storageCount != 1) {
            qWarning() << "Storage count is " << Storage::storageCount;
            Q_ASSERT(false);
        }
    }

    ~TestBase()
    {
        QFile::remove("unit-test-settings.ini");
    }

    bool checkStorageConsistency(int expectedTagCount = -1)
    {
        const Tag::List tags = m_storage->tags();
        expectedTagCount = expectedTagCount == -1 ? tags.count() : expectedTagCount;

        if (expectedTagCount != Tag::tagCount) {
            qWarning() << "Actual tag count is" << Tag::tagCount
                       << "; expected:" << expectedTagCount
                       << "; storageCount:" << Storage::storageCount;
            return false;
        }

        foreach (const Tag::Ptr &tag, m_storage->tags())
            if (!tag->kernel()) {
                qWarning() << "Invalid storage for tag " << tag->name();
                return false;
            }

        foreach (const Task::Ptr &task, m_storage->tasks()) {
            if (!task->kernel()) {
                qWarning() << "Invalid storage for task " << task->summary();
                return false;
            }
            foreach (const TagRef &tagref, task->tags()) {
                Tag::Ptr tag = m_storage->tag(tagref.tagName(), /*create=*/ false);
                if (!tags.contains(tag)) {
                    qWarning() << "Found unknown tag";
                    return false;
                }

                if (!tagref.storage()) {
                    qWarning() << "Tagref not associated with any storage";
                    return false;
                }

                if (!tagref.m_task && tag) {
                    qWarning() << "Null task or tag in TagRef";
                    return false;
                }
            }
        }

        return true;
    }

    void createNewKernel(const QString &dataFilename, bool load = true)
    {
        delete m_kernel;
        RuntimeConfiguration config;
        config.setDataFileName("data_files/" + dataFilename);
        config.setPluginsSupported(false);
        config.setSettings(new Settings("unit-test-settings.ini"));
        config.setSaveEnabled(false);
        config.setWebDAVFileName("unit-test-flow.dat");

        m_kernel = new Kernel(config);
        m_storage = m_kernel->storage();
        m_controller = m_kernel->controller();
        if (load)
            m_kernel->storage()->load();
    }

    void waitForIt()
    {
        QTestEventLoop::instance().enterLoop(10);
        QVERIFY(!QTestEventLoop::instance().timeout());
    }

    void stopWaiting()
    {
        QTestEventLoop::instance().exitLoop();
    }

protected:
    Kernel *m_kernel;
    Storage *m_storage;
    Controller *m_controller;
};

#endif
