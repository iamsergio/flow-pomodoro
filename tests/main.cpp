#include "teststorage.h"
#include "testtask.h"
#include "testtag.h"
#include "testtagmodel.h"
#include "testcheckabletagmodel.h"

#include <QtTest/QtTest>
#include <QGuiApplication>

int main(int argc, char *argv[])
{ 
    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    bool success = true;

    TestStorage test1;
    success &= QTest::qExec(&test1, argc, argv) == 0;

    TestTask test2;
    success &= QTest::qExec(&test2, argc, argv) == 0;

    TestTag test3;
    success &= QTest::qExec(&test3, argc, argv) == 0;

    TestTagModel test4;
    success &= QTest::qExec(&test4, argc, argv) == 0;

    TestCheckableTagModel test5;
    success &= QTest::qExec(&test5, argc, argv) == 0;

    delete Kernel::instance(); // Otherwise it's deleted by QGuiApplication and we hang deep in qml engine land for some reason

    if (!success)
        qWarning() << "\nSome tests failed!!!";

    return success;
}
