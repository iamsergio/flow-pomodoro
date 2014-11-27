#include "teststorage.h"
#include "testtask.h"
#include "testtag.h"
#include "testtagmodel.h"
#include "testcheckabletagmodel.h"
#include "testtaskfiltermodel.h"
#include "teststagedtasksmodel.h"
#include "testarchivedtasksmodel.h"

#ifndef NO_WEBDAV
# include "testwebdav.h"
#endif

#include <QtTest/QtTest>
#include <QGuiApplication>

int main(int argc, char *argv[])
{ 
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    bool success = true;

    {
        TestStorage test1;
        success &= QTest::qExec(&test1, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestTask test2;
        success &= QTest::qExec(&test2, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestTag test3;
        success &= QTest::qExec(&test3, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestTagModel test4;
        success &= QTest::qExec(&test4, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestCheckableTagModel test5;
        success &= QTest::qExec(&test5, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestArchivedTasksModel test6;
        success &= QTest::qExec(&test6, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestStagedTasksModel test7;
        success &= QTest::qExec(&test7, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestTaskFilterModel test8;
        success &= QTest::qExec(&test8, argc, argv) == 0;
        Q_ASSERT(success);
    }

#ifndef NO_WEBDAV
    {
        TestWebDav test9;
        success &= QTest::qExec(&test9, argc, argv) == 0;
        Q_ASSERT(success);
    }
#endif

    if (success)
        qDebug() << "Success!";
    else
        qWarning() << "\nSome tests failed!!!";

    return success ? 0 : -1;
}
