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

#ifndef SIGNALSPY_H
#define SIGNALSPY_H

#include <QObject>
#include <QStringList>

// Better version of QSignalSpy, which supports order and multiple signals

class SignalSpy : public QObject
{
    Q_OBJECT
public:
    explicit SignalSpy(QObject *parent = 0);

    template <typename Func1>
    inline void listenTo(const typename QtPrivate::FunctionPointer<Func1>::Object *sender,
                         Func1 signal)
    {
        connect(sender, signal, this, &SignalSpy::onSignalTriggered);

    }

    template <typename Func1>
    inline void unlistenTo(const typename QtPrivate::FunctionPointer<Func1>::Object *sender,
                           Func1 signal)
    {
        disconnect(sender, signal, this, &SignalSpy::onSignalTriggered);
    }

    void clear();
    int count() const;
    QString at(int) const;
    QStringList caughtSignals() const;

    void dumpDebugInfo();

public Q_SLOTS:
    void onSignalTriggered();

private:
    QStringList m_caughtSignals;
};

#endif
