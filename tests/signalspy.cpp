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

#include "signalspy.h"
#include <QDebug>
#include <QMetaMethod>

SignalSpy::SignalSpy(QObject *parent)
    : QObject(parent)
{
}

void SignalSpy::clear()
{
    m_caughtSignals.clear();
}

int SignalSpy::count() const
{
    return m_caughtSignals.count();
}

QString SignalSpy::at(int index) const
{
    return m_caughtSignals.at(index);
}

QStringList SignalSpy::caughtSignals() const
{
    return m_caughtSignals;
}

void SignalSpy::dumpDebugInfo()
{
    foreach (const QString &signal, m_caughtSignals) {
        qDebug() << signal;
    }
}

void SignalSpy::onSignalTriggered()
{
    QMetaMethod method = sender()->metaObject()->method(senderSignalIndex());
    m_caughtSignals << method.name();
}
