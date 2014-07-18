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

Controller::Controller(QuickView *quickView,
                       TaskStorage *taskStorage, QObject *parent)
    : QObject(parent)
    , m_currentTaskDuration(0)
    , m_tickTimer(new QTimer(this))
    , m_afterAddingTimer(new QTimer(this))
    , m_elapsedMinutes(0)
    , m_expanded(false)
    , m_indexBeingEdited(-1)
    , m_taskStorage(taskStorage)
    , m_taskStatus(TaskStopped)
    , m_page(TheQueuePage)
    , m_selectedIndex(-1)
    , m_quickView(quickView)
    , m_popupVisible(false)
    , m_editMode(EditModeNone)
    , m_tagEditStatus(TagEditStatusNone)
{
    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(TickInterval);
    connect(m_tickTimer, &QTimer::timeout, this, &Controller::onTimerTick);
    connect(m_afterAddingTimer, &QTimer::timeout, this, &Controller::firstSecondsAfterAddingChanged);
    m_afterAddingTimer->setSingleShot(true);
    m_afterAddingTimer->setInterval(AfterAddingTimeout);

    m_defaultPomodoroDuration = Settings::instance()->value(QStringLiteral("defaultPomodoroDuration"), /*default=*/QVariant(25)).toInt();

    qApp->installEventFilter(this);
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

void Controller::startPomodoro(int queueIndex)
{
    stopPomodoro(/*requeue=*/true); // Stop previous one, if any

    m_currentTask = m_taskStorage->at(queueIndex);
    m_taskStorage->removeTask(queueIndex);

    m_elapsedMinutes = 0;
    m_currentTaskDuration = m_defaultPomodoroDuration;

    setExpanded(false);

    m_tickTimer->start();
    m_afterAddingTimer->start();
    emit firstSecondsAfterAddingChanged();

    setTaskStatus(TaskStarted);
}

void Controller::stopPomodoro(bool reQueueTask)
{
    if (m_taskStatus == TaskStopped) {
        return;
    }

    if (reQueueTask && !m_currentTask->summary().isEmpty()) {
        // Return it to the queue
        addTask(m_currentTask->summary(), false);
    }
    m_tickTimer->stop();
    m_elapsedMinutes = 0;
    m_currentTask.clear();

    setTaskStatus(TaskStopped);
}

void Controller::pausePomodoro()
{
    switch (m_taskStatus) {
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

void Controller::toggleSelectedIndex(int index)
{
    if (m_selectedIndex == index) {
        setSelectedIndex(-1);
    } else {
        setSelectedIndex(index);
    }
}

void Controller::cycleSelectionUp()
{
    if (m_selectedIndex == -1) {
        const int lastIndex = m_taskStorage->taskFilterModel()->count()-1;
        setSelectedIndex(lastIndex);
    } else if (m_selectedIndex > 0){
        setSelectedIndex(m_selectedIndex-1);
    }
}

void Controller::cycleSelectionDown()
{
    const int lastIndex = m_taskStorage->taskFilterModel()->count()-1;
    if (m_selectedIndex == -1 && lastIndex == -1) {
        return;
    }

    if (m_selectedIndex == -1) {
        setSelectedIndex(0);
    } else if (m_selectedIndex < lastIndex){
        setSelectedIndex(m_selectedIndex+1);
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
            editTask(-1, EditModeNone);
        }
        setSelectedIndex(-1);
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

TaskStatus Controller::taskStatus() const
{
    return m_taskStatus;
}

void Controller::setTaskStatus(TaskStatus status)
{
    if (status != m_taskStatus) {
        m_taskStatus = status;
        emit currentTaskChanged();
        emit remainingMinutesChanged();
        emit taskStatusChanged();
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

int Controller::selectedIndex() const
{
    return m_selectedIndex;
}

void Controller::setSelectedIndex(int index)
{
    if (index != m_selectedIndex) {
        m_selectedIndex = index;
        emit selectedIndexChanged();
    }
}

bool Controller::running() const
{
    return m_taskStatus == TaskStarted;
}

bool Controller::stopped() const
{
    return m_taskStatus == TaskStopped;
}

bool Controller::paused() const
{
    return m_taskStatus == TaskPaused;
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
    return m_taskBeingEdited.data();
}

Controller::TagEditStatus Controller::tagEditStatus() const
{
    return m_tagEditStatus;
}

Task *Controller::rightClickedTask() const
{
    return m_rightClickedTask;
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
    return m_currentTask.data();
}

void Controller::onTimerTick()
{
    //qDebug() << "Timer ticked";
    m_elapsedMinutes++;
    emit remainingMinutesChanged();

    if (remainingMinutes() == 0) {
        stopPomodoro(/**requeue=*/true);
        emit taskFinished();
    }
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

    if (m_page != TheQueuePage)
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
            editTask(-1, EditModeNone);
        } else {
            setExpanded(false);
        }
        return true;
        break;

    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (editing) {
            const int index = m_indexBeingEdited;
            editTask(-1, EditModeNone);
            setSelectedIndex(index);
        } else {
            if (m_selectedIndex != -1) {
                startPomodoro(m_selectedIndex);
                setExpanded(false);
            } else {
                setExpanded(true);
            }
        }

        return true;
        break;
    case Qt::Key_Space:
        pausePomodoro();
        return true;
        break;
    case Qt::Key_S:
        stopPomodoro(true);
        return true;
        break;
    case Qt::Key_N:
        setExpanded(true);
        addTask("New Task", /**open editor=*/true);
        return true;
        break;
    case Qt::Key_Delete:
        if (m_selectedIndex == -1) {
            stopPomodoro(false);
        } else {
            removeTask(m_selectedIndex);
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
        if (m_selectedIndex != -1) {
            editTask(m_selectedIndex, EditModeInline);
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

void Controller::editTask(int proxyIndex, Controller::EditMode editMode)
{
    if ((proxyIndex == -1 && editMode != EditModeNone) ||
        (proxyIndex != -1 && editMode == EditModeNone)) {
        // This doesn't happen.
        qWarning() << Q_FUNC_INFO << proxyIndex << editMode;
        Q_ASSERT(false);
        proxyIndex = -1;
        editMode = EditModeNone;
    }

    if (m_editMode != editMode) {
        m_editMode = editMode;
        emit editModeChanged();
    }

    if (m_indexBeingEdited != proxyIndex) {
        m_indexBeingEdited = proxyIndex;

        // Disabling saving when editor is opened, only save when it's closed.
        m_taskStorage->setDisableSaving(proxyIndex != -1);

        if (proxyIndex == -1) {
            m_taskBeingEdited.clear();
            m_taskStorage->saveTasks(); // Editor closed. Write to disk immediately.
        } else {
            m_taskBeingEdited = m_taskStorage->at(proxyIndex).data();
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

void Controller::addTask(const QString &text, bool startEditMode)
{
    m_taskStorage->addTask(text);
    editTask(-1, EditModeNone);

    if (startEditMode) {
        setExpanded(true);
        int lastIndex = m_taskStorage->taskFilterModel()->rowCount()-1;
        editTask(lastIndex, EditModeInline);
    }
}

void Controller::removeTask(int index)
{
    editTask(-1, EditModeNone);
    m_taskStorage->removeTask(index);
}
