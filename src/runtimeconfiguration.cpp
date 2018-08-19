/*
  This file is part of Flow.

  Copyright (C) 2014-2016 Sérgio Martins <iamsergio@gmail.com>

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

#include "runtimeconfiguration.h"

#include <QFileInfo>
#include <QDebug>

RuntimeConfiguration::RuntimeConfiguration()
{
}

void RuntimeConfiguration::setDataFileName(const QString &dataFileName)
{
    m_dataFileName = dataFileName;
    QFileInfo info(dataFileName);
    m_flowDir = info.absolutePath();

    QFileInfo dirInfo(m_flowDir);
    if (dirInfo.isSymLink())
        m_flowDir = dirInfo.symLinkTarget();
}

QString RuntimeConfiguration::dataFileName() const
{
    return m_dataFileName;
}

QString RuntimeConfiguration::flowDir() const
{
    return m_flowDir;
}

void RuntimeConfiguration::setPluginsSupported(bool supported)
{
    m_pluginsSupported = supported;
}

bool RuntimeConfiguration::pluginsSupported() const
{
    return m_pluginsSupported;
}

Settings *RuntimeConfiguration::settings() const
{
    return m_settings;
}

void RuntimeConfiguration::setSettings(Settings *settings)
{
    m_settings = settings;
}

bool RuntimeConfiguration::saveEnabled() const
{
    return m_saveEnabled;
}

void RuntimeConfiguration::setSaveEnabled(bool enabled)
{
    m_saveEnabled = enabled;
}

bool RuntimeConfiguration::useSystray() const
{
    return m_useSystray;
}

void RuntimeConfiguration::setUseSystray(bool use)
{
    m_useSystray = use;
}

bool RuntimeConfiguration::isMobileUI() const
{
    return m_isMobileUI;
}

void RuntimeConfiguration::setMobileUI(bool is)
{
    m_isMobileUI = is;
}
