#include "testbase.h"
#include <QtTest/QtTest>

class TestTag: public TestBase
{
    Q_OBJECT
public:
    TestTag();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testSetName();
    void testTaskCount();

private:
    Tag::Ptr m_tagA;
    Tag::Ptr m_tagB;
    Tag::Ptr m_tagC;
    SignalSpy m_spy;
};
