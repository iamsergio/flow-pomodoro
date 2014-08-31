#include "testbase.h"
#include <QtTest/QtTest>

class TestStorage: public TestBase
{
    Q_OBJECT
public:
    TestStorage();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCreateTag();
    void testDeleteTag();
    void testContainsTag();
    void testTag();
    void testIndexOf();
    void testRenameTag();

    void testAddTask();
    void testDeleteTask();
    void testPrependTask(); // indexOfItem, taskAt

private:
    SignalSpy m_storageSpy;
};
