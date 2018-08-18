/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  Copyright (C) 2016 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_SETTINGS_H
#define FLOW_SETTINGS_H

#include <QSettings>

enum {
    DefaultPomodoroDuration = 25
};

class Settings : public QSettings {
    Q_OBJECT
    Q_PROPERTY(bool supportsGitSync READ supportsGitSync WRITE setSupportsGitSync NOTIFY supportsGitSyncChanged)
    Q_PROPERTY(bool supportsEffort READ supportsEffort WRITE setSupportsEffort NOTIFY supportsEffortChanged)
    Q_PROPERTY(bool supportsDueDate READ supportsDueDate WRITE setSupportsDueDate NOTIFY supportsDueDateChanged)
    Q_PROPERTY(bool supportsPriority READ supportsPriority WRITE setSupportsPriority NOTIFY supportsPriorityChanged)
    Q_PROPERTY(bool showContextMenuAfterAdd READ showContextMenuAfterAdd WRITE setShowContextMenuAfterAdd NOTIFY showContextMenuAfterAddChanged)
    Q_PROPERTY(int defaultPomodoroDuration READ defaultPomodoroDuration WRITE setDefaultPomodoroDuration NOTIFY defaultPomodoroDurationChanged)
    Q_PROPERTY(bool pomodoroFunctionalityDisabled READ pomodoroFunctionalityDisabled WRITE setPomodoroFunctionalityDisabled NOTIFY pomodoroFunctionalityDisabledChanged)
    Q_PROPERTY(bool syncAtStartup READ syncAtStartup WRITE setSyncAtStartup NOTIFY syncAtStartupChanged)
    Q_PROPERTY(bool showAllTasksView READ showAllTasksView WRITE setShowAllTasksView NOTIFY showAllTasksViewChanged)
    Q_PROPERTY(bool showTaskAge READ showTaskAge WRITE setShowTaskAge NOTIFY showTaskAgeChanged)
    Q_PROPERTY(bool stickyWindow READ stickyWindow WRITE setStickyWindow NOTIFY stickyWindowChanged)
    Q_PROPERTY(bool useSystray READ useSystray WRITE setUseSystray NOTIFY useSystrayChanged)
    Q_PROPERTY(bool hideEmptyTags READ hideEmptyTags WRITE setHideEmptyTags NOTIFY hideEmptyTagsChanged)
    Q_PROPERTY(bool keepScreenOnDuringPomodoro READ keepScreenOnDuringPomodoro WRITE setKeepScreenOnDuringPomodoro NOTIFY keepScreenOnDuringPomodoroChanged)
    Q_PROPERTY(GeometryType geometryType READ geometryType WRITE setGeometryType NOTIFY geometryTypeChanged)
    Q_PROPERTY(Position initialPosition READ initialPosition WRITE setInitialPosition NOTIFY initialPositionChanged)
    Q_PROPERTY(QString fileName READ fileName CONSTANT)
public:
    enum Position {
        PositionNone = 0, // Window will appear where WM puts it
        PositionLast, // Window will appear at the same place as last time. Saves position at exit.
        PositionTop,
        PositionTopLeft,
        PositionTopRight,
        PositionBottom, // TODO: Bottom is not supported yet
        PositionBottomLeft,
        PositionBottomRight,
        MaxPositions
    };
    Q_ENUM(Position)

    enum GeometryType {
        GeometryStandard, // 400x40
        GeometryThin, // 400x20
        GeometrySmallSquare, // 80x80
        GeometryCustom, // uses width and height from settings
        MaxGeometryTypes
    };
    Q_ENUM(GeometryType)

    explicit Settings(QObject *parent = nullptr);
    explicit Settings(const QString &filename, QObject *parent = nullptr); // Overload for unit-tests
    ~Settings();
    void setValue(const QString &key, const QVariant &value);
    void scheduleSync();
    bool needsSync() const;

    void setDefaultPomodoroDuration(int duration);
    int defaultPomodoroDuration() const;
    void setPomodoroFunctionalityDisabled(bool);
    bool pomodoroFunctionalityDisabled() const;
    void setKeepScreenOnDuringPomodoro(bool);
    bool keepScreenOnDuringPomodoro() const;
    bool syncAtStartup() const;
    void setSyncAtStartup(bool);
    void setShowTaskAge(bool);
    bool showTaskAge() const;
    void setShowAllTasksView(bool);
    bool showAllTasksView() const;
    void setHideEmptyTags(bool);
    bool hideEmptyTags() const;
    void setUseSystray(bool);
    bool useSystray() const;
    void setStickyWindow(bool);
    bool stickyWindow() const;
    Position initialPosition() const; // Specifies where the window will appear at startup
    void setInitialPosition(Position);
    void setGeometryType(GeometryType);
    Settings::GeometryType geometryType() const;

    void setShowContextMenuAfterAdd(bool);
    bool showContextMenuAfterAdd() const;

    void setSupportsDueDate(bool);
    bool supportsDueDate() const;
    bool supportsPriority() const;
    void setSupportsPriority(bool);

    QString fileName() const;

    bool supportsEffort() const;
    void setSupportsEffort(bool);

    bool supportsGitSync() const;
    void setSupportsGitSync(bool);

    bool supportsToolTips() const;

private Q_SLOTS:
    void doSync();

Q_SIGNALS:
    void showContextMenuAfterAddChanged();
    void defaultPomodoroDurationChanged();
    void pomodoroFunctionalityDisabledChanged();
    void syncAtStartupChanged();
    void showAllTasksViewChanged();
    void showTaskAgeChanged();
    void stickyWindowChanged();
    void useSystrayChanged();
    void hideEmptyTagsChanged();
    void keepScreenOnDuringPomodoroChanged();
    void geometryTypeChanged();
    void initialPositionChanged();
    void supportsDueDateChanged();
    void supportsPriorityChanged();
    void supportsEffortChanged();
    void supportsGitSyncChanged();

private:
    void init();
    void sync();
    bool m_syncScheduled = false;
    bool m_needsSync = false;

    int m_defaultPomodoroDuration = DefaultPomodoroDuration;
    bool m_pomodoroFunctionalityDisabled = false;
    bool m_keepScreenOnDuringPomodoro = false;
    bool m_syncAtStartup = true;
    bool m_hideEmptyTags = false;
    bool m_showAllTasksView = false;
    bool m_useSystray = false;
    bool m_showTaskAge = false;
    bool m_stickyWindow = true;
    Position m_initialPosition;
    GeometryType m_geometryType;
    bool m_showContextMenuAfterAdd = true;
    bool m_supportsDueDate = true;
    bool m_supportsPriority = false;
    bool m_supportsEffort = false;
    bool m_supportsGitSync = false;
    bool m_supportsToolTips = true;
};

#endif
