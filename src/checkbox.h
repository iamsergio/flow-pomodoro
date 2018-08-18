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

#ifndef FLOW_CHECK_BOX_H
#define FLOW_CHECK_BOX_H

#include <QObject>

class CheckBoxImpl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
public:
    explicit CheckBoxImpl(QObject *parent = nullptr);

    bool checked() const;
    void setChecked(bool);

public Q_SLOTS:
    void toggle();

Q_SIGNALS:
    void checkedChanged();

private:
    bool m_checked;
};

#endif
