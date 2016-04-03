/*
  This file is part of Flow.

  Copyright (C) 2014-2015 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_TEST_BASE_H
#define FLOW_TEST_BASE_H


#include <QObject>
#include <qnamespace.h>

#include <functional>

class Kernel;
class QuickView;
class Storage;
class Controller;
class Settings;
class QString;
class QQuickItem;

class TestBase : public QObject
{
public:

    TestBase();
    ~TestBase();

    bool checkStorageConsistency(int expectedTagCount = -1);
    void createNewKernel(const QString &dataFilename, bool load = true);
    void createInstanceWithUI();
    void waitForIt();
    void waitUntil(std::function<bool()> pred);
    bool textInputHasFocus() const;
    void stopWaiting();
    void mouseClick(QQuickItem *item);
    void moveMouseTo(QQuickItem *item);
    void mouseClick(const QString &objectName);
    void sendText(const QString &text);
    void sendKey(int key, const QString &text = "", Qt::KeyboardModifiers modifiers = 0);
    void clearTasks();
protected:
    Kernel *m_kernel;
    QuickView *m_view;
    Storage *m_storage;
    Controller *m_controller;
    Settings *m_settings;
};

#endif
