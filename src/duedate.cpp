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

#include "duedate.h"
#include <QObject>

static DueDate::PeriodType capPeriod(DueDate::PeriodType type)
{
    return (type < DueDate::PeriodTypeNone || type >= DueDate::PeriodTypeCount) ? DueDate::PeriodTypeNone
                                                                                : type;
}

static uint capFrequency(uint frequency)
{
    return frequency == 0 ? 1 : frequency;
}

DueDate::DueDate()
{
}

DueDate::DueDate(QDate date, PeriodType period, uint frequency)
    : m_date(date)
    , m_periodType(capPeriod(period))
    , m_frequency(capFrequency(frequency))
{
}

void DueDate::setPeriodType(DueDate::PeriodType type)
{
    m_periodType = capPeriod(type);
}

DueDate::PeriodType DueDate::periodType() const
{
    return m_periodType;
}

uint DueDate::frequency() const
{
    return m_frequency;
}

void DueDate::setFrequency(uint frequency)
{
    m_frequency = capFrequency(frequency);
}

QDate DueDate::date() const
{
    return m_date;
}

void DueDate::setDate(QDate date)
{
    m_date = date;
}

qint64 DueDate::toJulianDay() const
{
    return m_date.toJulianDay();
}

bool DueDate::recurs() const
{
    return isValid() && m_periodType > PeriodTypeNone && m_periodType < PeriodTypeCount;
}

bool DueDate::operator==(DueDate other) const
{
    return m_periodType == other.m_periodType &&
           m_date == other.m_date &&
           m_frequency == other.m_frequency;
}

bool DueDate::operator!=(DueDate other) const
{
    return !(*this == other);
}

bool DueDate::isValid() const
{
    return m_date.isValid();
}

QDate DueDate::nextOcurrence() const
{
    if (!isValid())
        return QDate();

    switch (m_periodType) {
    case PeriodTypeNone:
        return m_date;
        break;
    case PeriodTypeDaily:
        return m_date.addDays(m_frequency);
        break;
    case PeriodTypeWeekly:
        return m_date.addDays(7 * m_frequency);
        break;
    case PeriodTypeMonthly:
        return m_date.addMonths(m_frequency);
        break;
    case PeriodTypeYearly:
        return m_date.addYears(m_frequency);
        break;
    case PeriodTypeCount:
        Q_ASSERT(false);
        return QDate();
        break;
    }

    Q_ASSERT(false);
    return QDate();
}

void DueDate::makeNext()
{
    if (recurs())
        m_date = nextOcurrence();
}

QString DueDate::frequencyWord() const
{
    if (!recurs())
        return QString();

    switch (m_periodType) {
    case DueDate::PeriodTypeNone:
    case DueDate::PeriodTypeCount:
        Q_ASSERT(false);
        return QString();
    case DueDate::PeriodTypeDaily:
        return m_frequency == 1 ? QObject::tr("day") : QObject::tr("days");
    case DueDate::PeriodTypeWeekly:
        return m_frequency == 1 ? QObject::tr("week") : QObject::tr("weeks");
    case DueDate::PeriodTypeMonthly:
        return m_frequency == 1 ? QObject::tr("month") : QObject::tr("months");
    case DueDate::PeriodTypeYearly:
        return m_frequency == 1 ? QObject::tr("year") : QObject::tr("years");
    }

    return QString();
}

QString DueDate::recurrenceString() const
{
    if (!recurs())
        return QString();

    switch (m_periodType) {
    case PeriodTypeNone:
        Q_ASSERT(false);
        return QString();
        break;
    case PeriodTypeDaily:
        return m_frequency == 1 ? QObject::tr("daily") : QObject::tr("every %1 days").arg(m_frequency);
        break;
    case PeriodTypeWeekly:
        return m_frequency == 1 ? QObject::tr("weekly") : QObject::tr("every %1 weeks").arg(m_frequency);
        break;
    case PeriodTypeMonthly:
        return m_frequency == 1 ? QObject::tr("monthly") : QObject::tr("every %1 months").arg(m_frequency);
        break;
    case PeriodTypeYearly:
        return m_frequency == 1 ? QObject::tr("yearly") : QObject::tr("every %1 years").arg(m_frequency);
        break;
    case PeriodTypeCount:
        Q_ASSERT(false);
        return QString();
        break;
    }

    return QString();
}
