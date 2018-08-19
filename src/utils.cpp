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
#include <QFile>
#include <QJsonDocument>

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
    return isWebGL();
#endif
}

bool Utils::isOSX()
{
#ifdef Q_OS_OSX
    return true;
#endif
    return false;
}

static qreal userScaleFactor()
{
    static qreal factor = -1;
    if (factor == -1) {
        factor = 1.0;
        QByteArray env = qgetenv("FLOW_SCALE_FACTOR");
        if (!env.isEmpty()) {
            bool ok = false;
            factor = env.toDouble(&ok);
            if (!ok || factor <= 0.0)
                factor = 1.0;
        }
    }

    return factor;
}

qreal Utils::dpiFactor(QScreen *screen)
{
    static QHash<QScreen*, qreal> s_dpiFactors;
    auto it = s_dpiFactors.constFind(screen);
    const bool found = it != s_dpiFactors.cend();
    if (found)
        return it.value();

    if (!screen)
        return 1.0 * userScaleFactor();

    const qreal screenDpi = screen->physicalDotsPerInch();
    qreal factor;
    if (isMobile()) {
        // Return the dpi racio against the phone where flow was prototyped in, so it looks the same.
        const qreal myOldPhoneDpi = 144.0;
        factor = screenDpi / myOldPhoneDpi;
    } else {
        // Return the dpi racio against the monitor where flow was prototyped in, so it looks the same.
        const qreal myOldMonitorDpi = 110.0;
        factor = screenDpi / myOldMonitorDpi;
    }

    factor *= userScaleFactor();

    s_dpiFactors.insert(screen, factor);
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

bool Utils::platformSupportsTooltips()
{
#ifdef QT_WIDGETS_LIB
    return !isWebGL();
#else
    return false;
#endif
}

bool Utils::isWebGL()
{
    return qApp->platformName() == QLatin1String("webgl");
}

bool Utils::platformSupportsWidgets()
{
#ifdef QT_WIDGETS_LIB
    return !isWebGL();
#else
    return false;
#endif
}

bool Utils::isValidJsonFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return false;

    QByteArray contents = f.readAll();
    if (contents.isEmpty())
        return false;

    auto document = QJsonDocument::fromJson(contents);
    return !document.isNull();
}
