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
#include "taskfilterproxymodel.h"
#include "storage.h"
#include "kernel.h"
#include "webdavsyncer.h"
#include "utils.h"
#include "loadmanager.h"

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
    , m_pomodoroFunctionalityDisabled(false)
    , m_qmlContext(context)
    , m_settings(settings)
    , m_port(80)
    , m_isHttps(false)
    , m_syncAtStartup(false)
    , m_optionsContextMenuVisible(false)
    , m_startupFinished(false)
    , m_newTagDialogVisible(false)
    , m_keepScreenOnDuringPomodoro(false)
    , m_showPomodoroOverlay(false)
    , m_pomodoroStartTimeStamp(0)
    , m_textRenderType(NativeRendering)
    , m_loadManager(new LoadManager(this))
    , m_hideEmptyTags(false)
    , m_useSystray(false)
    , m_stickyWindow(true)
{
    m_tickTimer->setInterval(TickInterval);
    connect(m_tickTimer, &QTimer::timeout, this, &Controller::onTimerTick);
    connect(m_afterAddingTimer, &QTimer::timeout, this, &Controller::firstSecondsAfterAddingChanged);
    m_afterAddingTimer->setSingleShot(true);
    m_afterAddingTimer->setInterval(AfterAddingTimeout);

    m_defaultPomodoroDuration = m_settings->value("defaultPomodoroDuration", /*default=*/ QVariant(25)).toInt();
    m_pomodoroFunctionalityDisabled = m_settings->value("pomodoroFunctionalityDisabled", /*default=*/ false).toBool();
    m_syncAtStartup = m_settings->value("syncAtStartup", /*default=*/ false).toBool();
    setKeepScreenOnDuringPomodoro(m_settings->value("keepScreenOnDuringPomodoro", /*default=*/ true).toInt());
    m_hideEmptyTags = m_settings->value("hideEmptyTags", /*default=*/ false).toBool();
    m_useSystray = m_settings->value("useSystray", /*default=*/ true).toBool();
    m_stickyWindow = m_settings->value("stickyWindow", /*default=*/ true).toBool();

    m_expanded = isMobile() || !m_stickyWindow;

    m_host = m_settings->value("webdavHost").toString();
    m_user = m_settings->value("webdavUser").toString();
    m_path = m_settings->value("webdavPath").toString();
    m_password = m_settings->value("webdavPassword").toString();
    m_isHttps = m_settings->value("webdavIsHttps", false).toBool();
    m_port = m_settings->value("webdavPort", 80).toInt();

    connect(this, &Controller::invalidateTaskModel,
            m_storage->taskFilterModel(), &TaskFilterProxyModel::invalidateFilter,
            Qt::QueuedConnection);

    qApp->installEventFilter(this);
    QMetaObject::invokeMethod(this, "setStartupFinished", Qt::QueuedConnection);
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

    stopPomodoro(); // Stop previous one, if any

    m_currentTask = task;

    m_elapsedMinutes = 0;
    m_pomodoroStartTimeStamp = QDateTime::currentMSecsSinceEpoch();
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

void Controller::toggleSelectedTask(Task *task)
{
    setSelectedTask(m_selectedTask == task ? Task::Ptr() : task->toStrongRef());
}

void Controller::cycleSelectionUp()
{
    Task::Ptr firstTask = taskAtCurrentTab(0);
    if (!firstTask)
        return;

    if (m_currentTabTag && m_selectedTask && !m_selectedTask->containsTag(m_currentTabTag->name()))
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

    if (m_currentTabTag && m_selectedTask && !m_selectedTask->containsTag(m_currentTabTag->name()))
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
    if (isMobile() || !m_stickyWindow)
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
        Utils::keepScreenOn(m_keepScreenOnDuringPomodoro && status == TaskStarted);
        setShowPomodoroOverlay(status == TaskStarted);
    }
}

void Controller::setDefaultPomodoroDuration(int duration)
{
    if (m_defaultPomodoroDuration != duration && duration > 0 && duration < 59) {
        m_defaultPomodoroDuration = duration;
        m_settings->setValue("defaultPomodoroDuration", QVariant(duration));
        emit defaultPomodoroDurationChanged();
    }
}

int Controller::defaultPomodoroDuration() const
{
    return m_defaultPomodoroDuration;
}

void Controller::setPomodoroFunctionalityDisabled(bool disable)
{
    if (disable != m_pomodoroFunctionalityDisabled) {
        m_pomodoroFunctionalityDisabled = disable;
        m_settings->setValue("pomodoroFunctionalityDisabled", QVariant(disable));
        stopPomodoro();
        emit pomodoroFunctionalityDisabledChanged();
    }
}

bool Controller::pomodoroFunctionalityDisabled() const
{
    return m_pomodoroFunctionalityDisabled;
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

void Controller::setCurrentTabTag(Tag *tag)
{
    if (m_currentTabTag != tag) {
        if (tag) {
            connect(tag, &Tag::destroyed,
                    this, &Controller::currentTabTagChanged, Qt::UniqueConnection);
        }

        if (m_currentTabTag) {
            disconnect(m_currentTabTag.data(), &Tag::destroyed,
                       this, &Controller::currentTabTagChanged);
        }

        m_currentTabTag = tag;
        emit currentTabTagChanged();
    }
}

void Controller::setRightClickedTask(Task *task)
{
    //if (m_rightClickedTask != task) { // m_rightClickedTask is a QPointer and task might have been deleted
        m_rightClickedTask = task;
        if (task)
            m_loadManager->setTaskContextMenuRequested(true);

        emit rightClickedTaskChanged();
    //}
}

void Controller::toggleConfigurePage()
{
    setCurrentPage(m_page == ConfigurePage ? MainPage : ConfigurePage);
}

void Controller::setKeepScreenOnDuringPomodoro(bool keep)
{
    if (keep != m_keepScreenOnDuringPomodoro) {
        m_keepScreenOnDuringPomodoro = keep;
        m_settings->setValue("keepScreenOnDuringPomodoro", keep);
        emit keepScreenOnDuringPomodoroChanged();
        Utils::keepScreenOn(m_keepScreenOnDuringPomodoro && currentTask()->status() == TaskStarted);
    }
}

bool Controller::keepScreenOnDuringPomodoro() const
{
    return m_keepScreenOnDuringPomodoro;
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

bool Controller::syncAtStartup() const
{
    return m_syncAtStartup;
}

void Controller::setSyncAtStartup(bool sync)
{
    if (m_syncAtStartup != sync) {
        m_syncAtStartup = sync;
        m_settings->setValue("syncAtStartup", sync);
        emit syncAtStartupChanged();
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
    return m_currentTabTag ? m_currentTabTag->taskModel() : m_storage->taskFilterModel();
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

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Back) {
        if (m_page == MainPage) {
            setQueueType(QueueTypeToday);
        } else {
            setCurrentPage(MainPage);
        }
        setSelectedTask(Task::Ptr());
        editTask(Q_NULLPTR, EditModeNone);
        return true;
    }

    const bool editing = !m_taskBeingEdited.isNull();

    if (keyEvent->key() == Qt::Key_Escape) {
        if (editing) {
            editTask(Q_NULLPTR, EditModeNone);
        } else if (m_rightClickedTask) {
            setRightClickedTask(0);
        } else if (m_tagBeingEdited) {
            setTagEditStatus(TagEditStatusNone);
        } else if (m_page != MainPage) {
            setCurrentPage(MainPage);
        } else {
            setExpanded(false);
        }
        return false;
    }

    if (m_page != MainPage)
        return false;

    if (m_editMode == EditModeEditor)
        return false;

    if (editing && (keyEvent->key() != Qt::Key_Escape &&
                    keyEvent->key() != Qt::Key_Enter &&
                    keyEvent->key() != Qt::Key_Return)) {
        return false;
    }

    switch (keyEvent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (editing) {
            editTask(Q_NULLPTR, EditModeNone);
        } else {
            if (m_selectedTask == Q_NULLPTR) {
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

    if (m_taskBeingEdited != task.data()) {
        if (m_taskBeingEdited && m_taskBeingEdited->summary().isEmpty()) {
            // Empty summaries are not allowed !
            m_taskBeingEdited->setSummary(tr("New Task"));
        }

        // Disabling saving when editor is opened, only save when it's closed.
        m_storage->setDisableSaving(!task.isNull());

        if (task.isNull()) {
            m_taskBeingEdited.clear();
            m_storage->save(); // Editor closed. Write to disk immediately.
        } else {
            m_taskBeingEdited = task.data();
        }
        emit taskBeingEditedChanged();
    }

    setSelectedTask(Task::Ptr());
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

    if (m_storage->createTag(tagName))
        setTagEditStatus(TagEditStatusNone);
}

void Controller::requestContextMenu(Task *task)
{
    editTask(0, EditModeNone);
    setRightClickedTask(task);
}

void Controller::addTask(const QString &text, bool startEditMode)
{
    Task::Ptr task = m_storage->prependTask(text);

    if (m_currentTabTag && queueType() == QueueTypeArchive)
        task->addTag(m_currentTabTag->name());

    task->setStaged(m_queueType == QueueTypeToday);
    editTask(Q_NULLPTR, EditModeNone);

    if (startEditMode) {
        setExpanded(true);
        //int lastIndex = m_storage->taskFilterModel()->rowCount()-1;
        int lastIndex = 0;
        editTask(m_storage->taskAt(lastIndex).data(), EditModeInline);
        emit forceFocus(lastIndex);
        emit addingNewTask();
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

void Controller::setLoadManager(LoadManager* loadManager)
{
    if (loadManager != m_loadManager) {
        m_loadManager = loadManager;
        emit loadManagerChanged();
    }
}

LoadManager* Controller::loadManager() const
{
    return m_loadManager;
}

QString Controller::gitDate() const
{
    return STR(FLOW_VERSION_DATE);
}

void Controller::setHideEmptyTags(bool hide)
{
    if (hide != m_hideEmptyTags) {
        m_hideEmptyTags = hide;
        m_settings->setValue("hideEmptyTags", QVariant(hide));
        emit hideEmptyTagsChanged();
    }
}

bool Controller::hideEmptyTags() const
{
    return m_hideEmptyTags;
}

QString Controller::qtVersion() const
{
    return qVersion();
}

void Controller::setUseSystray(bool use)
{
    if (use != m_useSystray) {
        m_useSystray = use;
        m_settings->setValue("useSystray", QVariant(use));
        emit useSystrayChanged();

        if (use)
            m_kernel->setupSystray();
        else
            m_kernel->destroySystray();
    }
}

bool Controller::useSystray() const
{
    return m_useSystray;
}

void Controller::setStickyWindow(bool sticky)
{
    if (sticky != m_stickyWindow) {
        m_stickyWindow = sticky;
        m_settings->setValue("stickyWindow", QVariant(sticky));
        emit stickyWindowChanged();
    }
}

bool Controller::stickyWindow() const
{
    return m_stickyWindow;
}

bool Controller::isOSX() const
{
    return Utils::isOSX();
}
