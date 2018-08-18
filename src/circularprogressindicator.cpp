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
    , m_backgroundColor(Qt::black)
    , m_foregroundColor(Qt::white)
{
}

void CircularProgressIndicator::paint(QPainter *painter)
{
    if (m_value < m_minimumValue || m_value > m_maximumValue) {
        // Ignore, the object is being setup and max/mim will be setup and call update() again
        return;
    }

    painter->setRenderHints(QPainter::Antialiasing, true);
    QRectF rect = boundingRect().adjusted(m_outterBorderWidth, m_outterBorderWidth, -m_outterBorderWidth, -m_outterBorderWidth);
    QBrush foregroundBrush(m_foregroundColor);
    QBrush backgroundBrush(m_backgroundColor);
    painter->setBrush(backgroundBrush);
    painter->drawEllipse(rect);

    // Outter border:
    if (m_drawOutterBorder) {
        QPen borderPen(m_foregroundColor);
        borderPen.setWidth(m_outterBorderWidth);
        painter->setPen(borderPen);
        painter->setBrush(QBrush());
        painter->drawEllipse(rect);
    }

    int a = m_innerSpacing;
    rect.adjust(a, a, -a, -a);
    QPen borderPen(m_foregroundColor);
    borderPen.setWidth(0);
    painter->setPen(borderPen);
    painter->setBrush(foregroundBrush);
    int zeroHours = 90 * 16;
    qreal currentPercentage = (1.0 * m_value - m_minimumValue) / (m_maximumValue - m_minimumValue);
    painter->drawPie(rect, zeroHours, -360 * currentPercentage * 16);

    a = m_innerBorderWidth;
    rect.adjust(a, a, -a, -a);

    painter->setPen(m_backgroundColor);
    painter->setBrush(backgroundBrush);
    painter->drawEllipse(rect);

    if (m_showStopIcon) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(foregroundBrush);
        QPointF center = boundingRect().center();
        qreal length = boundingRect().width() / 3.4; //  3.4 because it looks nice, no particular reason
        painter->drawRect(QRectF(center.x() - (length / 2), center.y() - (length / 2), length, length));
    } else {
        // Draw text. We draw a count-down.
        int remainingValue = m_maximumValue - m_value;
        QString text = QString::number(remainingValue);
        painter->setPen(m_foregroundColor);
        QFont font = painter->font();
        font.setBold(true);
        font.setPixelSize(m_fontPixelSize);
        painter->setFont(font);
        a = -2 * m_dpiFactor; // Make rect bigger
        rect.adjust(a, a, -a, -a);
        painter->drawText(rect, Qt::AlignCenter, text);
    }
}

int CircularProgressIndicator::dpiFactor() const
{
    return m_dpiFactor;
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

void CircularProgressIndicator::setOutterBorderWidth(int width)
{
    if (width != m_outterBorderWidth) {
        m_outterBorderWidth = width;
        update();
        emit outterBorderWidthChanged();
    }
}

int CircularProgressIndicator::outterBorderWidth() const
{
    return m_outterBorderWidth;
}

void CircularProgressIndicator::setValue(int value)
{
    if (m_value != value) {
        m_value = value;
        update();
        emit valueChanged();
    }
}

int CircularProgressIndicator::value() const
{
    return m_value;
}

int CircularProgressIndicator::maximumValue() const
{
    return m_maximumValue;
}

int CircularProgressIndicator::minimumValue() const
{
    return m_minimumValue;
}

void CircularProgressIndicator::setMaximumValue(int value)
{
    if (value != m_maximumValue) {
        m_maximumValue = value;
        update();
        emit maximumValue();
    }
}

void CircularProgressIndicator::setMinimumValue(int value)
{
    if (value != m_minimumValue) {
        m_minimumValue = value;
        update();
        emit maximumValue();
    }
}

void CircularProgressIndicator::setShowStopIcon(bool show)
{
    if (show != m_showStopIcon) {
        m_showStopIcon = show;
        update();
        emit showStopIconChanged();
    }
}

bool CircularProgressIndicator::showStopIcon() const
{
    return m_showStopIcon;
}

void CircularProgressIndicator::setFontPixelSize(int size)
{
    if (m_fontPixelSize != size) {
        m_fontPixelSize = size;
        update();
        emit fontPixelSizeChanged();
    }
}

int CircularProgressIndicator::fontPixelSize() const
{
    return m_fontPixelSize;
}

int CircularProgressIndicator::innerBorderWidth() const
{
    return m_innerBorderWidth;
}

void CircularProgressIndicator::setInnerBorderWidth(int width)
{
    if (width != m_innerBorderWidth) {
        m_innerBorderWidth = width;
        update();
        emit innerBorderWidthChanged();
    }
}

int CircularProgressIndicator::innerSpacing() const
{
    return m_innerSpacing;
}

void CircularProgressIndicator::setInnerSpacing(int spacing)
{
    if (spacing != m_innerSpacing) {
        m_innerSpacing = spacing;
        update();
        emit innerSpacingChanged();
    }
}
