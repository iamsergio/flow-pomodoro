#ifndef REALLY_GENERIC_LIST_MODEL_H
#define REALLY_GENERIC_LIST_MODEL_H

#include <QList>
#include <QDebug>
#include <QGuiApplication>
#include <QAbstractListModel>

#include <functional>
#include <QtCore/qcompilerdetection.h>
#ifdef Q_COMPILER_INITIALIZER_LISTS
#include <initializer_list>
#endif

#define ADD_ROLE3(role, list, method) \
    list.insertRole(role, [&](int i) { return list.at(i).method; })

#define ADD_ROLE4(roleNumber, roleName, list, method) \
    list.insertRole(roleName, [&](int i) { return list.at(i).method; }, roleNumber)

#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define ADD_ROLE(...) GET_MACRO(__VA_ARGS__, ADD_ROLE4, ADD_ROLE3)(__VA_ARGS__)

template <typename T>
class _InternalModel;

template <typename T>
class GenericListModel;

typedef QVariant (*GetterFunc)(int);

template <typename T>
class GenericListModel : public QList<T>
{
public:
    GenericListModel();
    GenericListModel(const GenericListModel &other);
#ifdef Q_COMPILER_INITIALIZER_LISTS
    GenericListModel(std::initializer_list<T> args);
#endif
    ~GenericListModel();

    operator QAbstractListModel*() const { return m_model; }
    void insertRole(const QByteArray &name, GetterFunc getter, int role = -1);
    void setDataFunction(QVariant (*)(const GenericListModel<T> &list, int role, int index));

    // Reimplemented from QList
    void append(const T&);
    void append(const QList<T> &);
    int	removeAll(const T &);
    void removeAt(int i);
    void insert(int i, const T &);
    void pop_back();
    void pop_front();
    void prepend(const T &);
    void push_back(const T &);
    void push_front(const T &);
    void removeFirst();
    void removeLast();
    bool removeOne(const T &);
    void replace(int i, const T &);
    T takeAt(int i);
    T takeFirst();
    T takeLast();
    void swap(QList<T> &other);
    void swap(int i, int j);
    void move(int from, int to);
    void clear();
    GenericListModel<T> &operator=(const GenericListModel<T> &);
    GenericListModel<T> & operator+=(const T &);
    GenericListModel<T> & operator<<(const T &);
    GenericListModel<T> & operator<<(const QList<T> &);
    GenericListModel<T> & operator+=(const QList<T> &other);

private:
    // deleted from QList<T>, because they return T& and we can't monitor changes on that.
    T & operator[](int i);
    typename QList<T>::iterator erase(typename QList<T>::iterator);
    typename QList<T>::iterator erase(typename QList<T>::iterator,
                                      typename QList<T>::iterator);
    typename QList<T>::iterator insert(typename QList<T>::iterator, const T&);
    T& first();
    T& front();
    T& last();
    T& back();

    // Not deleted because foreach needs them.
    //typename QList<T>::iterator begin();
    //typename QList<T>::iterator end();

    _InternalModel<T> *const m_model;
};

//------------------------------------------------------------------------------
// Implementation detail.

class _InternalModelBase : public QAbstractListModel
{
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_OBJECT
public:
    explicit _InternalModelBase(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int count() const
    {
        return rowCount();
    }

Q_SIGNALS:
    void countChanged();

};

template <typename T>
class _InternalModel : public _InternalModelBase {
public:
    _InternalModel(GenericListModel<T> &list)
        : _InternalModelBase(qApp) // So it has cpp ownership
        , m_list(list)
        , m_dataFunction(nullptr)
    {
        connect(this, &_InternalModel::rowsInserted,
                this, &_InternalModel::countChanged);
        connect(this, &_InternalModel::rowsRemoved,
                this, &_InternalModel::countChanged);
        connect(this, &_InternalModel::modelReset,
                this, &_InternalModel::countChanged);
        connect(this, &_InternalModel::layoutChanged,
                this, &_InternalModel::countChanged);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : m_list.count();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return m_roles;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid())
            return QVariant();

        QHash<int, QByteArray>::const_iterator it;
        for (it = m_roles.cbegin(); it != m_roles.cend(); ++it) {
            if (it.key() == role) {
                GetterFunc func = m_getters.value(it.key());
                return func ? func(index.row())
                            : m_dataFunction ? m_dataFunction(m_list, index.row(), role)
                                             : QVariant();

            }
        }

        return QVariant();
    }

    void insertRole(const QByteArray &name, GetterFunc getter, int role)
    {
        m_roles.insert(role, name);
        m_getters.insert(role, getter);
    }

    void setDataFunction(QVariant (*function)(const GenericListModel<T> &, int index, int role))
    {
        m_dataFunction = function;
    }

private:
    QHash<int, QByteArray> m_roles;
    QHash<int, GetterFunc> m_getters;
    GenericListModel<T> &m_list;
    QVariant (*m_dataFunction)(const GenericListModel<T> &, int index, int role);

    template <class U>
    friend class ::GenericListModel;
};

template <typename T>
GenericListModel<T>::GenericListModel()
    : QList<T>()
    , m_model(new _InternalModel<T>(*this))
{}

template <typename T>
GenericListModel<T>::GenericListModel(const GenericListModel &other)
    : QList<T>(other)
    , m_model(new _InternalModel<T>(*this))
{}

#ifdef Q_COMPILER_INITIALIZER_LISTS
template <typename T>
GenericListModel<T>::GenericListModel(std::initializer_list<T> args)
    : QList<T>(args)
    , m_model(new _InternalModel<T>(*this))
{}
#endif

template <typename T>
GenericListModel<T>::~GenericListModel()
{
    delete m_model;
}

template <typename T>
void GenericListModel<T>::append(const T &t)
{
    int count = this->count();
    m_model->beginInsertRows(QModelIndex(), count, count);
    QList<T>::append(t);
    m_model->endInsertRows();
}

template <typename T>
void GenericListModel<T>::append(const QList<T> &list)
{
    int count = this->count();
    m_model->beginInsertRows(QModelIndex(), count, count + list.count());
    QList<T>::append(list);
    m_model->endInsertRows();
}

template <typename T>
int GenericListModel<T>::removeAll(const T &value)
{
    int numRemoved = 0;
    int occurrences = this->count(value);
    if (occurrences == 1) {
        numRemoved = 1;
        removeAt(this->indexOf(value));
    } else if (occurrences > 1) {
        m_model->beginResetModel();
        numRemoved = QList<T>::removeAll(value);
        m_model->endResetModel();
    }

    return numRemoved;
}

template <typename T>
void GenericListModel<T>::removeAt(int i)
{
    if (i < 0 || i >= this->count()) {
        qWarning() << Q_FUNC_INFO << "Invalid index" << i << this->count();
        Q_ASSERT(false);
        return;
    }

    m_model->beginRemoveRows(QModelIndex(), i, i);
    QList<T>::removeAt(i);
    m_model->endRemoveRows();
}

template <typename T>
void GenericListModel<T>::insert(int i, const T &value)
{
    if (i < 0 || i > this->count()) {
        qWarning() << Q_FUNC_INFO << "Invalid index" << i << this->count();
        Q_ASSERT(false);
        return;
    }

    m_model->beginInsertRows(QModelIndex(), i, i);
    QList<T>::insert(i, value);
    m_model->endInsertRows();
}

template <typename T>
void GenericListModel<T>::insertRole(const QByteArray &name, GetterFunc getter, int role)
{
    if (role == -1)
        role = Qt::UserRole + m_model->roleNames().count() + 1;
    Q_ASSERT(!m_model->roleNames().contains(role));
    m_model->insertRole(name, getter, role);
}

template <typename T>
void GenericListModel<T>::setDataFunction(QVariant (*function)(const GenericListModel<T> &list, int, int))
{
    m_model->setDataFunction(function);
}

template <typename T>
GenericListModel<T> &GenericListModel<T>::operator=(const GenericListModel<T> &other)
{
    m_model->beginResetModel();
    QList<T>::operator=(other);
    m_model->endResetModel();

    return *this;
}

template <typename T>
GenericListModel<T> & GenericListModel<T>::operator<<(const T &value)
{
    append(value);
    return *this;
}

template <typename T>
GenericListModel<T> & GenericListModel<T>::operator<<(const QList<T> &value)
{
    append(value);
    return *this;
}

template <typename T>
GenericListModel<T> & GenericListModel<T>::operator+=(const T &value)
{
    append(value);
    return *this;
}

template <typename T>
GenericListModel<T> & GenericListModel<T>::operator+=(const QList<T> &other)
{
    append(other);
    return *this;
}

template <typename T>
void GenericListModel<T>::clear()
{
    m_model->beginResetModel();
    QList<T>::clear();
    m_model->endResetModel();
}

template <typename T>
void GenericListModel<T>::pop_back()
{
    if (!QList<T>::isEmpty())
        removeAt(QList<T>::count()-1);
}

template <typename T>
void GenericListModel<T>::pop_front()
{
    if (!QList<T>::isEmpty())
        removeAt(0);
}

template <typename T>
void GenericListModel<T>::prepend(const T &value)
{
    insert(0, value);
}

template <typename T>
void GenericListModel<T>::push_back(const T &value)
{
    append(value);
}

template <typename T>
void GenericListModel<T>::push_front(const T &value)
{
    prepend(value);
}

template <typename T>
void GenericListModel<T>::removeFirst()
{
    pop_front();
}

template <typename T>
void GenericListModel<T>::removeLast()
{
    pop_back();
}

template <typename T>
T GenericListModel<T>::takeAt(int i)
{
    Q_ASSERT(i >= 0 && i < QList<T>::count());
    m_model->beginRemoveRows(QModelIndex(), i, i);
    T t = QList<T>::takeAt(i);
    m_model->endRemoveRows();
    return t;
}

template <typename T>
T GenericListModel<T>::takeFirst()
{
    Q_ASSERT(!QList<T>::isEmpty());
    m_model->beginRemoveRows(QModelIndex(), 0, 0);
    m_model->endRemoveColumns();
    T t = QList<T>::takeFirst();
    return t;
}

template <typename T>
T GenericListModel<T>::takeLast()
{
    Q_ASSERT(!QList<T>::isEmpty());
    int index = QList<T>::count() - 1;
    m_model->beginRemoveRows(QModelIndex(), index, index);
    T t = QList<T>::takeLast();
    m_model->endRemoveColumns();
    return t;
}

template <typename T>
bool GenericListModel<T>::removeOne(const T &value)
{
    const int index = this->indexOf(value);
    if (index == -1) {
        return false;
    } else {
        removeAt(index);
        return true;
    }
}

template <typename T>
void GenericListModel<T>::replace(int i, const T &value)
{
    Q_ASSERT(i >= 0 && i < QList<T>::count());
    QList<T>::replace(i, value);
    m_model->dataChanged(m_model->index(i, 0), m_model->index(i, 0));
}

template <typename T>
void GenericListModel<T>::swap(QList<T> &other)
{
    m_model->beginResetModel();
    QList<T>::swap(other);
    m_model->endResetModel();
}

template <typename T>
void GenericListModel<T>::swap(int i, int j)
{
    QList<T>::swap(i, j);
    m_model->dataChanged(m_model->index(i, 0), m_model->index(i, 0));
    m_model->dataChanged(m_model->index(j, 0), m_model->index(j, 0));
}

template <typename T>
void GenericListModel<T>::move(int from, int to)
{
    insert(to, takeAt(from));
}

#endif
