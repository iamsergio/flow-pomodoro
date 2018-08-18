/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  Copyright (C) 2015-2016,2018 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_TASK_H
#define FLOW_TASK_H

#include "tag.h"
#include "tagref.h"
#include "syncable.h"
#include "genericlistmodel.h"
#include "duedate.h"

#include <QString>
#include <QMetaType>
#include <QDateTime>
#include <QDataStream>
#include <QSharedPointer>

enum TaskStatus {
    TaskStopped,
    TaskPaused,
    TaskStarted
};

enum SerializerVersion {
    SerializerVersion1 = 100,
    SerializerVersion2 = 101 // Added Task::description()
};

class CheckableTagModel;
class QAbstractListModel;
class Storage;
class Kernel;
class TaskContextMenuModel;
class SortedTaskContextMenuModel;

class Task : public QObject, public Syncable {
    Q_OBJECT
    Q_PROPERTY(bool isUrl READ isUrl NOTIFY summaryChanged)
    Q_PROPERTY(QString priorityStr READ priorityStr NOTIFY priorityChanged)
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(bool dueToday READ dueToday NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(bool isOverdue READ isOverdue NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(QString prettyDueDateString READ prettyDueDateString NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(QString prettyDueDateRecurString READ prettyDueDateRecurString NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(QString dueDateString READ dueDateString NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(QDate dueDate READ dueDate WRITE setDueDate NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(bool hasDueDate READ hasDueDate NOTIFY dueDateChanged)
    Q_PROPERTY(int daysSinceLastPomodoro READ daysSinceLastPomodoro NOTIFY daysSinceLastPomodoroChanged)
    Q_PROPERTY(int daysSinceCreation READ daysSinceCreation NOTIFY daysSinceCreationChanged)
    Q_PROPERTY(bool staged READ staged WRITE setStaged NOTIFY stagedChanged)
    Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool hasDescription READ hasDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QObject * tagModel READ tagModel CONSTANT)
    Q_PROPERTY(QObject * checkableTagModel READ checkableTagModel CONSTANT)
    // Shortcuts
    Q_PROPERTY(bool paused  READ paused  NOTIFY statusChanged STORED false)
    Q_PROPERTY(bool stopped READ stopped NOTIFY statusChanged STORED false)
    Q_PROPERTY(bool running READ running NOTIFY statusChanged STORED false)

    Q_PROPERTY(TaskContextMenuModel* contextMenuModel READ contextMenuModel CONSTANT)
    Q_PROPERTY(SortedTaskContextMenuModel* sortedContextMenuModel READ sortedContextMenuModel CONSTANT)
    Q_PROPERTY(int periodType READ periodType NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(bool recurs READ recurs NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(uint frequency READ frequency WRITE setFrequency NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(QString frequencyWord READ frequencyWord NOTIFY dueDateDisplayTextChanged)
    Q_PROPERTY(int estimatedEffort READ estimatedEffort WRITE setEstimatedEffort NOTIFY estimatedEffortChanged)
    Q_PROPERTY(QString upperRightCornerText READ upperRightCornerText NOTIFY upperRightCornerTextChanged)

public:
    typedef QSharedPointer<Task> Ptr;
    typedef GenericListModel<Ptr> List;

    enum Priority {
        // Don't change the numbering, 1 is the highest, per rfc2445
        PriorityNone   = 0,
        PriorityHigh   = 1,
        PriorityLow    = 10,
    };
    Q_ENUM(Priority)

    enum PeriodType { // Duplicated otherwise we can't use it in QML
        PeriodTypeNone = DueDate::PeriodTypeNone,
        PeriodTypeDaily,
        PeriodTypeWeekly,
        PeriodTypeMonthly,
        PeriodTypeYearly,
        PeriodTypeCount
    };
    Q_ENUM(PeriodType)

    ~Task();

    static Task::Ptr createTask(Kernel *kernel,
                                const QString &name = QString(),
                                const QString &uid = QString());

    bool staged() const;
    void setStaged(bool);

    QString summary() const;
    void setSummary(const QString &text);

    QString description() const;
    void setDescription(const QString &text);
    bool hasDescription() const;

    bool containsTag(const QString &name) const;
    int indexOfTag(const QString &name) const;
    TagRef::List tags() const;
    void setTagList(const TagRef::List &);
    QAbstractItemModel *tagModel() const;
    QAbstractItemModel *checkableTagModel() const;

    Q_INVOKABLE void addTag(const QString &tagName);
    Q_INVOKABLE void removeTag(const QString &tagName);
    Q_INVOKABLE void toggleTag(const QString &tagName);
    Q_INVOKABLE void removeDueDate();

    TaskStatus status() const;
    void setStatus(TaskStatus status);

    QDateTime creationDate() const;
    QDateTime modificationDate() const;
    QDateTime lastPomodoroDate() const;
    QDate dueDate() const;
    void setDueDate(QDate);
    bool hasDueDate() const;
    void setLastPomodoroDate(const QDateTime &);

    bool running() const;
    bool stopped() const;
    bool paused() const;

    Task::Ptr toStrongRef() const;
    void setWeakPointer(const QWeakPointer<Task> &);

    QVariantMap toJson() const override;
    void fromJson(const QVariantMap &) override;

    TaskContextMenuModel *contextMenuModel() const;
    SortedTaskContextMenuModel *sortedContextMenuModel() const;

    bool operator==(const Task &other) const;
    Kernel *kernel() const;
    void setKernel(Kernel *);
    Storage *storage() const;

    bool equals(Task *) const;

#if defined(UNIT_TEST_RUN)
    static int taskCount;
#endif

    int daysSinceCreation() const; // days since creation date
    int daysSinceLastPomodoro() const;

    QString dueDateString() const;
    QString prettyDueDateString() const;
    QString prettyDueDateRecurString() const;

    bool isOverdue() const;
    bool dueToday() const;

    void setPriority(Priority);
    Priority priority() const;

    QString priorityStr() const;
    bool isUrl() const;

    DueDate::PeriodType periodType() const;
    bool recurs() const;
    QString frequencyWord() const;
    uint frequency() const;
    void setFrequency(uint);

    int estimatedEffort() const;
    void setEstimatedEffort(int);

    QString upperRightCornerText() const;
    void incrementPriority();
    void decrementPriority();

    QString tagsStr() const;

public Q_SLOTS:
    void toggleRecurrenceType(PeriodType type);

Q_SIGNALS:
    void priorityChanged();
    void dueDateStringChanged();
    void daysSinceLastPomodoroChanged();
    void daysSinceCreationChanged();
    void summaryChanged();
    void descriptionChanged();
    void tagsChanged();
    void statusChanged(); // not a stored property, so not connected to changed()
    void stagedChanged();
    void changed();
    void tagToggled(const QString &tag);
    void dueDateChanged();
    void dueDateDisplayTextChanged();
    void estimatedEffortChanged();
    void upperRightCornerTextChanged();

protected:
    QVector<QString> supportedFields() const override;

private Q_SLOTS:
    void onEdited();
    void onDayChanged();

private:
    explicit Task(Kernel *kernel, const QString &name = QString());
    void modelSetup();
    void setModificationDate(const QDateTime &);
    void setCreationDate(const QDateTime &);
    void setDueDate(DueDate);

    QString m_summary;
    QString m_description;
    TagRef::List m_tags;
    CheckableTagModel *m_checkableTagModel = nullptr;
    TaskStatus m_status = TaskStopped;
    bool m_staged = false;
    QWeakPointer<Task> m_this;
    QDateTime m_creationDate;
    QDateTime m_modificationDate;
    QDateTime m_lastPomodoroDate;
    DueDate m_dueDate;
    TaskContextMenuModel *m_contextMenuModel = nullptr;
    SortedTaskContextMenuModel *m_sortedContextMenuModel = nullptr;
    Kernel *m_kernel = nullptr;
    Priority m_priority = PriorityNone;
    int m_estimatedEffort = 0;
};

inline QDebug operator<<(QDebug dbg, const Task::Ptr &task)
{
    if (!task)
        return dbg;

    dbg.nospace() << "Task: uuid=" << task->uuid() << "; summary=" << task->summary();
    return dbg.space();
}

#endif
