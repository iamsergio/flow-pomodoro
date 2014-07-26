/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

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

#include "controller.h"
#include "settings.h"
#include "quickview.h"
#include "taskstorage.h"
#include "taskfilterproxymodel.h"
#include "tagstorage.h"

#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QQmlExpression>

enum {
    AfterAddingTimeout = 1000,
    TickInterval = 1000*60 // Ticks every minute
};

Controller::Controller(QuickView *quickView)
    : QObject(quickView)
    , m_currentTaskDuration(0)
    , m_tickTimer(new QTimer(this))
    , m_afterAddingTimer(new QTimer(this))
    , m_elapsedMinutes(0)
    , m_expanded(false)
    , m_indexBeingEdited(-1)
    , m_taskStorage(TaskStorage::instance())
    , m_page(MainPage)
    , m_quickView(quickView)
    , m_popupVisible(false)
    , m_editMode(EditModeNone)
    , m_tagEditStatus(TagEditStatusNone)
    , m_invalidTask(Task::createTask())
    , m_configureTabIndex(0)
    , m_queueType(QueueTypeToday)
{
    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(TickInterval);
    connect(m_tickTimer, &QTimer::timeout, this, &Controller::onTimerTick);
    connect(m_afterAddingTimer, &QTimer::timeout, this, &Controller::firstSecondsAfterAddingChanged);
    m_afterAddingTimer->setSingleShot(true);
    m_afterAddingTimer->setInterval(AfterAddingTimeout);

    m_defaultPomodoroDuration = Settings::instance()->value(QStringLiteral("defaultPomodoroDuration"), /*default=*/QVariant(25)).toInt();

    connect(this, &Controller::invalidateTaskModel,
            m_taskStorage->taskFilterModel(), &TaskFilterProxyModel::invalidateFilter,
            Qt::QueuedConnection);

    qApp->installEventFilter(this);
}

Controller::~Controller()
{
}

int Controller::remainingMinutes() const
{
    return m_currentTaskDuration - m_elapsedMinutes;
}

int Controller::currentTaskDuration() const
{
    return m_currentTaskDuration;
}

int Controller::indexBeingEdited() const
{
    return m_indexBeingEdited;
}

Controller::EditMode Controller::editMode() const
{
    return m_editMode;
}

void Controller::startPomodoro(Task *t)
{
    if (!t || t == m_invalidTask) {
        Q_ASSERT(false);
        return;
    }

    Task::Ptr task = t->toStrongRef();

    stopPomodoro(); // Stop previous one, if any

    m_currentTask = task;

    m_elapsedMinutes = 0;
    m_currentTaskDuration = m_defaultPomodoroDuration;

    setExpanded(false);

    m_tickTimer->start();
    m_afterAddingTimer->start();
    emit firstSecondsAfterAddingChanged();

    setTaskStatus(TaskStarted);
    emit invalidateTaskModel();
}

void Controller::stopPomodoro()
{
    if (currentTask()->stopped())
        return;

    m_tickTimer->stop();
    m_elapsedMinutes = 0;
    setTaskStatus(TaskStopped);
    m_currentTask.clear();
    emit invalidateTaskModel();
}

void Controller::pausePomodoro()
{
    switch (currentTask()->status()) {
    case TaskPaused:
        m_tickTimer->start();
        setTaskStatus(TaskStarted);
        break;
    case TaskStarted:
        m_tickTimer->stop();
        setTaskStatus(TaskPaused);
        break;
    default:
        break;
    }
}

void Controller::toggleSelectedTask(Task *task)
{
    setSelectedTask(m_selectedTask == task ? Task::Ptr() : task->toStrongRef());
}

void Controller::cycleSelectionUp()
{
    Task::Ptr firstTask = taskAtCurrentTab(0);
    if (!firstTask)
        return;

    if (m_currentTabTag && !m_selectedTask->containsTag(m_currentTabTag->name()))
        m_selectedTask.clear();

    if (!m_selectedTask) {
        setSelectedTask(firstTask);
    } else if (m_selectedTask != firstTask.data()) {
        int currentIndex = indexOfTaskInCurrentTab(m_selectedTask->toStrongRef());
        setSelectedTask(taskAtCurrentTab(currentIndex - 1));
    }
}

void Controller::cycleSelectionDown()
{
    Task::Ptr firstTask = taskAtCurrentTab(0);
    Task::Ptr lastTask = lastTaskAtCurrentTab();
    if (!lastTask || m_selectedTask == lastTask.data())
        return;

    if (m_currentTabTag && !m_selectedTask->containsTag(m_currentTabTag->name()))
        m_selectedTask.clear();

    if (!m_selectedTask) {
        setSelectedTask(firstTask);
    } else if (m_selectedTask != lastTask.data()) {
        int currentIndex = indexOfTaskInCurrentTab(m_selectedTask->toStrongRef());
        setSelectedTask(taskAtCurrentTab(currentIndex + 1));
    }
}

void Controller::showQuestionPopup(QObject *obj, const QString &text, const QString &callback)
{
    if (!obj || callback.isEmpty() || text.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    m_popupCallbackOwner = obj;
    m_popupOkCallback = callback;

    setPopupText(text);
    setPopupVisible(true);
}

void Controller::onPopupButtonClicked(bool okClicked)
{
    if (!m_popupCallbackOwner) {
        qWarning() << "Null callback owner." << m_popupOkCallback;
        setPopupVisible(false);
        return;
    }

    if (okClicked) {
        // qDebug() << "Running " << m_popupOkCallback << "of" << m_popupCallbackOwner;
        QQmlExpression expr(m_quickView->rootContext(), m_popupCallbackOwner, m_popupOkCallback);
        bool valueIsUndefined = false;
        expr.evaluate(&valueIsUndefined);
    }

    setPopupVisible(false);
}

bool Controller::expanded() const
{
    return m_expanded;
}

void Controller::setExpanded(bool expanded)
{
    if (expanded != m_expanded) {
        m_expanded = expanded;
        if (expanded) {
            m_quickView->requestActivate();
        } else {
            editTask(nullptr, EditModeNone);
        }
        setSelectedTask(Task::Ptr());
        emit expandedChanged();
    }
}

bool Controller::firstSecondsAfterAdding() const
{
    return m_afterAddingTimer->isActive();
}

Controller::Page Controller::currentPage() const
{
    return m_page;
}

void Controller::setCurrentPage(Controller::Page page)
{
    if (page != m_page) {
        m_page = page;
        emit currentPageChanged();
    }
}

void Controller::setTaskStatus(TaskStatus status)
{
    if (status != currentTask()->status()) {
        currentTask()->setStatus(status);
        emit currentTaskChanged();
        emit remainingMinutesChanged();
        emit currentTaskDurationChanged();
    }
}

void Controller::setDefaultPomodoroDuration(int duration)
{
    if (m_defaultPomodoroDuration != duration && duration > 0 && duration < 59) {
        m_defaultPomodoroDuration = duration;
        Settings::instance()->setValue("defaultPomodoroDuration", QVariant(duration));
        emit defaultPomodoroDurationChanged();
    }
}

int Controller::defaultPomodoroDuration() const
{
    return m_defaultPomodoroDuration;
}

qreal Controller::dpiFactor() const
{
    QScreen *screen = QGuiApplication::primaryScreen();
    Q_ASSERT(screen);

    return screen->logicalDotsPerInch() / 96.0;
}

bool Controller::popupVisible() const
{
    return m_popupVisible;
}

void Controller::setPopupVisible(bool visible)
{
    if (m_popupVisible != visible) {
        m_popupVisible = visible;
        emit popupVisibleChanged();
    }
}

QString Controller::popupText() const
{
    return m_popupText;
}

void Controller::setPopupText(const QString &text)
{
    if (m_popupText != text) {
        m_popupText = text;
        emit popupTextChanged();
    }
}

Task *Controller::taskBeingEdited() const
{
    return m_taskBeingEdited.data() ? m_taskBeingEdited.data() : m_invalidTask.data();
}

Controller::TagEditStatus Controller::tagEditStatus() const
{
    return m_tagEditStatus;
}

Task *Controller::rightClickedTask() const
{
    return m_rightClickedTask;
}

int Controller::configureTabIndex() const
{
    return m_configureTabIndex;
}

void Controller::setConfigureTabIndex(int index)
{
    if (m_configureTabIndex != index) {
        m_configureTabIndex = index;
        emit configureTabIndexChanged();
    }
}

Task *Controller::selectedTask() const
{
    return m_selectedTask;
}

void Controller::setSelectedTask(const Task::Ptr &task)
{
    if (m_selectedTask.data() != task) {
        m_selectedTask = task.data();
        emit selectedTaskChanged();
    }
}

Tag *Controller::currentTabTag() const
{
    return m_currentTabTag;
}

Controller::QueueType Controller::queueType() const
{
    return m_queueType;
}

void Controller::setQueueType(QueueType type)
{
    if (type != m_queueType) {
        m_queueType = type;
        emit queueTypeChanged();
    }
}

void Controller::setCurrentTabTag(Tag *tag)
{
    if (m_currentTabTag != tag) {
        m_currentTabTag = tag;
        emit currentTabTagChanged();
    }
}

void Controller::setRightClickedTask(Task *task)
{
    // We don't have a way to detect when the context menu is closed (QtQuick.Controls bug?)
    // So comment the guards so we can click on the same task twice.
    //if (m_rightClickedTask != task) {
        m_rightClickedTask = task;
        emit rightClickedTaskChanged();
    //}
}

Task *Controller::currentTask() const
{
    return m_currentTask ? m_currentTask.data() : m_invalidTask.data();
}

void Controller::onTimerTick()
{
    //qDebug() << "Timer ticked";
    m_elapsedMinutes++;
    emit remainingMinutesChanged();

    if (remainingMinutes() == 0) {
        stopPomodoro();
        emit taskFinished();
    }
}

int Controller::indexOfTaskInCurrentTab(const Task::Ptr &task)
{
    QAbstractItemModel *model = currentTabTaskModel();
    int count = model->rowCount();
    for (int i = 0; i < count; ++i) {
        if (task == taskAtCurrentTab(i))
            return i;
    }

    return -1;
}

Task::Ptr Controller::lastTaskAtCurrentTab() const
{
    QAbstractItemModel *model = currentTabTaskModel();
    int lastIndex = model->rowCount() - 1;
    return lastIndex == -1 ? Task::Ptr() : taskAtCurrentTab(lastIndex);
}

Task::Ptr Controller::taskAtCurrentTab(int taskIndex) const
{
    QAbstractItemModel *model = currentTabTaskModel();
    return model->data(model->index(taskIndex, 0), TaskStorage::TaskPtrRole).value<Task::Ptr>();
}

QAbstractItemModel *Controller::currentTabTaskModel() const
{
    return m_currentTabTag ? m_currentTabTag->taskModel() : m_taskStorage->taskFilterModel();
}

void Controller::setTagEditStatus(TagEditStatus status)
{
    if (status != m_tagEditStatus) {
        if (status != TagEditStatusEdit && m_tagBeingEdited) {
            m_tagBeingEdited->setBeingEdited(false);
            m_tagBeingEdited.clear();
        }

        m_tagEditStatus = status;
        emit tagEditStatusChanged();
    }
}

bool Controller::eventFilter(QObject *, QEvent *event)
{
    if (event->type() != QEvent::KeyRelease)
        return false;

    if (m_page != MainPage)
        return false;

    if (m_editMode == EditModeEditor)
        return false;

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

    const bool editing = m_indexBeingEdited != -1;

    if (editing && (keyEvent->key() != Qt::Key_Escape &&
                    keyEvent->key() != Qt::Key_Enter &&
                    keyEvent->key() != Qt::Key_Return)) {
        return false;
    }

    switch (keyEvent->key()) {
    case Qt::Key_Escape:
        if (editing) {
            editTask(nullptr, EditModeNone);
        } else {
            setExpanded(false);
        }
        return true;
        break;

    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (editing) {
            editTask(nullptr, EditModeNone);
            setSelectedTask(m_taskStorage->at(m_indexBeingEdited));
        } else {
            if (m_selectedTask == nullptr) {
                setExpanded(true);
            } else {
                startPomodoro(m_selectedTask);
                setExpanded(false);
            }
        }

        return true;
        break;
    case Qt::Key_Space:
        pausePomodoro();
        return true;
        break;
    case Qt::Key_S:
        stopPomodoro();
        return true;
        break;
    case Qt::Key_N:
        setExpanded(true);
        addTask("New Task", nullptr, /**open editor=*/true); // Detect on which tab we're on and tag it properly
        return true;
        break;
    case Qt::Key_Delete:
        if (m_selectedTask == nullptr) {
            stopPomodoro();
        } else {
            removeTask(m_selectedTask);
        }
        return true;
        break;
    case Qt::Key_Up:
        cycleSelectionUp();
        return true;
        break;
    case Qt::Key_Down:
        cycleSelectionDown();
        return true;
        break;
    case Qt::Key_F2:
    case Qt::Key_E:
        if (m_selectedTask) {
            editTask(m_selectedTask, EditModeInline);
            return true;
        }
        return false;
        break;
    default:
        break;
    }

    return false;
}

void Controller::editTag(const QString &tagName)
{
    if (m_tagBeingEdited) {
        m_tagBeingEdited->setBeingEdited(false);
        m_tagBeingEdited.clear();
    }

    if (tagName.isEmpty()) {
        setTagEditStatus(TagEditStatusNone);
        return;
    }

    setTagEditStatus(TagEditStatusEdit);

    Tag::Ptr tag = TagStorage::instance()->tag(tagName, /*create=*/false);
    if (!tag) {
        qWarning() << Q_FUNC_INFO << "Could not find tag to edit:" << tagName;
        return;
    }

    tag->setBeingEdited(true);
    m_tagBeingEdited = tag;
}

bool Controller::renameTag(const QString &oldName, const QString &newName)
{
    bool success = TagStorage::instance()->renameTag(oldName, newName);
    if (success && m_tagBeingEdited) {
        m_tagBeingEdited->setBeingEdited(false);
        m_tagBeingEdited.clear();
    }

    return success;
}

void Controller::editTask(Task *t, Controller::EditMode editMode)
{
    Task::Ptr task = t ? t->toStrongRef() : Task::Ptr();
    if ((!task && editMode != EditModeNone) ||
        (task && editMode == EditModeNone)) {
        // This doesn't happen.
        qWarning() << Q_FUNC_INFO << t << editMode;
        Q_ASSERT(false);
        task = Task::Ptr();
        editMode = EditModeNone;
    }

    if (m_editMode != editMode) {
        m_editMode = editMode;
        emit editModeChanged();
    }

    if (task == m_invalidTask) {
        Q_ASSERT(false);
        return;
    }

    if (m_taskBeingEdited != task.data()) {
        m_indexBeingEdited = m_taskStorage->indexOf(task);

        // Disabling saving when editor is opened, only save when it's closed.
        m_taskStorage->setDisableSaving(!task.isNull());

        if (task.isNull()) {
            m_taskBeingEdited.clear();
            m_taskStorage->saveTasks(); // Editor closed. Write to disk immediately.
        } else {
            m_taskBeingEdited = task.data();
        }
        emit indexBeingEditedChanged();
    }
}

void Controller::beginAddingNewTag()
{
    setTagEditStatus(TagEditStatusNew);
}

void Controller::endAddingNewTag(const QString &tagName)
{
    if (tagName.isEmpty()) {
        // Just close the editor. Don't add
        setTagEditStatus(TagEditStatusNone);
        return;
    }

    if (TagStorage::instance()->createTag(tagName))
        setTagEditStatus(TagEditStatusNone);
}

void Controller::requestContextMenu(Task *task)
{
    setRightClickedTask(task);
}

void Controller::addTask(const QString &text, Tag *tag, bool startEditMode)
{
    Task::Ptr task = m_taskStorage->addTask(text);
    if (tag)
        task->addTag(tag->name());

    task->setStaged(m_queueType == QueueTypeToday);
    editTask(nullptr, EditModeNone);

    if (startEditMode) {
        setExpanded(true);
        int lastIndex = m_taskStorage->taskFilterModel()->rowCount()-1;
        editTask(m_taskStorage->at(lastIndex).data(), EditModeInline);
        emit forceFocus(lastIndex);
    }
}

void Controller::removeTask(Task *task)
{
    qDebug() << "Removing task" << task->summary();
    editTask(nullptr, EditModeNone);
    m_taskStorage->removeTask(m_taskStorage->indexOf(task->toStrongRef()));
}
