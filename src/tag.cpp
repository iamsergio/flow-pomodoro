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

#include "tag.h"
#include "tagstorage.h"
#include "remove_if.h"
#include "taskstorage.h"
#include "taskfilterproxymodel.h"
#include <QQmlEngine>

using namespace std::placeholders;

static bool taskIsTagged(const QPointer<Tag> &tag, const QVariant &variant)
{
    Task::Ptr task = variant.value<Task::Ptr>();
    if (!task || !tag) {
        qWarning() << Q_FUNC_INFO << "Unexpected null task or tag";
        return false;
    }

    return task->containsTag(tag->name());
}

Tag::Tag(const QString &_name)
    : QObject()
    , m_name(_name)
    , m_taskCount(0)
    , m_beingEdited(false)
    , m_taskModel(0)
{
    Q_ASSERT(!m_name.isEmpty());

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    TagStorage::instance()->monitorTag(this);
}

Tag::~Tag()
{
    delete m_taskModel;
}

int Tag::taskCount() const
{
    return m_taskCount;
}

void Tag::setTaskCount(int count)
{
    if (count != m_taskCount) {
        int tmp = m_taskCount;
        m_taskCount = count;
        emit taskCountChanged(tmp, m_taskCount);
    }
}

QString Tag::name() const
{
    return m_name;
}

void Tag::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged();
    }
}

bool Tag::beingEdited() const
{
    return m_beingEdited;
}

void Tag::setBeingEdited(bool yes)
{
    if (m_beingEdited != yes) {
        m_beingEdited = yes;
        emit beingEditedChanged();
    }
}

QAbstractItemModel *Tag::taskModel()
{
    // Delayed initialization do avoid deadlock accessing TaskStorage::instance() when TaskStorage is being constructed
    if (!m_taskModel) {
        auto filterFunc = std::bind(&taskIsTagged, QPointer<Tag>(this), _1);
        m_taskModel = new FunctionalModels::Remove_if(TaskStorage::instance()->taskFilterModel(),
                                                      filterFunc, TaskStorage::TaskPtrRole);

        connect(this, &Tag::taskCountChanged,
                m_taskModel, &FunctionalModels::Remove_if::invalidateFilter,
                Qt::QueuedConnection);
    }

    return m_taskModel;
}

bool operator==(const Tag::Ptr &tag1, const Tag::Ptr &tag2)
{
    return tag1 && tag2 && tag1->name() == tag2->name();
}
