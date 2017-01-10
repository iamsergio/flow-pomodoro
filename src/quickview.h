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

#ifndef FLOW_QUICK_WINDOW_H
#define FLOW_QUICK_WINDOW_H

#include <QQuickView>
#include <QUrl>

class Controller;
class Kernel;
class QKeyEvent;

class QuickView : public QQuickView {
    Q_OBJECT
    Q_PROPERTY(int contractedHeight READ contractedHeight WRITE setContractedHeight NOTIFY contractedHeightChanged)
    Q_PROPERTY(int contractedWidth READ contractedWidth WRITE setContractedWidth NOTIFY contractedWidthChanged)
public:
    explicit QuickView(Kernel *kernel);
    ~QuickView();

    void setContractedWidth(int);
    int contractedWidth() const;

    void setContractedHeight(int);
    int contractedHeight() const;

#ifdef UNIT_TEST_RUN
    QQuickItem *itemByName(const QString &name, QQuickItem *root = 0);
    QList<QQuickItem*> itemsByName(const QString &name, QQuickItem *root = 0);
    void moveMouseTo(QQuickItem *item);
    void mouseClick(QQuickItem *item);
    void mouseClick(const QString &objectName);
    void sendText(const QString &text);
    void sendKey(int, const QString &, Qt::KeyboardModifiers modifiers = 0);
#endif

public Q_SLOTS:
    void showWidgetContextMenu(QPoint pos);

protected:
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void toggleVisible();
    void setupWindowFlags();
    void onWindowGeometryTypeChanged();
    void setupSize();
    void dumpDebug();

Q_SIGNALS:
    void contractedHeightChanged();
    void contractedWidthChanged();

private:
    void reloadQML();
    void setupGeometry();
    QUrl styleFileName() const;
    void createStyleComponent();

    Controller *m_controller;
    const bool m_useqresources;
    Kernel *m_kernel;
    int m_contractedWidth;
    int m_contractedHeight;
};

#endif
