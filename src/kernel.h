/*
  This file is part of Flow.

  Copyright (C) 2014 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_KERNEL_H
#define FLOW_KERNEL_H

#include "task.h"
#include "runtimeconfiguration.h"

#include <QObject>
#include <QTimer>

class TagManager;
class Settings;
class Storage;
class Controller;
class PluginModel;
class QQmlEngine;
class QQmlContext;
class QMenu;
class QUrl;
class GitUpdater;

#ifdef QT_WIDGETS_LIB
#include <QSystemTrayIcon>
#endif

class Kernel : public QObject
{
    Q_OBJECT
public:
    explicit Kernel(const RuntimeConfiguration &, QObject *parent = nullptr);
    ~Kernel();
    Storage* storage() const;
    Controller *controller() const;
    TagManager *tagManager() const;
    QQmlContext *qmlContext() const;
    QQmlEngine *qmlEngine() const;
    Settings *settings() const;
    GitUpdater *gitUpdater() const;
    RuntimeConfiguration runtimeConfiguration() const;
    QDate currentDate() const;

    void setupSystray();
    void destroySystray();

    void saveOrOpenUrl(const QUrl &url);
    bool isReadOnly() const;

Q_SIGNALS:
    void systrayLeftClicked();
    void dayChanged();
    void dumpDebugInfoRequested();

private Q_SLOTS:
    void onTaskStatusChanged();
    void checkDayChanged();
    void maybeLoadPlugins();
    void maybeScheduleGitSync();
#if defined(QT_WIDGETS_LIB) && !defined(QT_NO_SYSTRAY)
    void onSystrayActivated(QSystemTrayIcon::ActivationReason reason);
#endif

private:
    void dumpDebug();
    void loadPlugins();
    void notifyPlugins(TaskStatus newStatus);

    const RuntimeConfiguration m_runtimeConfiguration;
    TagManager *const m_tagManager;
    Storage *m_storage;
    QQmlEngine *m_qmlEngine;
    Settings *m_settings;
    Controller *m_controller;
    PluginModel *m_pluginModel;
#if defined(QT_WIDGETS_LIB) && !defined(QT_NO_SYSTRAY)
    QSystemTrayIcon *m_systrayIcon;
    QMenu *m_trayMenu;
#endif
    QTimer m_dayChangedTimer;
    QDate m_currentDate;
    GitUpdater *const m_gitUpdater;

    static QPointer<Kernel> s_kernel; // QPointer, so unit-tests can delete and recreate
};

#endif
