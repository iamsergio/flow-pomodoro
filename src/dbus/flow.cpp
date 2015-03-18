/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "flow.h"
#include "kernel.h"
#include "controller.h"

Flow::Flow(Kernel *kernel, QObject *parent)
    : QObject(parent)
    , m_kernel(kernel)
{
}

void Flow::toggleExpand()
{
    if (m_kernel)
        m_kernel->controller()->setExpanded(!m_kernel->controller()->expanded());
}

void Flow::newTask(const QString &text, bool startEditor, bool expand)
{
    if (m_kernel) {
        m_kernel->controller()->setExpanded(expand);
        m_kernel->controller()->addTask(text, startEditor);
    }
}
