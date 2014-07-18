#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QIdentityProxyModel>
#include <functional>

namespace FunctionalModels {

using DataFunc = std::function<QVariant(const QModelIndex &sourceIndex, int role)>;

class Transform : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit Transform(QAbstractItemModel *sourceModel,
                       DataFunc, const QHash<int, QByteArray> &roleNames = {}, QObject *parent = 0);
    QVariant data(const QModelIndex &proxyIndex, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:
    DataFunc m_dataFunc;
    const QHash<int, QByteArray> m_roleNames;
};

}

#endif
