/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  Copyright (C) 2015-2016, 2018 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_CONTROLLER_H
#define FLOW_CONTROLLER_H

#include "task.h"

#include <QObject>
#include <QString>
#include <QPointer>

class QTimer;
class LoadManager;
class Storage;
class Kernel;
class Settings;
class QQmlContext;
class QWindow;

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(int selectedTaskIndex READ selectedTaskIndex NOTIFY selectedTaskIndexChanged)
    Q_PROPERTY(QAbstractItemModel* tagsModel READ tagsModel NOTIFY tagsModelChanged)
    Q_PROPERTY(Tag* untaggedTasksTag READ untaggedTasksTag CONSTANT)
    Q_PROPERTY(Tag* allTasksTag READ allTasksTag CONSTANT)
    Q_PROPERTY(Tag* dueDateTasksTag READ dueDateTasksTag CONSTANT)
    Q_PROPERTY(bool expertMode READ expertMode CONSTANT)
    Q_PROPERTY(QString buildOptionsText READ buildOptionsText CONSTANT)
    Q_PROPERTY(int currentMenuIndex READ currentMenuIndex WRITE setCurrentMenuIndex NOTIFY currentMenuIndexChanged)
    Q_PROPERTY(bool isOSX READ isOSX CONSTANT)
    Q_PROPERTY(QString qtVersion READ qtVersion CONSTANT)
    Q_PROPERTY(QString gitDate READ gitDate CONSTANT)
    Q_PROPERTY(LoadManager* loadManager READ loadManager)
    Q_PROPERTY(int textRenderType READ textRenderType CONSTANT)
    Q_PROPERTY(bool firstSecondsAfterAdding READ firstSecondsAfterAdding NOTIFY firstSecondsAfterAddingChanged)
    Q_PROPERTY(int remainingMinutes READ remainingMinutes NOTIFY remainingMinutesChanged)
    Q_PROPERTY(int currentTaskDuration READ currentTaskDuration NOTIFY currentTaskDurationChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int configureTabIndex READ configureTabIndex WRITE setConfigureTabIndex NOTIFY configureTabIndexChanged)
    Q_PROPERTY(Task* currentTask READ currentTask NOTIFY currentTaskChanged) // Task being played
    Q_PROPERTY(Task* rightClickedTask READ rightClickedTask NOTIFY rightClickedTaskChanged)
    Q_PROPERTY(Task *selectedTask READ selectedTask NOTIFY selectedTaskChanged)
    Q_PROPERTY(Tag* currentTag READ currentTag WRITE setCurrentTag NOTIFY currentTagChanged)
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
    Q_PROPERTY(qreal dpiFactor READ dpiFactor NOTIFY screenChanged)
    Q_PROPERTY(bool isMobile READ isMobile CONSTANT)
    Q_PROPERTY(bool isIOS READ isIOS CONSTANT)
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT)
    Q_PROPERTY(bool openSSLSupported READ openSSLSupported CONSTANT)
    Q_PROPERTY(bool hackingMenuSupported READ hackingMenuSupported CONSTANT)
    Q_PROPERTY(bool showPomodoroOverlay READ showPomodoroOverlay WRITE setShowPomodoroOverlay NOTIFY showPomodoroOverlayChanged)

    Q_PROPERTY(QString currentTitleText READ currentTitleText NOTIFY currentTitleTextChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(bool newTagDialogVisible READ newTagDialogVisible WRITE setNewTagDialogVisible NOTIFY newTagDialogVisibleChanged)
    Q_PROPERTY(bool optionsContextMenuVisible READ optionsContextMenuVisible WRITE setOptionsContextMenuVisible NOTIFY optionsContextMenuVisibleChanged)
    Q_PROPERTY(bool startupFinished READ startupFinished NOTIFY startupFinishedChanged)
    Q_PROPERTY(QDate currentDate READ currentDate NOTIFY currentDateChanged)

public:
    enum Page {
        InvalidPage = 0,
        MainPage,
        ConfigurePage,
        AboutPage
    };
    Q_ENUM(Page)

    enum ConfigureTab {
        GeneralTab = 0,
        TagsTab,
        PluginsTabs
    };
    Q_ENUM(ConfigureTab)

    enum EditMode {
        EditModeNone = 0, // We're not editing a task
        EditModeInline, // We're editing inline
        EditModeEditor // We're using the task editor
    };
    Q_ENUM(EditMode)

    enum TagEditStatus {
        TagEditStatusNone = 0,
        TagEditStatusEdit, // Tag is being edited
        TagEditStatusNew // Tag is being created
    };
    Q_ENUM(TagEditStatus)

    enum QueueType {
        QueueTypeToday = 0,
        QueueTypeArchive
    };
    Q_ENUM(QueueType)

    enum {
        NativeRendering = 0,
        QtRendering = 1
    };

    explicit Controller(QQmlContext *context, Kernel *, Storage *storage,
                        Settings *settings, QObject *parent = nullptr);
    ~Controller();

    int remainingMinutes() const;
    int currentTaskDuration() const; // in minutes
    Task *currentTask() const;
    EditMode editMode() const;

    bool expanded() const;
    void setExpanded(bool expanded);

    bool showPomodoroOverlay() const;
    void setShowPomodoroOverlay(bool);

    bool firstSecondsAfterAdding() const;

    Controller::Page currentPage() const;
    void setCurrentPage(Page page);

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

    Tag *currentTag() const;

    Controller::QueueType queueType() const;
    void setQueueType(QueueType);

    bool isMobile() const;
    bool isIOS() const;
    bool isAndroid() const;
    bool isOSX() const;
    bool openSSLSupported() const;
    bool hackingMenuSupported() const;

    QString currentTitleText() const;
    QString version() const;

    bool optionsContextMenuVisible() const;
    void setOptionsContextMenuVisible(bool);
    bool newTagDialogVisible() const;

    bool startupFinished() const;
    void setNewTagDialogVisible(bool visible);

    int textRenderType() const;

    LoadManager *loadManager() const;

    QString gitDate() const;
    QString qtVersion() const;

    int currentMenuIndex() const;

    QString buildOptionsText() const;

    bool expertMode() const;

    Tag* allTasksTag() const;
    Tag* hotTasksTag() const;
    Tag* untaggedTasksTag() const;
    Tag* dueDateTasksTag() const;

    QAbstractItemModel* tagsModel() const;

    int selectedTaskIndex() const;
    QDate currentDate() const;

    void setWindow(QWindow *);

    bool inSpecificTagView() const;
    bool inTodayView() const;

public Q_SLOTS:
    void dismissTaskMenuDelayed();
    void dismissTaskMenu();

    void setCurrentTag(Tag *);
    void addTask(const QString &text, bool startEditMode);
    void removeTask(Task *);

    void startPomodoro(Task *);
    void stopPomodoro();
    void pausePomodoro();

    void cycleTaskSelectionUp();
    void cycleTaskSelectionDown();

    void cycleMenuSelectionUp();
    void cycleMenuSelectionDown();

    void cycleTagSelectionLeft();
    void cycleTagSelectionRight();
    void selectTagByFirstLetter(QChar);

    void showQuestionPopup(QObject *obj, const QString &text, const QString &callback);
    void onPopupButtonClicked(bool okClicked);

    void editTag(const QString &tagName);
    bool renameTag(const QString &oldName, const QString &newName);

    void editTask(Task *, EditMode);
    void dismissTaskEditor();

    void endAddingNewTag(const QString &tagName);

    void requestContextMenu(Task *, bool tagOnlyMenu = false);

    void setRightClickedTask(Task *, bool tagOnlyMenu = false);
    void toggleConfigurePage();
    void forceFocusOnTaskBeingEdited();

    void openUrl(const QString &url);

    void setTaskDueDate(Task *task, QDate);

private Q_SLOTS:
    void onTimerTick();
    void onCurrentTagDestroyed();
    void onKeepScreenOnDuringPomodoroChanged();
    void onPomodoroFunctionalityDisabledChanged();
    void onShowAllTasksViewChanged();
    void onSupportsDueDateChanged();
    void onUseSystrayChanged();
    void onHideEmptyTagsChanged();
    void setStartupFinished();

Q_SIGNALS:
    void screenChanged();
    void currentDateChanged();
    void selectedTaskIndexChanged();
    void tagsModelChanged();
    void aboutToAddTask();
    void enterPressed();
    void currentMenuIndexChanged();
    void loadManagerChanged();
    void textRenderTypeChanged();
    void remainingMinutesChanged();
    void currentTaskDurationChanged();
    void taskFinished();
    void expandedChanged();
    void firstSecondsAfterAddingChanged();
    void currentPageChanged();
    void forceFocus(Task *task);
    void currentTaskChanged();
    void popupVisibleChanged();
    void popupTextChanged();
    void taskBeingEditedChanged();
    void editModeChanged();
    void tagEditStatusChanged();
    void rightClickedTaskChanged();
    void configureTabIndexChanged();
    void selectedTaskChanged();
    void currentTagChanged();
    void invalidateTaskModel();
    void queueTypeChanged();
    void addingNewTask();
    void requestActivateWindow();
    void currentTitleTextChanged();
    void optionsContextMenuVisibleChanged();
    void startupFinishedChanged();
    void newTagDialogVisibleChanged();
    void showPomodoroOverlayChanged();
    void taskEditorDismissed();

private:
    void handleScreenChanged();
    Tag::Ptr tagForSummary(QString &summary_inout) const; // Transforms "books: Foo" into "Foo" and returns tag books
    void setSelectedTaskIndex(int);
    void validateSelectedTask();
    void updateExtendedTagModel();
    bool anyOverlayVisible() const;
    bool taskMenuVisible() const;
    void setCurrentMenuIndex(int);
    void toggleQueueType();
    void setTextRenderType(int);
    int indexOfTaskInCurrentTab(const Task::Ptr &task);
    Task::Ptr lastTaskAtCurrentTab() const;
    Task::Ptr taskAtCurrentTab(int taskIndex) const;
    QAbstractItemModel *currentTabTaskModel() const;
    void setTaskStatus(TaskStatus status);
    void setTagEditStatus(TagEditStatus);
    bool eventFilter(QObject *, QEvent *) override;

    Kernel *m_kernel;
    int m_currentTaskDuration;
    QTimer *m_tickTimer;
    QTimer *m_afterAddingTimer;
    int m_elapsedMinutes;
    bool m_expanded;
    Task::Ptr m_currentTask;
    Page m_page;
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
    QPointer<Tag> m_currentTag;
    QueueType m_queueType;
    Storage *m_storage;
    QQmlContext *m_qmlContext;
    Settings *m_settings;

    bool m_optionsContextMenuVisible;
    bool m_startupFinished;

    bool m_newTagDialogVisible;
    bool m_showPomodoroOverlay;

    qint64 m_pomodoroStartTimeStamp;
    int m_textRenderType;
    LoadManager* m_loadManager;
    bool m_addingTask;
    int m_currentMenuIndex;
    bool m_expertMode;
    Tag::Ptr m_allTasksTag;
    Tag::Ptr m_hotTasksTag;
    Tag::Ptr m_untaggedTasksTag;
    Tag::Ptr m_dueDateTasksTag;
    int m_selectedTaskIndex;
    QWindow *m_window = nullptr;
};

#endif
