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

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "task.h"

#include <QObject>
#include <QString>
#include <QPointer>

class QTimer;
class Storage;
class Settings;
class QQmlContext;

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool firstSecondsAfterAdding READ firstSecondsAfterAdding NOTIFY firstSecondsAfterAddingChanged)
    Q_PROPERTY(int remainingMinutes READ remainingMinutes NOTIFY remainingMinutesChanged)
    Q_PROPERTY(int currentTaskDuration READ currentTaskDuration NOTIFY currentTaskDurationChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int configureTabIndex READ configureTabIndex WRITE setConfigureTabIndex NOTIFY configureTabIndexChanged)
    Q_PROPERTY(int defaultPomodoroDuration READ defaultPomodoroDuration WRITE setDefaultPomodoroDuration NOTIFY defaultPomodoroDurationChanged)
    Q_PROPERTY(bool pomodoroFunctionalityDisabled READ pomodoroFunctionalityDisabled WRITE setPomodoroFunctionalityDisabled NOTIFY pomodoroFunctionalityDisabledChanged)
    Q_PROPERTY(Task* currentTask READ currentTask NOTIFY currentTaskChanged) // Task being played
    Q_PROPERTY(Task* rightClickedTask READ rightClickedTask WRITE setRightClickedTask NOTIFY rightClickedTaskChanged)
    Q_PROPERTY(Task *selectedTask READ selectedTask NOTIFY selectedTaskChanged)
    Q_PROPERTY(Tag* currentTabTag READ currentTabTag WRITE setCurrentTabTag NOTIFY currentTabTagChanged)
    Q_PROPERTY(QueueType queueType READ queueType WRITE setQueueType NOTIFY queueTypeChanged)
    // Editing task properties
    Q_PROPERTY(QObject* taskBeingEdited READ taskBeingEdited NOTIFY taskBeingEditedChanged)
    Q_PROPERTY(EditMode editMode READ editMode NOTIFY editModeChanged)
    // Popup properties
    Q_PROPERTY(QString popupText READ popupText NOTIFY popupTextChanged)
    Q_PROPERTY(bool popupVisible READ popupVisible NOTIFY popupVisibleChanged)
    // Editing Tag properties
    Q_PROPERTY(TagEditStatus tagEditStatus READ tagEditStatus NOTIFY tagEditStatusChanged)
    // Other properties
    Q_PROPERTY(qreal dpiFactor READ dpiFactor CONSTANT)
    Q_PROPERTY(bool isMobile READ isMobile CONSTANT)
    Q_PROPERTY(bool isIOS READ isIOS CONSTANT)
    Q_PROPERTY(bool openSSLSupported READ openSSLSupported CONSTANT)
    Q_PROPERTY(bool hackingMenuSupported READ hackingMenuSupported CONSTANT)

    Q_PROPERTY(bool isHttps READ isHttps WRITE setIsHttps NOTIFY isHttpsChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString currentTitleText READ currentTitleText NOTIFY currentTitleTextChanged)
    Q_PROPERTY(QString version READ version CONSTANT)

    Q_PROPERTY(bool optionsContextMenuVisible READ optionsContextMenuVisible WRITE setOptionsContextMenuVisible NOTIFY optionsContextMenuVisibleChanged)
    Q_PROPERTY(bool configurePageRequested READ configurePageRequested NOTIFY configurePageRequestedChanged)
    Q_PROPERTY(bool aboutPageRequested READ aboutPageRequested NOTIFY aboutPageRequestedChanged)
    Q_PROPERTY(bool inlineEditorRequested READ inlineEditorRequested NOTIFY inlineEditorRequestedChanged)

public:
    enum Page {
        InvalidPage = 0,
        MainPage,
        ConfigurePage,
        AboutPage
    };
    Q_ENUMS(Page)

    enum ConfigureTab {
        GeneralTab = 0,
        TagsTab,
        PluginsTabs
    };
    Q_ENUMS(ConfigureTab)

    enum EditMode {
        EditModeNone = 0, // We're not editing a task
        EditModeInline, // We're editing inline
        EditModeEditor // We're using the task editor
    };
    Q_ENUMS(EditMode)

    enum TagEditStatus {
        TagEditStatusNone = 0,
        TagEditStatusEdit, // Tag is being edited
        TagEditStatusNew // Tag is being created
    };
    Q_ENUMS(TagEditStatus)

    enum QueueType {
        QueueTypeToday = 0,
        QueueTypeArchive
    };
    Q_ENUMS(QueueType)

    explicit Controller(QQmlContext *context, Storage *storage,
                        Settings *settings, QObject *parent = 0);
    ~Controller();

    int remainingMinutes() const;
    int currentTaskDuration() const; // in minutes
    Task *currentTask() const;
    EditMode editMode() const;

    bool expanded() const;
    void setExpanded(bool expanded);

    bool firstSecondsAfterAdding() const;

    Controller::Page currentPage() const;
    void setCurrentPage(Page page);

    void setDefaultPomodoroDuration(int duration);
    int defaultPomodoroDuration() const;

    void setPomodoroFunctionalityDisabled(bool);
    bool pomodoroFunctionalityDisabled() const;

    qreal dpiFactor() const;

    bool popupVisible() const;
    void setPopupVisible(bool);

    QString popupText() const;
    void setPopupText(const QString &);

    Task *taskBeingEdited() const;
    TagEditStatus tagEditStatus() const;

    Task *rightClickedTask() const;

    int configureTabIndex() const;
    void setConfigureTabIndex(int);

    Task *selectedTask() const;
    void setSelectedTask(const Task::Ptr &task);

    Tag *currentTabTag() const;

    Controller::QueueType queueType() const;
    void setQueueType(QueueType);

    bool isMobile() const;
    bool isIOS() const;
    bool openSSLSupported() const;
    bool hackingMenuSupported() const;

    // Webdav
    bool isHttps() const;
    void setIsHttps(bool);
    QString host() const;
    void setHost(const QString &);
    QString path() const;
    void setPath(const QString &);
    QString user() const;
    void setUser(const QString &);
    QString password() const;
    void setPassword(const QString &);
    int port() const;
    void setPort(int);

    QString currentTitleText() const;
    QString version() const;

    bool optionsContextMenuVisible() const;
    void setOptionsContextMenuVisible(int);

public Q_SLOTS:
    void updateWebDavCredentials();
    void setCurrentTabTag(Tag *);
    void addTask(const QString &text, bool startEditMode);
    void removeTask(Task *);

    void startPomodoro(Task *);
    void stopPomodoro();
    void pausePomodoro();

    void toggleSelectedTask(Task *task);
    void cycleSelectionUp();
    void cycleSelectionDown();

    void showQuestionPopup(QObject *obj, const QString &text, const QString &callback);
    void onPopupButtonClicked(bool okClicked);

    void editTag(const QString &tagName);
    bool renameTag(const QString &oldName, const QString &newName);

    void editTask(Task *, EditMode);

    void beginAddingNewTag();
    void endAddingNewTag(const QString &tagName);

    void requestContextMenu(Task *);

    void webDavSync();
    void setRightClickedTask(Task *);

    bool configurePageRequested() const;
    bool aboutPageRequested() const;
    bool inlineEditorRequested() const;

private Q_SLOTS:
    void onTimerTick();

Q_SIGNALS:
    void pomodoroFunctionalityDisabledChanged();
    void remainingMinutesChanged();
    void currentTaskDurationChanged();
    void taskFinished();
    void expandedChanged();
    void firstSecondsAfterAddingChanged();
    void currentPageChanged();
    void defaultPomodoroDurationChanged();
    void forceFocus(int index);
    void currentTaskChanged();
    void popupVisibleChanged();
    void popupTextChanged();
    void taskBeingEditedChanged();
    void editModeChanged();
    void tagEditStatusChanged();
    void rightClickedTaskChanged();
    void configureTabIndexChanged();
    void selectedTaskChanged();
    void currentTabTagChanged();
    void invalidateTaskModel();
    void queueTypeChanged();
    void addingNewTask();
    void requestActivateWindow();
    void currentTitleTextChanged();
    void optionsContextMenuVisibleChanged();
    void configurePageRequestedChanged();
    void aboutPageRequestedChanged();
    void inlineEditorRequestedChanged();

    // webdav stuff
    void hostChanged();
    void userChanged();
    void passwordChanged();
    void isHttpsChanged();
    void portChanged();
    void pathChanged();

private:
    int indexOfTaskInCurrentTab(const Task::Ptr &task);
    Task::Ptr lastTaskAtCurrentTab() const;
    Task::Ptr taskAtCurrentTab(int taskIndex) const;
    QAbstractItemModel *currentTabTaskModel() const;
    void setTaskStatus(TaskStatus status);
    void setTagEditStatus(TagEditStatus);
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;
    void setAboutPageRequested(bool);
    void setConfigurePageRequested(bool);
    void setInlineEditorRequested(bool);

    int m_currentTaskDuration;
    QTimer *m_tickTimer;
    QTimer *m_afterAddingTimer;
    int m_elapsedMinutes;
    bool m_expanded;
    Task::Ptr m_currentTask;
    Page m_page;
    int m_defaultPomodoroDuration;
    bool m_popupVisible;
    QString m_popupText;
    QString m_popupOkCallback;
    QPointer<QObject> m_popupCallbackOwner;
    Tag::Ptr m_tagBeingEdited;
    QPointer<Task> m_taskBeingEdited;
    EditMode m_editMode;
    TagEditStatus m_tagEditStatus;
    QPointer<Task> m_rightClickedTask;
    Task::Ptr m_invalidTask;
    int m_configureTabIndex;
    QPointer<Task> m_selectedTask;
    QPointer<Tag> m_currentTabTag;
    QueueType m_queueType;
    Storage *m_storage;
    bool m_pomodoroFunctionalityDisabled;
    QQmlContext *m_qmlContext;
    Settings *m_settings;

    // webdav stuff:
    int m_port;
    bool m_isHttps;
    QString m_host;
    QString m_path;
    QString m_user;
    QString m_password;

    bool m_optionsContextMenuVisible;
    bool m_configurePageRequested;
    bool m_aboutPageRequested;
    bool m_inlineEditorRequested;
};

#endif
