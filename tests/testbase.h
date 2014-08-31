#ifndef FLOW_TEST_BASE_H
#define FLOW_TEST_BASE_H

#include "signalspy.h"
#include "storage.h"
#include "kernel.h"
#include "runtimeconfiguration.h"
#include "tag.h"
#include "task.h"
#include "tagref.h"
#include <QtTest/QtTest>

class TestBase : public QObject
{
public:

    TestBase()
    {
        m_kernel = Kernel::instance();
        m_storage = m_kernel->storage();
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
};

#endif
