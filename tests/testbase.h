#ifndef FLOW_TEST_BASE_H
#define FLOW_TEST_BASE_H

#include "signalspy.h"
#include "storage.h"
#include "kernel.h"
#include "runtimeconfiguration.h"
#include "tag.h"
#include "task.h"
#include <QtTest/QtTest>

class TestBase : public QObject
{
public:

    TestBase()
    {
        qDebug() << "inited";
        m_kernel = Kernel::instance();
        m_storage = m_kernel->storage();
        RuntimeConfiguration config;
        config.setDataFileName("data.dat");
        m_kernel->setRuntimeConfiguration(config);
    }

    ~TestBase()
    {
        delete m_kernel;
        m_kernel = 0;
    }

protected:
    Kernel *m_kernel;
    Storage *m_storage;
    SignalSpy m_storageSpy;
};

#endif
