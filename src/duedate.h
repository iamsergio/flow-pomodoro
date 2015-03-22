/*
  This file is part of Flow.

  Copyright (C) 2015 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_DUE_DATE_H
#define FLOW_DUE_DATE_H

#include <QDate>
#include <QVariant>

/**
 * A class that bundles a QDate with a recurrence period.
 */

class DueDate {
public:
    enum PeriodType {
        PeriodTypeNone = 0,
        PeriodTypeDaily,
        PeriodTypeWeekly,
        PeriodTypeMonthly,
        PeriodTypeYearly,
        PeriodTypeCount
    };

    DueDate();
    DueDate(const QDate &date, PeriodType = PeriodTypeNone, uint frequency = 1);

    void setPeriodType(PeriodType);
    PeriodType periodType() const;
    uint frequency() const;
    void setFrequency(uint);

    QDate date() const;
    qint64 toJulianDay() const;

    bool recurs() const;
    bool isValid() const;
    QDate nextOcurrence() const;
    void makeNext();

    QString frequencyWord() const;
    QString recurrenceString() const;

    bool operator==(const DueDate &) const;
    bool operator!=(const DueDate &) const;

private:
    QDate m_date;
    PeriodType m_periodType;
    uint m_frequency;
};

#endif
