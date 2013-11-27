/*
  This file is part of Flow.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

class QTimer;
class TaskModel;

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool firstSecondsAfterAdding READ firstSecondsAfterAdding NOTIFY firstSecondsAfterAddingChanged)
    Q_PROPERTY(int remainingMinutes READ remainingMinutes NOTIFY remainingMinutesChanged)
    Q_PROPERTY(int currentTaskDuration READ currentTaskDuration NOTIFY currentTaskDurationChanged)
    Q_PROPERTY(QString taskText READ taskText NOTIFY taskTextChanged)
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(int indexBeingEdited READ indexBeingEdited WRITE setIndexBeingEdited NOTIFY indexBeingEditedChanged)
    Q_PROPERTY(Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int defaultPomodoroDuration READ defaultPomodoroDuration WRITE setDefaultPomodoroDuration NOTIFY defaultPomodoroDurationChanged)
    Q_PROPERTY(TaskStatus taskStatus READ taskStatus NOTIFY taskStatusChanged)

    // Shortcuts
    Q_PROPERTY(bool paused  READ paused  NOTIFY taskStatusChanged)
    Q_PROPERTY(bool stopped READ stopped NOTIFY taskStatusChanged)
    Q_PROPERTY(bool running READ running NOTIFY taskStatusChanged)

public:

    enum Page {
        InvalidPage,
        TheQueuePage,
        ConfigurePage,
        AboutPage
    };
    Q_ENUMS(Page);

    explicit Controller(TaskModel *model, QObject *parent = 0);

    int remainingMinutes() const;
    int currentTaskDuration() const; // in minutes
    QString taskText() const;
    int indexBeingEdited() const;
    void setIndexBeingEdited(int index);

    bool expanded() const;
    void setExpanded(bool expanded);

    bool firstSecondsAfterAdding() const;

    Controller::Page currentPage() const;
    void setCurrentPage(Page page);

    TaskStatus taskStatus() const;
    void setTaskStatus(TaskStatus status);

    void setDefaultPomodoroDuration(int duration);
    int defaultPomodoroDuration() const;

    bool running() const;
    bool stopped() const;
    bool paused() const;

public Q_SLOTS:
    void addTask(const QString &text, bool startEditMode);
    void removeTask(int index);
    void updateTask(int index, const QString &newText);

    void startPomodoro(int queueIndex);
    void stopPomodoro(bool reQueueTask);
    void pausePomodoro();

private Q_SLOTS:
    void onTimerTick();

Q_SIGNALS:
    void remainingMinutesChanged(int);
    void currentTaskDurationChanged(int);
    void taskTextChanged();
    void taskFinished();
    void indexBeingEditedChanged(int);
    void expandedChanged(bool);
    void firstSecondsAfterAddingChanged();
    void currentPageChanged(Page page);
    void defaultPomodoroDurationChanged(int);
    void taskStatusChanged();

private:
    void setSelectedIndex(int index);

private:
    int m_currentTaskDuration;
    QTimer *m_tickTimer;
    QTimer *m_afterAddingTimer;
    int m_elapsedMinutes;
    bool m_expanded;
    int m_indexBeingEdited;
    TaskModel *m_model;
    TaskStatus m_taskStatus;
    Task m_currentTask;
    Page m_page;
    int m_defaultPomodoroDuration;
};

#endif
