#include "taskfilterproxymodel.h"
#include "taskstorage.h"

#include <QDebug>

TaskFilterProxyModel::TaskFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &TaskFilterProxyModel::rowsInserted,
            this, &TaskFilterProxyModel::countChanged);
    connect(this, &TaskFilterProxyModel::rowsRemoved,
            this, &TaskFilterProxyModel::countChanged);
    connect(this, &TaskFilterProxyModel::modelReset,
            this, &TaskFilterProxyModel::countChanged);
    connect(this, &TaskFilterProxyModel::layoutChanged,
            this, &TaskFilterProxyModel::countChanged);
}

int TaskFilterProxyModel::count() const
{
    return rowCount();
}

bool TaskFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    if (source_parent.isValid() || source_row < 0 || source_row >= sourceModel()->rowCount()) {
        qWarning() << source_parent.isValid() << source_row << sourceModel()->rowCount();
        Q_ASSERT(false);
        return false;
    }

    QModelIndex index = sourceModel()->index(source_row, 0);
    if (!index.isValid())
        return false;

    QVariant v = index.data(TaskStorage::TaskPtrRole);
    if (!v.isValid())
        return false;

    if (m_tagText.isEmpty())
        return true;

    Task::Ptr task = v.value<Task::Ptr>();
    if (!task)
        return false;

    TagRef::List tags = task->tags();
    return std::find_if(tags.cbegin(), tags.cend(),
                        [&](const TagRef &tag) { return tag.m_tag->name() == m_tagText; }) != tags.cend();
}

void TaskFilterProxyModel::setTagName(const QString &tagText)
{
    if (m_tagText != tagText) {
        m_tagText = tagText;
        invalidateFilter();
    }
}
