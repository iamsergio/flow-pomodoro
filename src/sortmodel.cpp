#include "sortmodel.h"

using namespace FunctionalModels;

SortModel::SortModel(QAbstractItemModel *sourceModel,
                     FunctionalModels::SortFunc sortFunc)
    : QSortFilterProxyModel(sourceModel)
    , m_sortFunc(sortFunc)
{
    setSourceModel(sourceModel);
    init();
}

SortModel::SortModel(QAbstractItemModel *sourceModel,
                     SortFunc2 sortFunc, int role)
    : QSortFilterProxyModel(sourceModel)
    , m_sortFunc2(sortFunc)
{
    setSourceModel(sourceModel);
    setSortRole(role);
    init();
}

bool SortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (!sourceModel())
        return false;

    if (!left.isValid() || !right.isValid())
        return false;

    if (m_sortFunc)
        return m_sortFunc(left, right);

    return m_sortFunc2(left.data(sortRole()), right.data(sortRole()));
}

int SortModel::count() const
{
    return rowCount();
}


void SortModel::init()
{
    sort(Qt::AscendingOrder);

    connect(this, &SortModel::rowsInserted,
            this, &SortModel::countChanged);
    connect(this, &SortModel::rowsRemoved,
            this, &SortModel::countChanged);
    connect(this, &SortModel::modelReset,
            this, &SortModel::countChanged);
    connect(this, &SortModel::layoutChanged,
            this, &SortModel::countChanged);
}
