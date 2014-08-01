/*
  This file is part of Flow.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "tagstorageqsettings.h"
#include "settings.h"

static const char* GROUP_TAG = "tags";
static const char* TAG_TAG = "tags";

TagStorageQSettings::TagStorageQSettings(QObject *parent)
    : TagStorage(parent)
    , m_settings(Settings::instance())
{
}

void TagStorageQSettings::loadTags_impl()
{
    m_settings->beginGroup(GROUP_TAG);
    QVariant v = m_settings->value(TAG_TAG);
    QStringList tagNames = v.toStringList();

    m_tags.clear();
    foreach (const QString &name, tagNames)
        m_tags << Tag::Ptr(new Tag(name));

    m_settings->endGroup();
}

void TagStorageQSettings::saveTags_impl()
{
    m_settings->beginGroup(GROUP_TAG);
    QStringList tagNames;
    for (int i = 0; i < m_tags.count(); ++i)
        tagNames << m_tags.at(i)->name();

    m_settings->setValue(TAG_TAG, tagNames);
    m_settings->endGroup();
    m_settings->sync();
}
