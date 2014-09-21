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
#include "taskfilterproxymodel.h"
#include "archivedtasksfiltermodel.h"
#include "taskfilterproxymodel.h"
#include "storage.h"
#include "kernel.h"

#include <QQmlEngine>

#if defined(QT_TESTLIB_LIB)
    int Tag::tagCount = 0;
#endif

Tag::Tag(Kernel *kernel, const QString &_name)
    : QObject()
    , Syncable()
    , m_name(_name.trimmed())
    , m_taskCount(0)
    , m_beingEdited(false)
    , m_taskModel(Q_NULLPTR)
    , m_kernel(kernel)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    //Kernel::instance()->storage()->monitorTag(this);
#if defined(QT_TESTLIB_LIB)
    tagCount++;
#endif
}

Tag::~Tag()
{
#if defined(QT_TESTLIB_LIB)
    tagCount--;
#endif
}

int Tag::taskCount() const
{
    return m_taskCount;
}

void Tag::incrementTaskCount(int increment)
{
    if (m_taskCount + increment >= 0) {
        m_taskCount += increment;
        emit taskCountChanged(m_taskCount - increment, m_taskCount);
    } else {
        Q_ASSERT(false);
    }
}

QString Tag::name() const
{
    return m_name;
}

void Tag::setName(const QString &name)
{
    if (name.trimmed().toLower() != m_name.toLower() && !name.isEmpty()) {
        m_name = name.trimmed(); // We preserve original case
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
    // Delayed initialization do avoid deadlock accessing Kernel::instance()->storage() when TaskStorage is being constructed
    // TODO: this should be ok now
    if (!m_taskModel) {
        m_taskModel = new TaskFilterProxyModel(this);
        m_taskModel->setTagName(m_name);
        m_taskModel->setSourceModel(m_kernel->storage()->archivedTasksModel());
        m_taskModel->setObjectName(QString("Tasks with tag %1 model").arg(m_name));

        connect(this, &Tag::taskCountChanged,
                m_taskModel, &TaskFilterProxyModel::invalidateFilter,
                Qt::QueuedConnection);
    }

    return m_taskModel;
}

QVariantMap Tag::toJson() const
{
    QVariantMap map = Syncable::toJson();
    map.insert("name", m_name);
    return map;
}

void Tag::fromJson(const QVariantMap &map)
{
    Syncable::fromJson(map);
    QString name = map.value("name").toString();
    if (name.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "empty tag name";
    } else {
        setName(name);
    }
}

bool Tag::operator==(const Tag &other) const
{
    return other.m_name.toLower() == m_name.toLower();
}

bool operator==(const Tag::Ptr &tag1, const Tag::Ptr &tag2)
{
    return (!tag1 && !tag2) || (tag1 && tag2 && *tag1 == *tag2);
}

void Tag::onTaskStagedChanged()
{
    //Task *task = qobject_cast<Task*>(sender());
    //Q_ASSERT(task);
}
