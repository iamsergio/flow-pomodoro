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
#include "taskfilterproxymodel.h"
#if defined(UNIT_TEST_RUN)
# include "assertingproxymodel.h"
#endif
#include "storage.h"
#include "kernel.h"

#include <QQmlEngine>

#if defined(QT_TESTLIB_LIB)
    int Tag::tagCount = 0;
#endif

Tag::Tag(Kernel *kernel, const QString &name)
    : QObject()
    , Syncable()
    , m_name(name.trimmed())
    , m_taskCount(0)
    , m_beingEdited(false)
    , m_taskModel(Q_NULLPTR)
    , m_kernel(kernel)
    , m_isFake(false)
    , m_dontUpdateRevision(false)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
#if defined(QT_TESTLIB_LIB)
    tagCount++;
    // qDebug() << "[TAG] CTOR " << ((void*)this) << m_name;
#endif
}

Tag::Tag(const QString &name, QAbstractItemModel *taskModel)
    : QObject()
    , Syncable()
    , m_name(name)
    , m_taskCount(0)
    , m_beingEdited(false)
    , m_taskModel(taskModel)
    , m_kernel(Q_NULLPTR)
    , m_isFake(true)
    , m_dontUpdateRevision(true)
{
}

Tag::~Tag()
{
#if defined(QT_TESTLIB_LIB)
    if (m_kernel)
        tagCount--;
    // qDebug() << "[TAG] DTOR " << ((void*)this) << m_name;
#endif
}

int Tag::taskCount() const
{
    return m_taskCount;
}

void Tag::incrementTaskCount(int increment)
{
    Q_ASSERT(!m_isFake);
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
    Q_ASSERT(!m_isFake);
    if (name.trimmed().toLower() != m_name.toLower() && !name.isEmpty()) {
        m_name = name.trimmed(); // We preserve original case
        if (!m_dontUpdateRevision)
            m_revision++;
        emit nameChanged();
    }
}

bool Tag::beingEdited() const
{
    return m_beingEdited;
}

void Tag::setBeingEdited(bool yes)
{
    Q_ASSERT(!m_isFake);
    if (m_beingEdited != yes) {
        m_beingEdited = yes;
        emit beingEditedChanged();
    }
}

QAbstractItemModel *Tag::taskModel()
{
    if (!m_taskModel && !m_isFake) {
        TaskFilterProxyModel *model = new TaskFilterProxyModel(this);
        model->setTagName(m_name);
        model->setSourceModel(m_kernel->storage()->archivedTasksModel());
        model->setObjectName(QString("Tasks with tag %1 model").arg(m_name));
        m_taskModel = model;

        connect(this, &Tag::taskCountChanged,
                model, &TaskFilterProxyModel::invalidateFilter,
                Qt::QueuedConnection);
#if defined(UNIT_TEST_RUN)
        AssertingProxyModel *assert = new AssertingProxyModel(this);
        assert->setSourceModel(m_taskModel);
#endif
    }

    return m_taskModel;
}

QVariantMap Tag::toJson() const
{
    Q_ASSERT(!m_isFake);
    QVariantMap map = Syncable::toJson();
    map.insert("name", m_name);
    return map;
}

void Tag::fromJson(const QVariantMap &map)
{
    Q_ASSERT(!m_isFake);
    Syncable::fromJson(map);
    QString name = map.value("name").toString();
    if (name.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "empty tag name";
    } else {
        m_dontUpdateRevision = true;
        setName(name);
        m_dontUpdateRevision = false;
    }
}

bool Tag::operator==(const Tag &other) const
{
    return m_uuid == other.uuid() || m_name == other.m_name;
}

Kernel *Tag::kernel() const
{
    return m_kernel;
}

void Tag::setKernel(Kernel *kernel)
{
    Q_ASSERT(kernel && !m_kernel);
    Q_ASSERT(!m_isFake);
    m_kernel = kernel;
}

bool Tag::isFake() const
{
    return m_isFake;
}

bool Tag::equals(Tag *other) const
{
    return Syncable::equals(other) && m_name.trimmed().toLower() == other->name().trimmed().toLower();
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

QVector<QString> Tag::supportedFields() const
{
    static QVector<QString> fields;
    if (fields.isEmpty()) {
        fields = Syncable::supportedFields();
        fields.reserve(1); // so I don't forget reserve when adding more fields
        fields << "name"; // since 0.9
    }

    return fields;
}
