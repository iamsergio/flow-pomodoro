#ifndef SORTMODEL_H
#define SORTMODEL_H

#include <QSortFilterProxyModel>
#include <functional>

namespace FunctionalModels {

using SortFunc = std::function<bool(const QModelIndex &left, const QModelIndex &right)>;
using SortFunc2 = std::function<bool(const QVariant &left, const QVariant &right)>;

class SortModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit SortModel(QAbstractItemModel *sourceModel, SortFunc);
    explicit SortModel(QAbstractItemModel *sourceModel, SortFunc2, int role);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    int count() const;

signals:
    void countChanged();

private:
    void init();
    SortFunc m_sortFunc;
    SortFunc2 m_sortFunc2;
};

}

#endif
