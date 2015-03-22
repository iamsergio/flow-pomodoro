#include "testduedate.h"
#include "duedate.h"

#include <QDate>

TestDueDate::TestDueDate()
{

}

void TestDueDate::testCTOR()
{
    DueDate date;
    QVERIFY(!date.isValid());
    QVERIFY(!date.date().isValid());
    QCOMPARE(date.frequency(), (uint)1);
    QCOMPARE(date.periodType(), DueDate::PeriodTypeNone);
    QVERIFY(!date.recurs());

    const QDate today = QDate::currentDate();
    DueDate date2(today, DueDate::PeriodTypeYearly, 2);
    QCOMPARE(date2.frequency(), (uint)2);
    QCOMPARE(date2.periodType(), DueDate::PeriodTypeYearly);
    QVERIFY(date2.recurs());
}

void TestDueDate::testSetPeriodType()
{
    DueDate date(QDate::currentDate(), DueDate::PeriodTypeNone, 1);
    QVERIFY(date.isValid());
    date.setPeriodType(DueDate::PeriodTypeCount);
    QCOMPARE(date.periodType(), DueDate::PeriodTypeNone);
    QVERIFY(!date.recurs());

    date.setPeriodType(DueDate::PeriodTypeWeekly);
    QCOMPARE(date.periodType(), DueDate::PeriodTypeWeekly);
    QVERIFY(date.recurs());

    date.setPeriodType(DueDate::PeriodTypeDaily);
    QCOMPARE(date.periodType(), DueDate::PeriodTypeDaily);
    QVERIFY(date.recurs());

    date.setPeriodType(DueDate::PeriodTypeMonthly);
    QCOMPARE(date.periodType(), DueDate::PeriodTypeMonthly);
    QVERIFY(date.recurs());
}

void TestDueDate::testSetFrequency()
{
    DueDate date(QDate::currentDate(), DueDate::PeriodTypeNone, 2);
    QVERIFY(!date.recurs());
    QVERIFY(date.isValid());
    QCOMPARE(date.frequency(), (uint)2);
    date.setFrequency(0);
    QCOMPARE(date.frequency(), (uint)1);
    date.setFrequency(1000);
    QCOMPARE(date.frequency(), (uint)1000);
}
