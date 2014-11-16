/*
  This file is part of Flow.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Sérgio Martins <sergio.martins@kdab.com>

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

#ifndef _QUICK_WINDOW_H_
#define _QUICK_WINDOW_H_

#include <QQuickView>
#include <QUrl>

class Controller;
class Kernel;
class QKeyEvent;

class QuickView : public QQuickView {
    Q_OBJECT
    Q_PROPERTY(int contractedHeight READ contractedHeight WRITE setContractedHeight NOTIFY contractedHeightChanged)
    Q_PROPERTY(int contractedWidth READ contractedWidth WRITE setContractedWidth NOTIFY contractedWidthChanged)
    Q_PROPERTY(GeometryType geometryType READ geometryType WRITE setGeometryType NOTIFY geometryTypeChanged)
    Q_PROPERTY(Position initialPosition READ initialPosition WRITE setInitialPosition NOTIFY initialPositionChanged)
public:
    enum Position {
        PositionNone = 0, // Window will appear where WM puts it
        PositionLast, // Window will appear at the same place as last time. Saves position at exit.
        PositionTop,
        PositionTopLeft,
        PositionTopRight,
        PositionBottom, // TODO: Bottom is not supported yet
        PositionBottomLeft,
        PositionBottomRight,
        MaxPositions
    };
    Q_ENUMS(Position)

    enum GeometryType {
        GeometryStandard, // 400x40
        GeometryThin, // 400x20
        GeometrySmallSquare, // 80x80
        GeometryCustom, // uses width and height from settings
        MaxGeometryTypes
    };
    Q_ENUMS(GeometryType)

    explicit QuickView(Kernel *kernel);
    ~QuickView();

    // Specifies where the window will appear at startup
    Position initialPosition() const;
    void setInitialPosition(Position);

    void setGeometryType(GeometryType);
    QuickView::GeometryType geometryType() const;

    void setContractedWidth(int);
    int contractedWidth() const;

    void setContractedHeight(int);
    int contractedHeight() const;

protected:
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void toggleVisible();

Q_SIGNALS:
    void contractedHeightChanged();
    void contractedWidthChanged();
    void geometryTypeChanged();
    void initialPositionChanged();

private:
    void reloadQML();
    void setupGeometry();
    void readInitialPosition();
    void readGeometryType();
    QUrl styleFileName() const;
    void createStyleComponent();

    Controller *m_controller;
    const bool m_useqresources;
    Kernel *m_kernel;
    Position m_initialPosition;
    GeometryType m_geometryType;
    int m_contractedWidth;
    int m_contractedHeight;
};

#endif
