#include "signalspy.h"
#include <QtTest/QtTest>

class Kernel;
class Storage;

class Tests: public QObject
{
    Q_OBJECT

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
    Kernel *m_kernel;
    Storage *m_storage;

    SignalSpy m_storageSpy;
};
