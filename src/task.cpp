/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  Copyright (C) 2015-2016 Sérgio Martins <iamsergio@gmail.com>

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

#include "task.h"
#include "settings.h"
#include "checkabletagmodel.h"
#include "taskcontextmenumodel.h"
#include "sortedtaskcontextmenumodel.h"
#if defined(UNIT_TEST_RUN)
# include "assertingproxymodel.h"
#endif
#include "storage.h"
#include "kernel.h"

#include <QQmlEngine>
#include <QUuid>

#if defined(UNIT_TEST_RUN)
    int Task::taskCount;
#endif

enum {
    TagRole = Qt::UserRole,
    TaskRole
};

static QVariant dataFunction(const TagRef::List &list, int index, int role)
{
    switch (role) {
    case TagRole:
        return QVariant::fromValue<Tag*>(list.at(index).tag().data());
    case TaskRole:
        return QVariant::fromValue<Task*>(list.at(index).m_task.data());
    default:
        return QVariant();
    }
}

Task::Task(Kernel *kernel, const QString &summary)
    : QObject()
    , Syncable()
    , m_summary(summary.isEmpty() ? tr("New Task") : summary)
    , m_checkableTagModel(0)
    , m_status(TaskStopped)
    , m_staged(false)
    , m_creationDate(QDateTime::currentDateTimeUtc())
    , m_modificationDate(m_creationDate)
    , m_contextMenuModel(0)
    , m_sortedContextMenuModel(0)
    , m_kernel(kernel)
    , m_priority(PriorityNone)
    , m_estimatedEffort(0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(this, &Task::summaryChanged, &Task::onEdited);
    connect(this, &Task::tagsChanged, &Task::onEdited);
    connect(this, &Task::descriptionChanged, &Task::onEdited);
    connect(this, &Task::statusChanged, &Task::onEdited);
    connect(this, &Task::stagedChanged, &Task::onEdited);
    connect(this, &Task::dueDateChanged, &Task::onEdited);
    connect(this, &Task::priorityChanged, &Task::onEdited);
    connect(this, &Task::estimatedEffortChanged, &Task::onEdited);

    connect(this, &Task::dueDateDisplayTextChanged, &Task::upperRightCornerTextChanged);
    connect(this, &Task::estimatedEffortChanged, &Task::upperRightCornerTextChanged);
    connect(this, &Task::dueDateChanged, &Task::dueDateDisplayTextChanged);
    connect(m_kernel->settings(), &Settings::showTaskAgeChanged, this, &Task::upperRightCornerTextChanged);
    connect(m_kernel->settings(), &Settings::supportsEffortChanged, this, &Task::upperRightCornerTextChanged);

    connect(kernel, &Kernel::dayChanged, this, &Task::onDayChanged);

#if defined(UNIT_TEST_RUN)
    taskCount++;
#endif

    if (kernel)
        modelSetup();
}

void Task::modelSetup()
{
    Storage *storage = m_kernel->storage();

    m_checkableTagModel = new CheckableTagModel(this);

    m_tags.setDataFunction(&dataFunction);
    m_tags.insertRole("tag", Q_NULLPTR, TagRole);
    m_tags.insertRole("task", Q_NULLPTR, TaskRole);

    QAbstractItemModel *tagsModel = m_tags; // android doesn't build if you use m_tags directly in the connect statement
    connect(tagsModel, &QAbstractListModel::modelReset, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::rowsInserted, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::rowsRemoved, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::layoutChanged, this, &Task::tagsChanged);
    connect(tagsModel, &QAbstractListModel::dataChanged, this, &Task::tagsChanged);

    QHash<int, QByteArray> roleNames = storage->tagsModel()->roleNames();
    roleNames.insert(Qt::CheckStateRole, QByteArray("checkState"));
    QAbstractItemModel *allTagsModel = storage->tagsModel();
    Q_ASSERT(allTagsModel);
    m_checkableTagModel->setSourceModel(allTagsModel);
    m_contextMenuModel = new TaskContextMenuModel(this, this);
    m_sortedContextMenuModel = new SortedTaskContextMenuModel(this);
    m_sortedContextMenuModel->setSourceModel(m_contextMenuModel);

#if defined(UNIT_TEST_RUN)
    AssertingProxyModel *assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_contextMenuModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_sortedContextMenuModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_checkableTagModel);
    assert = new AssertingProxyModel(this);
    assert->setSourceModel(m_tags);
#endif
}

Task::~Task()
{
#if defined(UNIT_TEST_RUN)
    taskCount--;
#endif
}

Task::Ptr Task::createTask(Kernel *kernel, const QString &summary, const QString &uid)
{
    Task::Ptr task = Task::Ptr(new Task(kernel, summary));
    task->setWeakPointer(task);
    if (!uid.isEmpty())
        task->setUuid(uid);
    return task;
}

bool Task::staged() const
{
   return m_staged;
}

void Task::setStaged(bool staged)
{
    if (m_staged != staged) {
        m_staged = staged;
        emit stagedChanged();

        const QDate today = m_kernel->currentDate();
        if (!staged && recurs() && m_dueDate.date() <= today) {
            do {
                m_dueDate.makeNext();
            } while (m_dueDate.date() <= today);
            emit dueDateChanged();
        }
    }
}

QString Task::summary() const
{
    return m_summary;
}

void Task::setSummary(const QString &text)
{
    if (m_summary != text) {
        m_summary = text;
        emit summaryChanged();
    }
}

QString Task::description() const
{
    return m_description;
}

void Task::setDescription(const QString &text)
{
    if (m_description != text) {
        m_description = text;
        emit descriptionChanged();
    }
}

bool Task::hasDescription() const
{
    return !m_description.isEmpty();
}

bool Task::containsTag(const QString &name) const
{
    return indexOfTag(name) != -1;
}

int Task::indexOfTag(const QString &name) const
{
    QString trimmedName = name.toLower().trimmed();
    for (int i = 0; i < m_tags.count(); ++i) {
        Q_ASSERT(m_tags.at(i).storage());
        if (m_tags.at(i).tagName().toLower() == trimmedName)
            return i;
    }

    return -1;
}

TagRef::List Task::tags() const
{
    return m_tags;
}

void Task::setTagList(const TagRef::List &list)
{
    if (!m_tags.isEmpty()) // No need to emit uneeded signals
        m_tags.clear();

    // Filter out duplicated tags
    QStringList addedTags;
    foreach (const TagRef &ref, list) {
        QString name = ref.tagName().toLower();
        if (!addedTags.contains(name) && !name.isEmpty()) {
            addedTags << name;
            m_tags << ref;
        }
    }
}

QAbstractItemModel *Task::tagModel() const
{
    return m_tags;
}

QAbstractItemModel *Task::checkableTagModel() const
{
    return m_checkableTagModel;
}

void Task::addTag(const QString &tagName)
{
    QString trimmedName = tagName.trimmed();
    if (trimmedName.isEmpty())
        return;

    if (!containsTag(trimmedName)) {
        m_tags.append(TagRef(this, trimmedName, storage(), /*temporary=*/ true));
        emit tagToggled(trimmedName);
    }
}

void Task::removeTag(const QString &tagName)
{
    int index = indexOfTag(tagName);
    if (index != -1) {
        m_tags.removeAt(index);
        emit tagToggled(tagName);
    }
}

void Task::toggleTag(const QString &tagName)
{
    if (containsTag(tagName)) {
        removeTag(tagName);
    } else {
        addTag(tagName);
    }
}

void Task::removeDueDate()
{
    setDueDate(DueDate());
}

TaskStatus Task::status() const
{
    return m_status;
}

void Task::setStatus(TaskStatus status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void Task::setCreationDate(const QDateTime &date)
{
    m_creationDate = date;
}

void Task::setLastPomodoroDate(const QDateTime &date)
{
    if (date != m_lastPomodoroDate) {
        m_lastPomodoroDate = date;
        emit daysSinceLastPomodoroChanged();
    }
}

QDateTime Task::creationDate() const
{
    return m_creationDate;
}

void Task::setModificationDate(const QDateTime &date)
{
    m_modificationDate = date;
}

QDateTime Task::modificationDate() const
{
    return m_modificationDate;
}

QDateTime Task::lastPomodoroDate() const
{
    return m_lastPomodoroDate;
}

QDate Task::dueDate() const
{
    return m_dueDate.date();
}

void Task::setDueDate(DueDate date)
{
    if (date != m_dueDate) {
        m_dueDate = date;
        emit dueDateChanged();
    }
}

bool Task::hasDueDate() const
{
    return m_dueDate.isValid();
}

bool Task::running() const
{
    return m_status == TaskStarted;
}

bool Task::stopped() const
{
    return m_status == TaskStopped;
}

bool Task::paused() const
{
    return m_status == TaskPaused;
}

Task::Ptr Task::toStrongRef() const
{
    Q_ASSERT(m_this);
    return m_this.toStrongRef();
}

void Task::setWeakPointer(const QWeakPointer<Task> &ptr)
{
    m_this = ptr;
}

QVariantMap Task::toJson() const
{
    QVariantMap map = Syncable::toJson();
    map.insert(QStringLiteral("summary"), m_summary);
    map.insert(QStringLiteral("staged"), m_staged);
    map.insert(QStringLiteral("description"), m_description);
    QVariantList tags;
    const int numTags = m_tags.count();
    tags.reserve(numTags);
    for (int i = 0; i < numTags; ++i)
        tags << m_tags.at(i).tagName();
    map.insert(QStringLiteral("tags"), tags);
    map.insert(QStringLiteral("creationTimestamp"), m_creationDate.toMSecsSinceEpoch());

    if (m_modificationDate.isValid())
        map.insert(QStringLiteral("modificationTimestamp"), m_modificationDate.toMSecsSinceEpoch());

    if (m_lastPomodoroDate.isValid())
        map.insert(QStringLiteral("lastPomodoroDate"), m_lastPomodoroDate.toMSecsSinceEpoch());

    if (m_dueDate.isValid()) {
        map.insert(QStringLiteral("dueDate"), m_dueDate.toJulianDay());
        if (m_dueDate.recurs()) {
            map.insert(QStringLiteral("periodType"), m_dueDate.periodType());
            if (m_dueDate.frequency() > 1)
                map.insert(QStringLiteral("frequency"), m_dueDate.frequency());
        }
    }

    if (m_priority != PriorityNone)
        map.insert(QStringLiteral("priority"), QVariant(m_priority));

    if (m_estimatedEffort > 0)
        map.insert(QStringLiteral("estimatedEffort"), m_estimatedEffort);

    return map;
}

QVector<QString> Task::supportedFields() const
{
    static QVector<QString> fields;
    if (fields.isEmpty()) {
        fields = Syncable::supportedFields();
        fields.reserve(1); // so I don't forget reserve when adding more fields
        fields << QStringLiteral("summary")                 // Since 0.9
               << QStringLiteral("staged")                  // Since 0.9
               << QStringLiteral("description")             // Since 0.9
               << QStringLiteral("creationTimestamp")       // Since 0.9
               << QStringLiteral("tags")                    // Since 0.9
               << QStringLiteral("modificationTimestamp")   // Since 0.9
               << QStringLiteral("lastPomodoroDate")        // Since 0.9
               << QStringLiteral("dueDate")                 // Since 1.0
               << QStringLiteral("priority")                // Since 1.0
               << QStringLiteral("periodType")              // Since 1.1
               << QStringLiteral("frequency")               // Since 1.1
               << QStringLiteral("estimatedEffort");        // Since 1.2
    }

    return fields;
}

void Task::fromJson(const QVariantMap &map)
{
    Syncable::fromJson(map);

    QString summary = map.value(QStringLiteral("summary")).toString();
    if (summary.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "empty task summary";
        summary = tr("New Task");
    }

    blockSignals(true); // so we don't increment revision while calling setters
    setSummary(summary);

    QString description = map.value(QStringLiteral("description")).toString();
    setDescription(description);
    setStaged(map.value(QStringLiteral("staged"), false).toBool());
    setPriority(static_cast<Priority>(map.value(QStringLiteral("priority"), PriorityNone).toInt()));
    int effort = map.value(QStringLiteral("estimatedEffort"), 0).toInt();
    if (effort > 0)
        setEstimatedEffort(effort);

    QDateTime creationDate = QDateTime::fromMSecsSinceEpoch(map.value(QStringLiteral("creationTimestamp"), QDateTime()).toLongLong());
    if (creationDate.isValid()) // If invalid it uses the ones set in CTOR
        setCreationDate(creationDate);

    QDateTime modificationDate = QDateTime::fromMSecsSinceEpoch(map.value(QStringLiteral("modificationTimestamp"), QDateTime()).toLongLong());
    if (modificationDate.isValid())
        setModificationDate(modificationDate);

    QDateTime lastPomodoroDate = QDateTime::fromMSecsSinceEpoch(map.value(QStringLiteral("lastPomodoroDate"), QDateTime()).toLongLong());
    if (lastPomodoroDate.isValid())
        setLastPomodoroDate(lastPomodoroDate);

    if (map.contains(QStringLiteral("dueDate"))) { // from julian of QDate() then toJulian returns a valid date, so check map
        QDate date = QDate::fromJulianDay(map.value(QStringLiteral("dueDate")).toLongLong());
        if (date.isValid() && date.toJulianDay() != 0) {
            int periodType = map.value(QStringLiteral("periodType"), DueDate::PeriodTypeNone).toInt();
            int frequency = map.value(QStringLiteral("frequency"), 1).toInt();
            setDueDate(DueDate(date, static_cast<DueDate::PeriodType>(periodType), frequency));
        }
    }

    QVariantList tagsVariant = map.value(QStringLiteral("tags")).toList();
    TagRef::List tags;
    foreach (const QVariant &tag, tagsVariant) {
        if (!tag.toString().isEmpty())
            tags << TagRef(this, tag.toString(), storage());
    }

    setTagList(tags);
    blockSignals(false);
}

SortedTaskContextMenuModel *Task::sortedContextMenuModel() const
{
    return m_sortedContextMenuModel;
}

TaskContextMenuModel *Task::contextMenuModel() const
{
    return m_contextMenuModel;
}

bool Task::operator==(const Task &other) const
{
    return m_uuid == other.uuid();
}

Kernel *Task::kernel() const
{
    return m_kernel;
}

void Task::setKernel(Kernel *kernel)
{
    Q_ASSERT(kernel && !m_kernel);
    m_kernel = kernel;

    for (int i = 0; i < m_tags.count(); ++i) {
        TagRef tagRef(this, m_tags.at(i).tagName(), m_kernel->storage());
        m_tags.replace(i, tagRef); // operator[] is private
    }

    modelSetup();
}

Storage *Task::storage() const
{
    return m_kernel ? m_kernel->storage() : 0;
}

bool Task::equals(Task *other) const
{
    if (!Syncable::equals(other))
        return false;

    if (m_summary != other->summary() || m_description != other->description())
        return false;

    if (m_tags.count() != other->tags().count())
        return false;

    for (int i = 0; i < m_tags.count(); ++i) {
        if (!m_tags.at(i).tag()->equals(other->tags().at(i).tag().data()))
            return false;
    }

    return true;
}

void Task::onEdited()
{
    m_modificationDate = QDateTime::currentDateTimeUtc();
    m_revision++;
    emit changed();
}

void Task::onDayChanged()
{
    if (m_dueDate.isValid()) {
        emit dueDateDisplayTextChanged();
        if (dueToday() || isOverdue())
            setStaged(true);
    }
}

int Task::daysSinceCreation() const
{
    if (!m_creationDate.isValid())
        return -1;

    return m_creationDate.toLocalTime().date().daysTo(QDate::currentDate());
}

int Task::daysSinceLastPomodoro() const
{
    if (!m_lastPomodoroDate.isValid())
        return -1;

    return m_lastPomodoroDate.toLocalTime().date().daysTo(QDate::currentDate());
}

QString Task::dueDateString() const
{
    return m_dueDate.isValid() ? m_dueDate.date().toString() : QString();
}

QString Task::prettyDueDateString() const
{
    if (!m_dueDate.isValid())
        return QString();

    const QDate date = m_dueDate.date();

    QString pretty;

    const QDate today = QDate::currentDate();
    const int daysTo = today.daysTo(date);
    if (date == today)
        pretty = tr("today");
    else if (date == today.addDays(1))
        pretty = tr("tomorrow");
    else if (daysTo < 0)
        pretty = tr("overdue");
     else if (daysTo <= 7)
        pretty = tr("next %1").arg(m_dueDate.date().toString(QStringLiteral("dddd"))); // Next Monday, for example.
    else // Year is visual noise if equal to current year
        pretty = date.year() == today.year() ? date.toString(QStringLiteral("MMMM d"))
                                             : date.toString(Qt::ISODate);

    return pretty;
}

QString Task::prettyDueDateRecurString() const
{
    return prettyDueDateString() + (m_dueDate.recurs() ? (" (" + m_dueDate.recurrenceString() + ")")
                                                       : QString());
}

bool Task::isOverdue() const
{
    return m_dueDate.isValid() && m_dueDate.date() < QDate::currentDate();
}

bool Task::dueToday() const
{
    return m_dueDate.isValid() && m_dueDate.date() == QDate::currentDate();
}

void Task::setPriority(Priority priority)
{
    if (priority != m_priority) {
        m_priority = priority;
        emit priorityChanged();
    }
}

Task::Priority Task::priority() const
{
    return m_priority;
}

QString Task::priorityStr() const
{
    if (m_priority == PriorityHigh) {
        return tr("high");
    } else if (m_priority == PriorityLow) {
        return tr("low");
    } else {
        return QString();
    }
}

bool Task::isUrl() const
{
    // We only want to open http urls, so don't use QUrl::scheme()
    return m_summary.startsWith(QLatin1String("http://"), Qt::CaseInsensitive) ||
           m_summary.startsWith(QLatin1String("https://"), Qt::CaseInsensitive);
}

DueDate::PeriodType Task::periodType() const
{
    return m_dueDate.periodType();
}

bool Task::recurs() const
{
    return m_dueDate.recurs();
}

QString Task::frequencyWord() const
{
    return m_dueDate.frequencyWord();
}

uint Task::frequency() const
{
    return m_dueDate.frequency();
}

void Task::setFrequency(uint frequency)
{
    if (frequency != m_dueDate.frequency()) {
        m_dueDate.setFrequency(frequency);
        emit dueDateChanged();
    }
}

int Task::estimatedEffort() const
{
   return m_estimatedEffort;
}

void Task::setEstimatedEffort(int effort)
{
    if (m_estimatedEffort != effort) {
        m_estimatedEffort = effort;
        emit estimatedEffortChanged();
    }
}

QString Task::upperRightCornerText() const
{
    auto s = m_kernel->settings();
    const bool hasEffort = s->supportsEffort() && m_estimatedEffort > 0;
    const bool hasAge = s->showTaskAge() && !hasDueDate();

    if (hasEffort && hasAge) {
        return QStringLiteral("%1,~%2").arg(daysSinceCreation()).arg(m_estimatedEffort);
    } else if (hasEffort) {
        return QStringLiteral("~%1").arg(m_estimatedEffort);
    } else if (hasAge) {
        return QStringLiteral("%1").arg(daysSinceCreation());
    }

    return QString();
}

void Task::incrementPriority()
{
    switch (m_priority) {
    case PriorityNone:
        setPriority(PriorityHigh);
        break;
    case PriorityHigh:
        break;
    case PriorityLow:
        setPriority(PriorityNone);
        break;
    }
}

void Task::decrementPriority()
{
    switch (m_priority) {
    case PriorityNone:
        setPriority(PriorityLow);
        break;
    case PriorityHigh:
        setPriority(PriorityNone);
        break;
    case PriorityLow:
        break;
    }
}

void Task::toggleRecurrenceType(PeriodType type)
{
    if (!m_dueDate.isValid())
        return;

    auto ptype = static_cast<DueDate::PeriodType>(type);
    if (ptype <= DueDate::PeriodTypeNone || ptype >= DueDate::PeriodTypeCount) {
        m_dueDate.setPeriodType(DueDate::PeriodTypeNone);
    } else {
        m_dueDate.setPeriodType(ptype == m_dueDate.periodType() ? DueDate::PeriodTypeNone : ptype);
    }

    emit dueDateChanged();
}
