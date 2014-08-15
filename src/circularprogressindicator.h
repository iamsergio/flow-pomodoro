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

#ifndef CIRCULARPROGRESSINDICATOR_H
#define CIRCULARPROGRESSINDICATOR_H

#include <QColor>
#include <QQuickPaintedItem>

class CircularProgressIndicator : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int dpiFactor READ dpiFactor WRITE setDpiFactor NOTIFY dpiFactorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(bool drawOutterBorder READ drawOutterBorder WRITE setDrawOutterBorder NOTIFY drawOutterBorderChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit CircularProgressIndicator(QQuickItem *parent = 0);
    void paint(QPainter *painter) override;

    int dpiFactor() const;
    void setDpiFactor(int);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor &);

    void setDrawOutterBorder(bool);
    bool drawOutterBorder() const;

    void setProgress(int); // between 0 and 100
    int progress() const;

Q_SIGNALS:
    void dpiFactorChanged();
    void foregroundColorChanged();
    void backgroundColorChanged();
    void drawOutterBorderChanged();
    void setProgressChanged();
    void progressChanged();

private:
    int m_dpiFactor;
    QColor m_backgroundColor;
    QColor m_foregroundColor;
    bool m_drawOutterBorder;
    int m_progress;
};

#endif
