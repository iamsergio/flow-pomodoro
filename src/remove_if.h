#ifndef REMOVE_IF_H
#define REMOVE_IF_H

#include <QSortFilterProxyModel>
#include <functional>

namespace FunctionalModels {

using FilterFunc = std::function<bool(const QVariant &)>; // Uses filterRole
using FilterFunc2 = std::function<bool(const QModelIndex &)>; // Supports any role. // TODO rename it.

class Remove_if : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    Remove_if(QAbstractItemModel *sourceModel, FilterFunc, int role = Qt::DisplayRole);
    Remove_if(QAbstractItemModel *sourceModel, FilterFunc2);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
    void invalidateFilter();
    int count() const;

signals:
    void countChanged();

private:
    void init();
    FilterFunc m_filterFunc;
    FilterFunc2 m_filterFunc2;
};

}

#endif
