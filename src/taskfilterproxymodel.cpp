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

#include "taskfilterproxymodel.h"
#include "storage.h"

#include <QDebug>

TaskFilterProxyModel::TaskFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)

{
    connect(this, &TaskFilterProxyModel::rowsInserted,
            this, &TaskFilterProxyModel::onSourceCountChanged);
    connect(this, &TaskFilterProxyModel::rowsRemoved,
            this, &TaskFilterProxyModel::onSourceCountChanged);
    connect(this, &TaskFilterProxyModel::modelReset,
            this, &TaskFilterProxyModel::onSourceCountChanged);
    connect(this, &TaskFilterProxyModel::layoutChanged,
            this, &TaskFilterProxyModel::onSourceCountChanged);

    sort(0);
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

    QVariant v = index.data(Storage::TaskPtrRole);
    if (!v.isValid())
        return false;

    Task::Ptr task = v.value<Task::Ptr>();
    if (!task)
        return false;

    if (task->status() == TaskStarted)
        return false;

    if (m_filterArchived && task->staged())
        return false;

    if (m_filterStaged && !task->staged())
        return false;

    if (m_filterUntagged)
        return task->tags().isEmpty();

    if (m_filterDueDated)
        return task->dueDate().isValid();

    if (m_tagText.isEmpty())
        return true;

    TagRef::List tags = task->tags();
    for (int i = 0; i < tags.count(); ++i) {
        const TagRef &tagref = tags.at(i);
        if (tagref.tagName() == m_tagText)
            return true;
    }

    return false;
}

bool TaskFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    Task::Ptr leftTask = left.data(Storage::TaskPtrRole).value<Task::Ptr>();
    Task::Ptr rightTask = right.data(Storage::TaskPtrRole).value<Task::Ptr>();
    if (m_filterDueDated) {
        if (leftTask->dueDate() == rightTask->dueDate()) {
            return defaultLessThan(leftTask, rightTask);
        } else {
            return leftTask->dueDate() < rightTask->dueDate();
        }
    } else {
        return defaultLessThan(leftTask, rightTask);
    }
}

void TaskFilterProxyModel::setTagName(const QString &tagText)
{
    if (m_tagText != tagText) {
        m_tagText = tagText;
        invalidateFilter();
    }
}

void TaskFilterProxyModel::setFilterUntagged(bool filter)
{
    if (m_filterUntagged != filter) {
        m_filterUntagged = filter;
        invalidateFilter();
    }
}

void TaskFilterProxyModel::setFilterDueDated(bool filter)
{
    if (m_filterDueDated != filter) {
        m_filterDueDated = filter;
        invalidateFilter();
    }
}

void TaskFilterProxyModel::setFilterArchived(bool filter)
{
    if (filter != m_filterArchived) {
        m_filterArchived = filter;
        if (filter)
            setFilterStaged(false);
        invalidateFilter();
    }
}

void TaskFilterProxyModel::setFilterStaged(bool filter)
{
    if (filter != m_filterStaged) {
        m_filterStaged = filter;
        if (filter)
            setFilterArchived(false);
        invalidateFilter();
    }
}

void TaskFilterProxyModel::invalidateFilter()
{
    QSortFilterProxyModel::invalidateFilter();
    m_previousCount = rowCount();
}

void TaskFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    m_previousCount = rowCount();
}

void TaskFilterProxyModel::onSourceCountChanged()
{
    emit countChanged(rowCount(), m_previousCount);
    m_previousCount = rowCount();
}

// factored out into a separate method to avoid duplicated code
static bool evenMoreDefaultLessThan(const Task::Ptr &leftTask, const Task::Ptr &rightTask)
{
    if (leftTask->creationDate() == rightTask->creationDate()) {
        return leftTask->summary() < rightTask->summary();
    } else {
        return leftTask->creationDate() > rightTask->creationDate();
    }
}

bool TaskFilterProxyModel::defaultLessThan(const Task::Ptr &leftTask, const Task::Ptr &rightTask) const
{
    const int leftPriority = leftTask->priority() == Task::PriorityNone ? 9 : leftTask->priority();
    const int rightPriority = rightTask->priority() == Task::PriorityNone ? 9 : rightTask->priority();

    if (leftPriority == rightPriority) {
        if (leftTask->hasDueDate() && rightTask->hasDueDate()) {
            if (leftTask->dueDate() == rightTask->dueDate()) {
                return evenMoreDefaultLessThan(leftTask, rightTask);
            } else {
                return leftTask->dueDate() < rightTask->dueDate();
            }
        } else if (leftTask->hasDueDate() ^ rightTask->hasDueDate()) {
            return rightTask->hasDueDate();
        } else {
            return evenMoreDefaultLessThan(leftTask, rightTask);
        }
    } else {
        return leftPriority < rightPriority;
    }
}
