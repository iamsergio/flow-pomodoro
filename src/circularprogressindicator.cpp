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

#include "circularprogressindicator.h"
#include <QPainter>

CircularProgressIndicator::CircularProgressIndicator(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_dpiFactor(1)
    , m_backgroundColor(Qt::black)
    , m_foregroundColor(Qt::white)
    , m_progress(0)
{
}

void CircularProgressIndicator::paint(QPainter *painter)
{
    painter->setRenderHints(QPainter::Antialiasing, true);
    int outterPenWidth = 3 * m_dpiFactor;
    QRectF rect = boundingRect().adjusted(outterPenWidth, outterPenWidth, -outterPenWidth, -outterPenWidth);
    QBrush foregroundBrush(m_foregroundColor);
    QBrush backgroundBrush(m_backgroundColor);
    painter->setBrush(backgroundBrush);
    painter->drawEllipse(rect);

    QPen foregroundPen(m_foregroundColor);
    foregroundPen.setWidth(3 * m_dpiFactor);
    painter->setPen(m_foregroundColor);
    painter->setBrush(QBrush());
    painter->drawEllipse(rect);

    int a = 4 * m_dpiFactor;
    rect.adjust(a, a, -a, -a);
    painter->setBrush(foregroundBrush);
    int zeroHours = 90 * 16;
    painter->drawPie(rect, zeroHours, -(360*m_progress/100.0) * 16);

    a = 2 * m_dpiFactor;
    rect.adjust(a, a, -a, -a);

    painter->setPen(m_backgroundColor);
    painter->setBrush(backgroundBrush);
    painter->drawEllipse(rect);
}

int CircularProgressIndicator::dpiFactor() const
{
    return 1;
}

void CircularProgressIndicator::setDpiFactor(int dpiFactor)
{
    if (m_dpiFactor != dpiFactor) {
        m_dpiFactor = dpiFactor;
        update();
        emit dpiFactorChanged();
    }
}

QColor CircularProgressIndicator::backgroundColor() const
{
    return m_backgroundColor;
}

void CircularProgressIndicator::setBackgroundColor(const QColor &color)
{
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        update();
        emit backgroundColorChanged();
    }
}

QColor CircularProgressIndicator::foregroundColor() const
{
    return m_foregroundColor;
}

void CircularProgressIndicator::setForegroundColor(const QColor &color)
{
    if (m_foregroundColor != color) {
        m_foregroundColor = color;
        update();
        emit foregroundColorChanged();
    }
}

void CircularProgressIndicator::setDrawOutterBorder(bool enable)
{
    if (enable != m_drawOutterBorder) {
        m_drawOutterBorder = enable;
        update();
        emit drawOutterBorderChanged();
    }
}

bool CircularProgressIndicator::drawOutterBorder() const
{
    return m_drawOutterBorder;
}

void CircularProgressIndicator::setProgress(int progress)
{
    if (progress < 0 || progress > 100) {
        qWarning() << "Invalid progress value" << progress;
        Q_ASSERT(false);
        return;
    }

    if (m_progress != progress) {
        m_progress = progress;
        update();
        emit progressChanged();
    }
}

int CircularProgressIndicator::progress() const
{
    return m_progress;
}
