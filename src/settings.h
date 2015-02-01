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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <QSettings>

class Settings : public QSettings {
    Q_OBJECT
    Q_PROPERTY(bool supportsDueDate READ supportsDueDate WRITE setSupportsDueDate NOTIFY supportsDueDateChanged)
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
    Q_ENUMS(Position)

    enum GeometryType {
        GeometryStandard, // 400x40
        GeometryThin, // 400x20
        GeometrySmallSquare, // 80x80
        GeometryCustom, // uses width and height from settings
        MaxGeometryTypes
    };
    Q_ENUMS(GeometryType)

    explicit Settings(QObject *parent = 0);
    explicit Settings(const QString &filename, QObject *parent = 0); // Overload for unit-tests
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

private:
    void init();
    void sync();
    bool m_syncScheduled;
    bool m_needsSync;

    int m_defaultPomodoroDuration;
    bool m_pomodoroFunctionalityDisabled;
    bool m_keepScreenOnDuringPomodoro;
    bool m_syncAtStartup;
    bool m_hideEmptyTags;
    bool m_showAllTasksView;
    bool m_useSystray;
    bool m_showTaskAge;
    bool m_stickyWindow;
    Position m_initialPosition;
    GeometryType m_geometryType;
    bool m_showContextMenuAfterAdd;
    bool m_supportsDueDate;
};

#endif
