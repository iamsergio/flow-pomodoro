#include "testbase.h"
#include <QtTest/QtTest>

class TestTask: public TestBase
{
    Q_OBJECT
public:
    TestTask();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testSetTags();
    void testAddAndRemoveTag();
private:
    Task::Ptr m_task1;
    Task::Ptr m_task2;
    SignalSpy m_spy;
};
