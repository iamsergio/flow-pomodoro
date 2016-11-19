#include "teststorage.h"
#include "testtask.h"
#include "testtag.h"
#include "testtagmodel.h"
#include "testduedate.h"
#include "testcheckabletagmodel.h"
#include "testtaskfiltermodel.h"
#include "teststagedtasksmodel.h"
#include "testarchivedtasksmodel.h"
#include "testgitutils.h"
#include "quick/testui.h"

#ifndef NO_WEBDAV
# include "testwebdav.h"
#endif

#include <QtTest/QtTest>
#include <QApplication>

int main(int argc, char *argv[])
{ 
    QApplication app(argc, argv);
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

    {
        TestDueDate test10;
        success &= QTest::qExec(&test10, argc, argv) == 0;
        Q_ASSERT(success);
    }


    {
        TestGitUtils test11;
        success &= QTest::qExec(&test11, argc, argv) == 0;
        Q_ASSERT(success);
    }

    {
        TestUI uiTest;
        success &= QTest::qExec(&uiTest, argc, argv) == 0;
        Q_ASSERT(success);
    }

    if (success)
        qDebug() << "Success!";
    else
        qWarning() << "\nSome tests failed!!!";

    return success ? 0 : -1;
}
