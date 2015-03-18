/*
  This file is part of Flow.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

  Copyright (C) 2015 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef _DBUS_FLOW_H_
#define _DBUS_FLOW_H_

#include <QObject>
#include <QPointer>

class Kernel;

class Flow : public QObject {
    Q_OBJECT
public:
    explicit Flow(Kernel *kernel, QObject *parent = 0);

public Q_SLOTS:
    Q_SCRIPTABLE void toggleExpand();
    Q_SCRIPTABLE void newTask(const QString &text, bool startEditor, bool expand);

private:
    QPointer<Kernel> m_kernel;
};

#endif
