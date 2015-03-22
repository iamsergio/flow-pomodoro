/*
  This file is part of Flow.

  Copyright (C) 2014-2015 Sérgio Martins <iamsergio@gmail.com>

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

RuntimeConfiguration::RuntimeConfiguration()
    : m_pluginsSupported(true)
    , m_settings(0)
    , m_saveEnabled(true)
    , m_webDAVFileName("flow.dat")
    , m_useSystray(true)
{
}

void RuntimeConfiguration::setDataFileName(const QString &dataFileName)
{
    m_dataFileName = dataFileName;
}

QString RuntimeConfiguration::dataFileName() const
{
    return m_dataFileName;
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

QString RuntimeConfiguration::webDAVFileName() const
{
    return m_webDAVFileName;
}

void RuntimeConfiguration::setWebDAVFileName(const QString &name)
{
    m_webDAVFileName = name;
}

bool RuntimeConfiguration::useSystray() const
{
    return m_useSystray;
}

void RuntimeConfiguration::setUseSystray(bool use)
{
    m_useSystray = use;
}
