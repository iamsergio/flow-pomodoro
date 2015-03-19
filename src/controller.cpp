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
#include "taskcontextmenumodel.h"
#include "taskfilterproxymodel.h"
#include "extendedtagsmodel.h"
#include "sortedtaskcontextmenumodel.h"
#include "storage.h"
#include "kernel.h"
#include "webdavsyncer.h"
#include "utils.h"
#include "loadmanager.h"
#include "flow_version.h"

#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QQmlExpression>
#include <QQmlContext>
#include <QKeyEvent>
#include <qglobal.h>

enum {
    AfterAddingTimeout = 1000,
    TickInterval = 1000*60 // Ticks every minute
};

Controller::Controller(QQmlContext *context, Kernel *kernel, Storage *storage,
                       Settings *settings, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
    , m_currentTaskDuration(0)
    , m_tickTimer(new QTimer(this))
    , m_afterAddingTimer(new QTimer(this))
    , m_elapsedMinutes(0)
    , m_expanded(isMobile())
    , m_page(MainPage)
    , m_popupVisible(false)
    , m_editMode(EditModeNone)
    , m_tagEditStatus(TagEditStatusNone)
    , m_invalidTask(Task::createTask(kernel))
    , m_configureTabIndex(0)
    , m_queueType(QueueTypeToday)
    , m_storage(storage)
    , m_qmlContext(context)
    , m_settings(settings)
    , m_port(80)
    , m_isHttps(false)
    , m_optionsContextMenuVisible(false)
    , m_startupFinished(false)
    , m_newTagDialogVisible(false)
    , m_showPomodoroOverlay(false)
    , m_pomodoroStartTimeStamp(0)
    , m_textRenderType(NativeRendering)
    , m_loadManager(new LoadManager(this))
    , m_addingTask(false)
    , m_currentMenuIndex(-1)
    , m_expertMode(false)
    , m_selectedTaskIndex(-1)
{
    m_tickTimer->setInterval(TickInterval);
    connect(m_tickTimer, &QTimer::timeout, this, &Controller::onTimerTick);
    connect(m_afterAddingTimer, &QTimer::timeout, this, &Controller::firstSecondsAfterAddingChanged);
    m_afterAddingTimer->setSingleShot(true);
    m_afterAddingTimer->setInterval(AfterAddingTimeout);

    m_expanded = isMobile() || !m_settings->stickyWindow();

    connect(m_settings, &Settings::keepScreenOnDuringPomodoroChanged, this, &Controller::onKeepScreenOnDuringPomodoroChanged);
    connect(m_settings, &Settings::pomodoroFunctionalityDisabledChanged, this, &Controller::onPomodoroFunctionalityDisabledChanged);
    connect(m_settings, &Settings::showAllTasksViewChanged, this, &Controller::onShowAllTasksViewChanged);
    connect(m_settings, &Settings::useSystrayChanged, this, &Controller::onUseSystrayChanged);
    connect(m_settings, &Settings::hideEmptyTagsChanged, this, &Controller::onHideEmptyTagsChanged);
    connect(m_settings, &Settings::supportsDueDateChanged, this, &Controller::onSupportsDueDateChanged);

    connect(m_kernel, &Kernel::dayChanged, this, &Controller::currentDateChanged);

    m_host = m_settings->value("webdavHost").toString();
    m_user = m_settings->value("webdavUser").toString();
    m_path = m_settings->value("webdavPath").toString();
    m_password = m_settings->value("webdavPassword").toString();
    m_isHttps = m_settings->value("webdavIsHttps", false).toBool();
    m_port = m_settings->value("webdavPort", 80).toInt();

    m_expertMode = qApp->arguments().contains("--expert");

    connect(this, &Controller::invalidateTaskModel,
            m_storage->taskFilterModel(), &TaskFilterProxyModel::invalidateFilter,
            Qt::QueuedConnection);

    qApp->installEventFilter(this);
    QMetaObject::invokeMethod(this, "setStartupFinished", Qt::QueuedConnection);

    m_untaggedTasksTag = Tag::Ptr(new Tag(tr("Untagged"), m_storage->untaggedTasksModel()));
    m_allTasksTag = Tag::Ptr(new Tag(tr("All"), m_storage->archivedTasksModel()));
    m_dueDateTasksTag = Tag::Ptr(new Tag(tr("with date"), m_storage->dueDateTasksModel()));
    m_currentTag = m_untaggedTasksTag.data();
    updateExtendedTagModel();
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
    setSelectedTask(Task::Ptr());
    t->setLastPomodoroDate(QDateTime::currentDateTimeUtc());
    stopPomodoro(); // Stop previous one, if any

    m_currentTask = task;

    m_elapsedMinutes = 0;
    m_pomodoroStartTimeStamp = QDateTime::currentMSecsSinceEpoch();
    m_currentTaskDuration = m_settings->defaultPomodoroDuration();

    setExpanded(false);

    m_tickTimer->start();
    m_afterAddingTimer->start();
    emit firstSecondsAfterAddingChanged();

    setTaskStatus(TaskStarted);
    emit invalidateTaskModel();
    m_storage->scheduleSave();
}

void Controller::stopPomodoro()
{
    if (currentTask()->stopped())
        return;

    m_tickTimer->stop();
    m_elapsedMinutes = 0;
    m_pomodoroStartTimeStamp = 0;
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

void Controller::validateSelectedTask()
{
    if (!m_selectedTask)
        return;

    // The selected task must exist in the current view
    QAbstractItemModel *model = currentTabTaskModel();
    if (!model) {
        setSelectedTask(Task::Ptr());
        return;
    }

    for (int i = 0; i < model->rowCount(); ++i) {
        Task::Ptr task = model->data(model->index(i, 0), Storage::TaskPtrRole).value<Task::Ptr>();
        if (task.data() == m_selectedTask) // found, lets leae
            return;
    }

    // Selected task is not in our list anymore, clear it
    m_selectedTask.clear();
}

void Controller::cycleTaskSelectionUp()
{
    Task::Ptr firstTask = taskAtCurrentTab(0);
    if (!firstTask || !m_selectedTask || m_selectedTask == firstTask.data()) {
        setSelectedTask(Task::Ptr());
        return;
    }

    validateSelectedTask();

    if (!m_selectedTask) {
        setSelectedTask(firstTask);
    } else if (m_selectedTask != firstTask.data()) {
        int currentIndex = indexOfTaskInCurrentTab(m_selectedTask->toStrongRef());
        setSelectedTask(taskAtCurrentTab(currentIndex - 1));
    }
}

void Controller::cycleTaskSelectionDown()
{
    Task::Ptr firstTask = taskAtCurrentTab(0);
    Task::Ptr lastTask = lastTaskAtCurrentTab();
    if (!lastTask || m_selectedTask == lastTask.data())
        return;

    validateSelectedTask();

    if (!m_selectedTask) {
        setSelectedTask(firstTask);
    } else if (m_selectedTask != lastTask.data()) {
        int currentIndex = indexOfTaskInCurrentTab(m_selectedTask->toStrongRef());
        setSelectedTask(taskAtCurrentTab(currentIndex + 1));
    }
}

void Controller::cycleMenuSelectionUp()
{
    if (m_currentMenuIndex > 0)
        setCurrentMenuIndex(m_currentMenuIndex - 1);
}

void Controller::cycleMenuSelectionDown()
{
    if (!m_rightClickedTask)
        return;

    if (m_currentMenuIndex < m_rightClickedTask->sortedContextMenuModel()->count() - 1)
        setCurrentMenuIndex(m_currentMenuIndex + 1);
}

static int indexOfTag(QAbstractItemModel *model, Tag *tag)
{
    int count = model->rowCount();
    for (int i = 0; i < count; ++i) {
        Tag *t = model->data(model->index(i, 0), Storage::TagRole).value<Tag*>();
        if (t == tag)
            return i;
    }
    return -1;
}

void Controller::cycleTagSelectionLeft()
{
    QAbstractItemModel *m = tagsModel();
    int currentIndex = indexOfTag(m, m_currentTag);
    if (currentIndex > 0)
        setCurrentTag(m->data(m->index(currentIndex - 1, 0), Storage::TagRole).value<Tag*>());
}

void Controller::cycleTagSelectionRight()
{
    QAbstractItemModel *m = tagsModel();
    int currentIndex = indexOfTag(m, m_currentTag);
    if (currentIndex < m->rowCount() - 1)
        setCurrentTag(m->data(m->index(currentIndex + 1, 0), Storage::TagRole).value<Tag*>());
}

void Controller::selectTagByFirstLetter(const QChar &c)
{
    int count = tagsModel()->rowCount();
    for (int i = 0; i < count; ++i) {
        Tag *t = tagsModel()->data(tagsModel()->index(i, 0), Storage::TagRole).value<Tag*>();
        if (t->name().toLower().startsWith(c.toLower()))
            setCurrentTag(t);
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
        setNewTagDialogVisible(false);
        return;
    }

    if (okClicked) {
        // qDebug() << "Running " << m_popupOkCallback << "of" << m_popupCallbackOwner;
        QQmlExpression expr(m_qmlContext, m_popupCallbackOwner, m_popupOkCallback);
        bool valueIsUndefined = false;
        expr.evaluate(&valueIsUndefined);
    }

    setPopupVisible(false);
    setNewTagDialogVisible(false);
}

bool Controller::expanded() const
{
    return m_expanded;
}

void Controller::setExpanded(bool expanded)
{
    if (isMobile() || !m_settings->stickyWindow())
        return;

    if (expanded != m_expanded) {
        m_expanded = expanded;
        if (expanded) {
            emit requestActivateWindow();
        } else {
            editTask(Q_NULLPTR, EditModeNone);
        }
        setSelectedTask(Task::Ptr());
        emit expandedChanged();
        emit currentTitleTextChanged();
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
        if (m_page == ConfigurePage) {
            // If we came from configure, then sync()
            m_settings->scheduleSync();
        }

        if (page == ConfigurePage) {
            m_loadManager->setConfigurePageRequested(true);
        } else if (page == AboutPage) {
            m_loadManager->setAboutPageRequested(true);
        }

        m_page = page;
        emit currentPageChanged();
        emit currentTitleTextChanged();
    }
}

void Controller::setTaskStatus(TaskStatus status)
{
    if (status != currentTask()->status()) {
        currentTask()->setStatus(status);
        emit currentTaskChanged();
        emit remainingMinutesChanged();
        emit currentTaskDurationChanged();
        emit currentTaskChanged();
        emit currentTitleTextChanged();
        Utils::keepScreenOn(m_settings->keepScreenOnDuringPomodoro() && status == TaskStarted);
        setShowPomodoroOverlay(status == TaskStarted);
    }
}

qreal Controller::dpiFactor() const
{
    return Utils::dpiFactor();
}

bool Controller::popupVisible() const
{
    return m_popupVisible;
}

void Controller::setPopupVisible(bool visible)
{
    if (m_popupVisible != visible) {
        m_popupVisible = visible;
        if (visible)
            m_loadManager->setQuestionPopupRequested(true);
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
        setSelectedTaskIndex(indexOfTaskInCurrentTab(task));
        emit selectedTaskChanged();
    }
}

Tag *Controller::currentTag() const
{
    return m_currentTag;
}

Controller::QueueType Controller::queueType() const
{
    return m_queueType;
}

void Controller::setQueueType(QueueType type)
{
    if (type != m_queueType) {
        m_queueType = type;

        if (type == QueueTypeArchive)
            m_loadManager->setArchiveRequested(true);

        emit queueTypeChanged();
        emit currentTitleTextChanged();
    }
}

bool Controller::isMobile() const
{
    return Utils::isMobile();
}

bool Controller::isIOS() const
{
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

bool Controller::isAndroid() const
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool Controller::openSSLSupported() const
{
#ifndef QT_NO_OPENSSL
    return true;
#endif
    return false;
}

bool Controller::hackingMenuSupported() const
{
#ifdef DEVELOPER_MODE
return true;
#endif
return false;
}

bool Controller::isHttps() const
{
    return m_isHttps;
}

void Controller::setIsHttps(bool is)
{
    if (is != m_isHttps) {
        m_isHttps = is;
        m_settings->setValue("webdavIsHttps", m_isHttps);
        emit isHttpsChanged();
        updateWebDavCredentials();
    }
}

QString Controller::host() const
{
    return m_host;
}

void Controller::setHost(const QString &host)
{
    if (host != m_host) {
        m_host = host;
        m_settings->setValue("webdavHost", m_host);
        updateWebDavCredentials();
        emit hostChanged();
    }
}

QString Controller::path() const
{
    return m_path.startsWith("/") ? m_path : ("/" + m_path);
}

void Controller::setPath(const QString &path)
{
    if (path != m_path) {
        m_path = path;
        m_settings->setValue("webdavPath", m_path);
        emit pathChanged();
        updateWebDavCredentials();
    }
}

QString Controller::user() const
{
    return m_user;
}

void Controller::setUser(const QString &user)
{
    if (user != m_user) {
        m_user = user;
        m_settings->setValue("webdavUser", m_user);
        emit userChanged();
        updateWebDavCredentials();
    }
}

QString Controller::password() const
{
    return m_password;
}

void Controller::setPassword(const QString &pass)
{
    if (pass != m_password) {
        m_password = pass;
        m_settings->setValue("webdavPassword", m_password);
        emit passwordChanged();
        updateWebDavCredentials();
    }
}

int Controller::port() const
{
    return m_port;
}

void Controller::setPort(int port)
{
    if (port != m_port) {
        m_port = port;
        m_settings->setValue("webdavPort", m_port);
        emit portChanged();
        updateWebDavCredentials();
    }
}

QString Controller::currentTitleText() const
{    
    if (m_page == ConfigurePage && m_expanded) {
        return tr("Configuration");
    } else if (m_page == AboutPage && m_expanded) {
        return tr("About");
    } else {
        if (!currentTask()->stopped())
            return currentTask()->summary();
        if (!m_expanded)
            return QString();
        return m_queueType == QueueTypeToday ? tr("Today's queue") : tr("Later queue");
    }

}

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

QString Controller::version() const
{
    return STR(FLOW_VERSION);
}

bool Controller::optionsContextMenuVisible() const
{
    return m_optionsContextMenuVisible;
}

void Controller::setOptionsContextMenuVisible(bool visible)
{
    if (visible != m_optionsContextMenuVisible) {
        m_optionsContextMenuVisible = visible;

        if (visible)
            m_loadManager->setConfigurePopupRequested(true);

        emit optionsContextMenuVisibleChanged();
    }
}

bool Controller::newTagDialogVisible() const
{
    return m_newTagDialogVisible;
}

void Controller::setCurrentTag(Tag *tag)
{
    Q_ASSERT(tag);
    if (tag && m_currentTag != tag) {
        if (tag) {
            connect(tag, &Tag::destroyed,
                    this, &Controller::onCurrentTagDestroyed, Qt::UniqueConnection);
        }

        if (m_currentTag) {
            disconnect(m_currentTag.data(), &Tag::destroyed,
                       this, &Controller::onCurrentTagDestroyed);
        }

        m_currentTag = tag;
        emit currentTagChanged();
    }
}

void Controller::setRightClickedTask(Task *task, bool tagOnlyMenu)
{
    //if (m_rightClickedTask != task) { // m_rightClickedTask is a QPointer and task might have been deleted
        setCurrentMenuIndex(-1);
        m_rightClickedTask = task;
        if (task) {
            task->contextMenuModel()->setTagOnlyMenu(tagOnlyMenu);
            m_loadManager->setTaskContextMenuRequested(true);
        }

        emit rightClickedTaskChanged();
    //}
}

void Controller::toggleConfigurePage()
{
    setCurrentPage(m_page == ConfigurePage ? MainPage : ConfigurePage);
}

bool Controller::showPomodoroOverlay() const
{
    return m_showPomodoroOverlay;
}

void Controller::setShowPomodoroOverlay(bool show)
{
    if (show != m_showPomodoroOverlay) {
        m_showPomodoroOverlay = show;
        emit showPomodoroOverlayChanged();
    }
}

void Controller::setNewTagDialogVisible(bool visible)
{
    if (visible != m_newTagDialogVisible) {
        m_newTagDialogVisible = visible;
        emit newTagDialogVisibleChanged();
    }
}

void Controller::setStartupFinished()
{
    m_startupFinished = true;
    emit startupFinishedChanged();
}

void Controller::forceFocusOnTaskBeingEdited()
{
    if (m_editMode == EditModeInline && m_taskBeingEdited)
        emit forceFocus(m_taskBeingEdited);
}

void Controller::openUrl(const QString &url)
{
    Utils::openUrl(QUrl(url));
}

void Controller::updateExtendedTagModel()
{
    Tag::List extraTags;
    if (m_settings->showAllTasksView()) {
        extraTags << m_allTasksTag;
    }

    if (m_settings->supportsDueDate()) {
        extraTags << m_dueDateTasksTag;
    }

    extraTags << m_untaggedTasksTag;
    m_storage->extendedTagsModel()->setExtraRows(extraTags);
}

bool Controller::anyOverlayVisible() const
{
    return taskMenuVisible() || newTagDialogVisible();
}

bool Controller::taskMenuVisible() const
{
    return m_rightClickedTask != 0;
}

void Controller::toggleQueueType()
{
    setQueueType(m_queueType == QueueTypeToday ? QueueTypeArchive : QueueTypeToday);
}

bool Controller::startupFinished() const
{
    return m_startupFinished;
}

Task *Controller::currentTask() const
{
    return m_currentTask ? m_currentTask.data() : m_invalidTask.data();
}

void Controller::onTimerTick()
{
    // qDebug() << "Timer ticked" << "old=" << m_pomodoroStartTimeStamp
    // << "; delta=" << (QDateTime::currentMSecsSinceEpoch() - m_pomodoroStartTimeStamp);
    m_elapsedMinutes = (QDateTime::currentMSecsSinceEpoch() - m_pomodoroStartTimeStamp) / 60000;
    emit remainingMinutesChanged();
    if (remainingMinutes() <= 0) {
        stopPomodoro();
        emit taskFinished();
    }
}

void Controller::onCurrentTagDestroyed()
{
    setCurrentTag(m_untaggedTasksTag.data());
}

void Controller::onKeepScreenOnDuringPomodoroChanged()
{
    Utils::keepScreenOn(m_settings->keepScreenOnDuringPomodoro() && currentTask()->status() == TaskStarted);
}

void Controller::onPomodoroFunctionalityDisabledChanged()
{
    stopPomodoro();
}

void Controller::onShowAllTasksViewChanged()
{
    updateExtendedTagModel();
}

void Controller::onSupportsDueDateChanged()
{
    updateExtendedTagModel();
}

void Controller::onUseSystrayChanged()
{
    if (m_settings->useSystray())
        m_kernel->setupSystray();
    else
        m_kernel->destroySystray();
}

void Controller::onHideEmptyTagsChanged()
{
    if (m_settings->hideEmptyTags() && m_currentTag->taskCount() == 0 && m_currentTag->kernel()) {
        setCurrentTag(m_untaggedTasksTag.data());
    }
    emit tagsModelChanged();
}

void Controller::updateWebDavCredentials()
{
#ifndef NO_WEBDAV
    WebDAVSyncer *webdav = m_kernel->webdavSyncer();
    webdav->setConnectionSettings(m_isHttps, m_port, m_host, m_path, m_user, m_password);
    if (m_syncAtStartup && !webdav->syncedAtStartup()) {
        webdav->sync();
    }
#endif
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
    return model->data(model->index(taskIndex, 0), Storage::TaskPtrRole).value<Task::Ptr>();
}

QAbstractItemModel *Controller::currentTabTaskModel() const
{
    if (m_queueType == QueueTypeToday)
        return m_storage->stagedTasksModel();

    return m_currentTag ? m_currentTag->taskModel() : Q_NULLPTR;
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

bool Controller::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return false;

    if (!object->inherits("QWindow")) // Filters out duplicated events
        return false;

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Back) {
        if (m_page == MainPage) {
            if (m_queueType != QueueTypeToday) {
                setQueueType(QueueTypeToday);
            } else if (isAndroid()) {
                qApp->quit();
            }
        } else {
            setCurrentPage(MainPage);
        }
        setSelectedTask(Task::Ptr());
        editTask(Q_NULLPTR, EditModeNone);
        return true;
    }

    const bool editing = !m_taskBeingEdited.isNull();
    const bool escKeyPressed = keyEvent->key() == Qt::Key_Escape;

    if (escKeyPressed) {
        if (editing) {
            editTask(Q_NULLPTR, EditModeNone);
        } else if (newTagDialogVisible()) {
            setNewTagDialogVisible(false);
        } else if (m_rightClickedTask) {
            setRightClickedTask(0);
        } else if (m_tagBeingEdited) {
            setTagEditStatus(TagEditStatusNone);
        } else if (m_page != MainPage) {
            setCurrentPage(MainPage);
        } else {
            setExpanded(false);
        }
        return true;
    }

    if (m_page != MainPage)
        return false;

    if (m_editMode == EditModeEditor)
        return false;

    if (editing)
        return false;

    const bool enterKeyPressed = keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return;

    if (enterKeyPressed) {
        if (newTagDialogVisible())
            return false;

        if (taskMenuVisible()) {
            // Too many situations where we can loose focus, so lets not use QML keyboard handling
            if (m_currentMenuIndex == -1) {
                setRightClickedTask(Q_NULLPTR);
            } else {
                emit enterPressed();
            }

            return true;
        }
    }

    const bool archiveVisible = m_queueType == QueueTypeArchive && m_page == MainPage;

    if (!anyOverlayVisible()) {
       switch (keyEvent->key()) {
       case Qt::Key_Return:
       case Qt::Key_Enter:
           if (m_selectedTask) {
               startPomodoro(m_selectedTask);
               setExpanded(false);
           } else {
               setExpanded(true);
           }

           return true;
           break;
       case Qt::Key_S:
           stopPomodoro();
           return true;
           break;
       case Qt::Key_Tab:
           if (m_page == MainPage && keyEvent->modifiers() & Qt::ControlModifier) {
               toggleQueueType();
               return true;
           }

           return false;
           break;
       case Qt::Key_N:
           setExpanded(true);
           addTask("New Task", /**open editor=*/true); // Detect on which tab we're on and tag it properly
           return true;
           break;
       case Qt::Key_Delete:
           if (m_selectedTask == Q_NULLPTR) {
               stopPomodoro();
           } else {
               removeTask(m_selectedTask);
           }
           return true;
           break;
       case Qt::Key_F2:
       case Qt::Key_E:
           if (m_selectedTask) {
               requestContextMenu(m_selectedTask);
               return true;
           }
           return false;
           break;
       case Qt::Key_Left:
           if (m_queueType == QueueTypeArchive && m_page == MainPage) {
               cycleTagSelectionLeft();
               return true;
           } else {
               return false;
           }
       case Qt::Key_Right:
           if (archiveVisible) {
               cycleTagSelectionRight();
               return true;
           } else {
               return false;
           }
       }

       if (archiveVisible) {
           QString text = keyEvent->text();
           if (!text.isEmpty()) {
               selectTagByFirstLetter(text.at(0));
           }
       }
    }

    switch (keyEvent->key()) {
    case Qt::Key_Space:
        if (taskMenuVisible() && m_currentMenuIndex != -1) {
            m_rightClickedTask->sortedContextMenuModel()->toggleTag(m_currentMenuIndex);
            return true;
        } else if (!anyOverlayVisible()) {
            pausePomodoro();
            return true;
        } else {
            return false;
        }
        break;
    case Qt::Key_Up:
        if (taskMenuVisible())
            cycleMenuSelectionUp();
        else
            cycleTaskSelectionUp();
        return true;
        break;
    case Qt::Key_Down:
        if (taskMenuVisible())
            cycleMenuSelectionDown();
        else
            cycleTaskSelectionDown();
        return true;
        break;
    case Qt::Key_Tab:
        if (taskMenuVisible() && !newTagDialogVisible())
            cycleMenuSelectionDown();
        return true;
        break;
    case Qt::Key_Backtab:
        if (taskMenuVisible() && !newTagDialogVisible())
            cycleMenuSelectionUp();
        return true;
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

    Tag::Ptr tag = m_storage->tag(tagName, /*create=*/false);
    if (!tag) {
        qWarning() << Q_FUNC_INFO << "Could not find tag to edit:" << tagName;
        return;
    }

    tag->setBeingEdited(true);
    m_tagBeingEdited = tag;
}

bool Controller::renameTag(const QString &oldName, const QString &newName)
{
    bool success = m_storage->renameTag(oldName, newName);
    if (success && m_tagBeingEdited) {
        m_tagBeingEdited->setBeingEdited(false);
        m_tagBeingEdited.clear();
    }

    return success;
}

void Controller::editTask(Task *t, Controller::EditMode editMode)
{
    if (!t && !m_taskBeingEdited)
        return;

    if (editMode == EditModeEditor && !m_settings->supportsDueDate())
        editMode = EditModeInline;

    Task::Ptr task = t ? t->toStrongRef() : Task::Ptr();
    if ((!task && editMode != EditModeNone) ||
        (task && editMode == EditModeNone)) {
        // This doesn't happen.
        qWarning() << Q_FUNC_INFO << t << editMode;
        Q_ASSERT(false);
        task = Task::Ptr();
        editMode = EditModeNone;
    }

    if (editMode == EditModeInline) {
        m_loadManager->setInlineEditorRequested(true); // for delayed loading
    }

    if (m_editMode != editMode) {
        m_editMode = editMode;
        emit editModeChanged();
    }

    if (task == m_invalidTask) {
        Q_ASSERT(false);
        return;
    }

    setSelectedTask(Task::Ptr());

    if (m_taskBeingEdited == task.data()) {
        // Nothing do to
        return;
    }

    if (task.isNull()) {
        if (m_taskBeingEdited && m_taskBeingEdited->summary().isEmpty()) {
            // Empty summaries are not allowed !
            m_taskBeingEdited->setSummary(tr("New Task"));
        }

        Task *previousTask = m_taskBeingEdited.data();
        m_taskBeingEdited.clear(); // clear before calling requestContextMenu, due to re-entrancy of editTask(nullptr) being called by requestContextMenu()

        if (m_addingTask && previousTask) {
            m_addingTask = false;
            QString summary = previousTask->summary();
            if (m_queueType == QueueTypeToday) {
                Tag::Ptr tag = tagForSummary(/*by-ref*/summary); // transforms "books: foo" into "foo" + tag books
                if (tag) {
                    previousTask->addTag(tag->name());
                    previousTask->setSummary(summary);
                } else if (m_settings->showContextMenuAfterAdd() && !m_storage->tasks().isEmpty()) {
                    // It's a new task, lets popup the context menu to choose tags
                    requestContextMenu(previousTask, /*tagOnlyMenu=*/ true);
                }
            }
        }
    } else {
        m_taskBeingEdited = task.data();
    }

    emit taskBeingEditedChanged();
}

void Controller::dismissTaskEditor()
{
    emit taskEditorDismissed();
}

void Controller::endAddingNewTag(const QString &tagName)
{
    if (tagName.isEmpty()) {
        // Just close the editor. Don't add
        setTagEditStatus(TagEditStatusNone);
        return;
    }

    if (m_storage->createTag(tagName))
        setTagEditStatus(TagEditStatusNone);
}

void Controller::requestContextMenu(Task *task, bool tagOnlyMenu)
{
    editTask(Q_NULLPTR, EditModeNone);
    setRightClickedTask(task, tagOnlyMenu);
}

Tag::Ptr Controller::tagForSummary(QString &summary_inout) const
{
    QStringList splitted = summary_inout.split(":");
    if (splitted.count() < 2 || splitted.first().contains(" "))
        return Tag::Ptr();

    Tag::Ptr tag = m_storage->tag(splitted.first(), /*create=*/false);
    if (tag) {
        splitted.removeAt(0);
        summary_inout = splitted.join("").trimmed();
    }

    return tag;
}

void Controller::addTask(const QString &text, bool startEditMode)
{
    m_addingTask = false;
    emit aboutToAddTask();
    Task::Ptr task = m_storage->prependTask(text);

    if (m_currentTag && queueType() == QueueTypeArchive) {
        if (!m_currentTag->isFake())
            task->addTag(m_currentTag->name());
        else if (m_currentTag.data() == m_dueDateTasksTag && !task->dueDate().isValid())
            task->setDueDate(QDate::currentDate());
    }

    task->setStaged(m_queueType == QueueTypeToday);
    editTask(Q_NULLPTR, EditModeNone);

    if (startEditMode) {
        setExpanded(true);
        editTask(task.data(), EditModeInline);
        forceFocusOnTaskBeingEdited();
        emit addingNewTask();
        m_addingTask = true;
    }
}

void Controller::removeTask(Task *task)
{
    qDebug() << "Removing task" << task->summary();
    editTask(Q_NULLPTR, EditModeNone);
    m_storage->removeTask(task->toStrongRef());
}

void Controller::webDavSync()
{
#ifndef NO_WEBDAV
    m_kernel->webdavSyncer()->sync();
#else
    qDebug() << "WebDAV sync not supported";
#endif
}

void Controller::setTextRenderType(int textRenderType)
{
    if (textRenderType != m_textRenderType) {
        m_textRenderType = textRenderType;
        emit textRenderTypeChanged();
    }
}

int Controller::textRenderType() const
{
    return m_textRenderType;
}

LoadManager* Controller::loadManager() const
{
    return m_loadManager;
}

QString Controller::gitDate() const
{
    return STR(FLOW_VERSION_DATE);
}

QString Controller::qtVersion() const
{
    return qVersion();
}

bool Controller::isOSX() const
{
    return Utils::isOSX();
}

void Controller::setCurrentMenuIndex(int currentMenuIndex)
{
    if (currentMenuIndex != m_currentMenuIndex) {
        m_currentMenuIndex = currentMenuIndex;
        emit currentMenuIndexChanged();
    }
}

int Controller::currentMenuIndex() const
{
    return m_currentMenuIndex;
}

QString Controller::buildOptionsText() const
{
    QStringList options;
    options << (isMobile() ? "mobile" : "desktop");

    if (m_storage->webDAVSyncSupported()) {
        options << "webdav";
        options << (openSSLSupported() ? "openssl" : "no-openssl");
    } else {
        options << "no-webdav";
    }

    if (hackingMenuSupported())
        options << "hacking";

    QString text = tr("Build options") + ": ";
    for (int i = 0; i < options.count(); ++i) {
        if (i > 0)
            text += ", ";
        text += options.at(i);
    }

    return text;
}

bool Controller::expertMode() const
{
    return m_expertMode;
}

Tag* Controller::allTasksTag() const
{
    return m_allTasksTag.data();
}

Tag* Controller::untaggedTasksTag() const
{
    return m_untaggedTasksTag.data();
}

Tag *Controller::dueDateTasksTag() const
{
    return m_dueDateTasksTag.data();
}

QAbstractItemModel* Controller::tagsModel() const
{
    return m_settings->hideEmptyTags() ? m_storage->nonEmptyTagsModel()
                                       : m_storage->extendedTagsModel();
}

void Controller::setSelectedTaskIndex(int selectedTaskIndex)
{
    if (selectedTaskIndex != m_selectedTaskIndex) {
        m_selectedTaskIndex = selectedTaskIndex;
        emit selectedTaskIndexChanged();
    }
}

int Controller::selectedTaskIndex() const
{
    return m_selectedTaskIndex;
}

QDate Controller::currentDate() const
{
    return m_kernel->currentDate();
}

void Controller::dismissTaskMenu()
{
    setRightClickedTask(0);
}

void Controller::dismissTaskMenuDelayed()
{
    // No technical reason for calling it delayed. Just looks better when using the UI.
    QTimer::singleShot(200, this, SLOT(dismissTaskMenu()));
}
