#include "remove_if.h"

using namespace FunctionalModels;

Remove_if::Remove_if(QAbstractItemModel *source, FilterFunc func, int role)
    : QSortFilterProxyModel(source)
    , m_filterFunc(func)
{
    setFilterRole(role);
    setSourceModel(source);
    init();
}

Remove_if::Remove_if(QAbstractItemModel *source, FilterFunc2 func)
    : QSortFilterProxyModel(source)
    , m_filterFunc2(func)
{
    setSourceModel(source);
    init();
}

bool Remove_if::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (!index.isValid())
        return false;

    if (m_filterFunc2)
        return m_filterFunc2(index);

    QVariant variant = index.data(filterRole());
    if (!variant.isValid())
        return false;

    return m_filterFunc(variant);
}

void Remove_if::invalidateFilter()
{
    QSortFilterProxyModel::invalidateFilter();
}

int Remove_if::count() const
{
    return rowCount();
}

void Remove_if::init()
{
    connect(this, &Remove_if::rowsInserted,
            this, &Remove_if::countChanged);
    connect(this, &Remove_if::rowsRemoved,
            this, &Remove_if::countChanged);
    connect(this, &Remove_if::modelReset,
            this, &Remove_if::countChanged);
    connect(this, &Remove_if::layoutChanged,
            this, &Remove_if::countChanged);
}
