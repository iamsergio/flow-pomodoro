/*
  This file is part of Flow.

  Copyright (C) 2016 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef CALENDARCONTROLLER_H
#define CALENDARCONTROLLER_H

#include <QObject>
#include <QDate>
#include <QVariant>

class CalendarController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDate currentDate READ currentDate WRITE setCurrentDate NOTIFY currentDateChanged)
    Q_PROPERTY(QDate today READ today NOTIFY todayChanged)
    Q_PROPERTY(QString dateText READ dateText NOTIFY currentDateChanged)
    Q_PROPERTY(QVariantList dateList READ dateList NOTIFY dateListChanged)
    Q_PROPERTY(QDate shownDayOne READ shownDayOne NOTIFY dateListChanged)
public:
    explicit CalendarController(QObject *parent = nullptr);

    QDate currentDate() const;
    QDate today() const;
    void setCurrentDate(QDate);
    QString dateText() const;
    QVariantList dateList() const;
    QDate shownDayOne() const;

public Q_SLOTS:
    void addMonths(int n);
    void addYears(int n);

Q_SIGNALS:
    void currentDateChanged(QDate);
    void dateListChanged();
    void todayChanged();

private:
    void updateDateList();
    QDate m_currentDate;
    QVariantList m_dateListForGrid;
};

#endif
