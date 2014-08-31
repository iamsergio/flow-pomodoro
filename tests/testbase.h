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
#include "storage.h"
#include "kernel.h"
#include "runtimeconfiguration.h"
#include "tag.h"
#include "task.h"
#include "controller.h"
#include "tagref.h"
#include <QtTest/QtTest>

class TestBase : public QObject
{
public:

    TestBase()
    {
        m_kernel = Kernel::instance();
        m_storage = m_kernel->storage();
        m_controller = m_kernel->controller();
        RuntimeConfiguration config;
        config.setDataFileName("data.dat");
        config.setPluginsSupported(false);
        m_kernel->setRuntimeConfiguration(config);
    }

    ~TestBase()
    {
    }

    bool checkStorageConsistency()
    {
        const Tag::List tags = m_storage->tags();
        foreach (const Task::Ptr &task, m_storage->tasks()) {
            foreach (const TagRef &tagref, task->tags()) {
                if (!tags.contains(tagref.m_tag)) {
                    qWarning() << "Found unknown tag";
                    return false;
                }

                if (!tagref.m_task && tagref.m_tag) {
                    qWarning() << "Null task or tag in TagRef";
                    return false;
                }
            }
        }

        return true;
    }

protected:
    Kernel *m_kernel;
    Storage *m_storage;
    Controller *m_controller;
};

#endif
