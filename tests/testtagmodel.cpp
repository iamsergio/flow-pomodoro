/*
  This file is part of Flow.

  Copyright (C) 2014 Sérgio Martins <iamsergio@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "testtagmodel.h"

TestTagModel::TestTagModel()
    : TestBase()
{
}

void TestTagModel::initTestCase()
{
    m_storage->clearTags();
    m_storage->clearTasks();
}

void TestTagModel::cleanupTestCase()
{

}

void TestTagModel::testFromFile_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<int>("expectedNumTags");
    QTest::addColumn<QStringList>("expectedTags");
    QTest::addColumn<QStringList>("expectedSignals");
    QTest::addColumn<QStringList>("expectedSignalsInUnsortedModel");

    int expectedNumTags = 13;
    QStringList tags;
    tags << "ABC" << "bender" << "Blog" << "casa" << "compras" << "contas" << "Flow"
         << "kdab" << "kde" << "music" << "PPP" << "Qt" << "Tomar";

    QStringList expectedSignals;
    QStringList expectedSignalsInUnsortedModel;
    for (int i = 0; i < expectedNumTags; ++i) {
        expectedSignals << "rowsAboutToBeInserted" << "rowsInserted";
        expectedSignalsInUnsortedModel << "rowsAboutToBeInserted" << "rowsInserted";
    }

    expectedSignals << "modelAboutToBeReset" << "modelReset" // Because of final assignment when loading
                    << "layoutAboutToBeChanged" << "layoutChanged";

    expectedSignalsInUnsortedModel << "modelAboutToBeReset" << "modelReset";

    QTest::newRow("some tags") << "tagmodeltest1.dat" << expectedNumTags
                               << tags << expectedSignals << expectedSignalsInUnsortedModel;
}

void TestTagModel::testFromFile()
{
    QFETCH(QString, filename);
    QFETCH(int, expectedNumTags);
    QFETCH(QStringList, expectedTags);
    QFETCH(QStringList, expectedSignals);
    QFETCH(QStringList, expectedSignalsInUnsortedModel);


    delete Kernel::instance();
    RuntimeConfiguration config;
    config.setDataFileName("data_files/" + filename);
    config.setPluginsSupported(false);

    Kernel *kernel = Kernel::instance();
    kernel->setRuntimeConfiguration(config);
    Storage *storage = kernel->storage();
    ModelSignalSpy *tagsModelSpy = new ModelSignalSpy(storage->tagsModel());
    const TagList &unsortedTags = storage->tags();
    QAbstractListModel *unsortedTagsModel = unsortedTags;
    ModelSignalSpy *unsortedTagsModelSpy = new ModelSignalSpy(unsortedTagsModel);
    QVERIFY(storage->tags().isEmpty());
    QCOMPARE(unsortedTagsModel->rowCount(), 0);
    storage->load();

    if (storage->tags().count() != expectedNumTags) {
        qDebug() << "Got" << storage->tags().count()
                 << "; Expected" << expectedNumTags;

        foreach (const Tag::Ptr &tag, storage->tags()) {
            qDebug() << "Got" << tag->name();
        }

        QVERIFY(false);
    }

    QAbstractItemModel *model = storage->tagsModel();
    QCOMPARE(model->rowCount(), expectedNumTags);
    QCOMPARE(expectedNumTags, expectedTags.count());

    for (int i = 0; i < expectedNumTags; i++) {
        QModelIndex index = model->index(i, 0);
        QVariant variant = model->data(index, Storage::TagRole);
        Tag *tag = variant.value<Tag*>();
        QVERIFY(tag);
        QCOMPARE(tag->name(), expectedTags.at(i));
        // qDebug() << "tag " << tag->name() << tag->taskCount();
    }

    //spy->dumpDebugInfo();
    QCOMPARE(tagsModelSpy->count(), expectedSignals.count());

    for (int i = 0; i < expectedSignals.count(); ++i) {
        QCOMPARE(tagsModelSpy->caughtSignals().at(i).name, expectedSignals.at(i));
    }

    for (int i = 0; i < expectedSignalsInUnsortedModel.count(); ++i) {
        CaughtSignal signal = unsortedTagsModelSpy->caughtSignals().at(i);
        QCOMPARE(signal.name, expectedSignalsInUnsortedModel.at(i));
        if (signal.name == "rowsAboutToBeInserted") {
            QCOMPARE(signal.args.at(1), signal.args.at(2));
        }
    }

    tagsModelSpy->deleteLater();
    unsortedTagsModelSpy->deleteLater();
}


