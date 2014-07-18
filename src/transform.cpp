#include "transform.h"

using namespace FunctionalModels;

Transform::Transform(QAbstractItemModel *sourceModel, DataFunc dataFunc,
                     const QHash<int, QByteArray> &roleNames, QObject *parent)
    : QIdentityProxyModel(parent)
    , m_dataFunc(dataFunc)
    , m_roleNames(roleNames)
{
    setSourceModel(sourceModel);
}

QVariant Transform::data(const QModelIndex &proxyIndex, int role) const
{
    if (!sourceModel())
        return QVariant();

    QModelIndex index = mapToSource(proxyIndex);
    if (!index.isValid())
        return QVariant();

    return m_dataFunc(index, role);
}

QHash<int, QByteArray> Transform::roleNames() const
{
    return m_roleNames.isEmpty() ? QIdentityProxyModel::roleNames() : m_roleNames;
}
