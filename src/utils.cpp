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
#include <QScreen>
#include <QGuiApplication>
#include <QString>
#include <QUrl>
#include <QProcess>
#include <QtMath>

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
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WINRT)
    return true;
#else
    return false;
#endif
}

bool Utils::isOSX()
{
#ifdef Q_OS_OSX
    return true;
#endif
    return false;
}

qreal Utils::dpiFactor()
{
    static qreal factor = -1;
    if (factor == -1) {
        QScreen *screen = QGuiApplication::primaryScreen();
        Q_ASSERT(screen);
        const qreal screenDpi = screen->physicalDotsPerInch();

        if (isMobile()) {
            // Return the dpi racio against the phone where flow was prototyped in, so it looks the same.
            const qreal myOldPhoneDpi = 144.0;
            factor = screenDpi / myOldPhoneDpi;
        } else {
            // Return the dpi racio against the monitor where flow was prototyped in, so it looks the same.
            const qreal myOldMonitorDpi = 144.0;

            const qreal screenWidthInches = screen->geometry().width() / screenDpi;
            const qreal screenHeightInches = screen->geometry().height() / screenDpi;
            const qreal screenDiagonalInches = qSqrt(screenWidthInches * screenWidthInches + screenHeightInches * screenHeightInches);

            qDebug() << screenDiagonalInches;

            qreal bigScreenCompensation = 1;
            const qreal myBigScreenDiagonalInches = 27;
            if (screenDiagonalInches > 21) {
                // For big screens lets add some compensation, because usually you're a bit further away from them
                bigScreenCompensation = 1.8 * (screenDiagonalInches / myBigScreenDiagonalInches);
            } else {
                // For regular screens a linear DPI factor works quite well
                bigScreenCompensation = 1;
            }

            factor = bigScreenCompensation * screenDpi / myOldMonitorDpi;
        }
    }

    return factor;
}

void Utils::openUrl(const QUrl &url)
{
    if (url.isEmpty())
        return;

    QString command;
#if defined(Q_OS_LINUX)
    command = QStringLiteral("xdg-open");
#elif defined(Q_OS_WIN)
    command = "start";
#elif defined(Q_OS_OSX)
    command = "open";
#else
    qWarning() << "Will try opening" << url << "with firefox";
    command = "firefox";
#endif

    if (!QProcess::startDetached(command, QStringList() << url.url()))
        qWarning() << "Failed to start command " << command << url.url();
}
