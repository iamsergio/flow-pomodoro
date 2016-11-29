/*
  This file is part of Flow.

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

#ifndef FLOW_GIT_UPDATER_H
#define FLOW_GIT_UPDATER_H

#include <QObject>
#include <QTimer>

class Kernel;

class GitUpdater : public QObject
{
    Q_OBJECT
public:
    explicit GitUpdater(Kernel *kernel, QObject *parent = nullptr);
    void schedulePush();
    void push();
private:
    QTimer m_pushTimer;
    bool m_currenltyPushing = false;
    Kernel *const m_kernel;
};

#endif
