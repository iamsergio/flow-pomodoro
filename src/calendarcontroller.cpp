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

#include "calendarcontroller.h"

enum {
    NumDaysInGrid = 6 * 7
};

CalendarController::CalendarController(QObject *parent)
    : QObject(parent)
    , m_currentDate(QDate::currentDate())
{
    updateDateList();
}

QDate CalendarController::currentDate() const
{
    return m_currentDate;
}

QDate CalendarController::today() const
{
    return QDate::currentDate();
}

void CalendarController::setCurrentDate(QDate date)
{
    if (m_currentDate != date) {
        m_currentDate = date;

        if (date > m_dateListForGrid.constLast().toDate() ||
            date < m_dateListForGrid.constFirst().toDate()) {
            updateDateList();
        }

        emit currentDateChanged(date);
    }
}

QString CalendarController::dateText() const
{
    return m_currentDate.toString(QStringLiteral("MMMM yyyy"));
}

QVariantList CalendarController::dateList() const
{
    return m_dateListForGrid;
}

QDate CalendarController::shownDayOne() const
{
    QDate firstGridDay = m_dateListForGrid.constFirst().toDate();
    if (firstGridDay.day() == 1) {
        return firstGridDay;
    } else {
        QDate d = firstGridDay.addMonths(1);
        return d.addDays(-d.day() + 1);
    }
}

void CalendarController::addMonths(int n)
{
    setCurrentDate(m_currentDate.addMonths(n));
}

void CalendarController::addYears(int n)
{
    setCurrentDate(m_currentDate.addYears(n));
}

void CalendarController::updateDateList()
{
    const QDate dayOne = m_currentDate.addDays(-m_currentDate.day() + 1);
    const int offset = dayOne.dayOfWeek() - 1;

    m_dateListForGrid.clear();
    m_dateListForGrid.reserve(NumDaysInGrid);
    for (int i = 0; i < NumDaysInGrid; ++i) {
        m_dateListForGrid << dayOne.addDays(i - offset);
    }

    emit dateListChanged();
}
