#include "testbase.h"
#include <QtTest/QtTest>

class Tests: public TestBase
{
    Q_OBJECT
public:
    Tests();

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
};
