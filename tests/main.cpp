#include "teststorage.h"
#include <QtTest/QtTest>
#include <QGuiApplication>

int main(int argc, char *argv[])
{ 
    QGuiApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    Tests test1;
    return QTest::qExec(&test1, argc, argv);
}
