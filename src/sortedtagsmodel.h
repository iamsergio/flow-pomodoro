#ifndef SORTEDTAGSMODEL_H
#define SORTEDTAGSMODEL_H

#include <QSortFilterProxyModel>

class SortedTagsModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortedTagsModel(QObject *parent = 0);

signals:

public slots:

};

#endif // SORTEDTAGSMODEL_H
