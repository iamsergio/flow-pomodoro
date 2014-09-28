#include "teststorage.h"
#include "testtask.h"
#include "testtag.h"
#include "testtagmodel.h"
#include "testcheckabletagmodel.h"
#include "testwebdav.h"
#include "testtaskfiltermodel.h"
#include "teststagedtasksmodel.h"
#include "testarchivedtasksmodel.h"

#include <QtTest/QtTest>
#include <QGuiApplication>

int main(int argc, char *argv[])
{ 
    QCoreApplication app(argc, argv);
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

    TestArchivedTasksModel test6;
    success &= QTest::qExec(&test6, argc, argv) == 0;

    TestStagedTasksModel test7;
    success &= QTest::qExec(&test7, argc, argv) == 0;

    TestTaskFilterModel test8;
    success &= QTest::qExec(&test8, argc, argv) == 0;

    TestWebDav test9;
    success &= QTest::qExec(&test9, argc, argv) == 0;

    if (success)
        qDebug() << "Success!";
    else
        qWarning() << "\nSome tests failed!!!";

    return success ? 0 : -1;
}
