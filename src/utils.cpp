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

#include "utils.h"

#include <QDebug>
#include <QElapsedTimer>
#include <qglobal.h>
#include <QScreen>
#include <QGuiApplication>

#ifdef Q_OS_ANDROID
# include <QAndroidJniObject>
# include <QAndroidJniEnvironment>
# include <QtAndroid>
#endif

void Utils::printTimeInfo(const QString &location)
{
#ifdef FLOW_DEBUG_TIMESTAMPS
    static QElapsedTimer timer;
    static int total = 0;
    static int count = 0;

    if (count == 0)
        timer.start();

    count++;
    const int elapsed = timer.elapsed();
    total += elapsed;

    qDebug() << "Profiler:" << location << "; step=" << elapsed << "total=" << total;
    timer.restart();
#else
    Q_UNUSED(location);
#endif
}

void Utils::keepScreenOn(bool keep)
{
#ifdef Q_OS_ANDROID
    QtAndroid::androidActivity().callMethod<void>("setKeepScreenOn", "(Z)V", jboolean(keep));
    if (QAndroidJniEnvironment()->ExceptionCheck()) {
        QAndroidJniEnvironment()->ExceptionDescribe();
        QAndroidJniEnvironment()->ExceptionClear();
    }
#else
    Q_UNUSED(keep);
#endif
}


bool Utils::isMobile()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_BLACKBERRY) || defined(Q_OS_IOS) || defined(Q_OS_WINRT)
    return true;
#else
    return false;
#endif
}


qreal Utils::dpiFactor()
{
    static qreal factor = -1;
    if (factor == -1) {
        QScreen *screen = QGuiApplication::primaryScreen();
        Q_ASSERT(screen);
        factor = isMobile() ? (screen->physicalDotsPerInch() / 72.0) / 2 // /2 because looks good on all devices
                            : 1;
    }

    return factor;
}
