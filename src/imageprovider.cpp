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

#include "imageprovider.h"
#include <QScreen>
#include <QQuickItem>
#include <QGuiApplication>

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *, const QSize &)
{
    QScreen *screen = QGuiApplication::primaryScreen();

    // For touch related we're interested in physical DPI
    qreal dpi = screen->physicalDotsPerInch();

    // 0.30 inches seem decent for human fingers
    const qreal hitWidthInInches = 0.25;
    const qreal pixelWidth = hitWidthInInches * dpi;

    int iconSize = ceil(pixelWidth / 16) * 16;
    if (iconSize == 16 || iconSize == 32 || iconSize == 48) {
        return QPixmap(QString(":/img/%1x%2/%3").arg(iconSize).arg(iconSize).arg(id));
    } else {
        // we don't have higher resolution assets, so scale:
        return QPixmap(QString(":/img/48x48/%1").arg(id)).scaled(pixelWidth, pixelWidth);
    }
}
