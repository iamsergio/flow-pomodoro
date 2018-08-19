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

#ifndef RUNTIMECONFIGURATION_H
#define RUNTIMECONFIGURATION_H

#include <QString>

class Settings;

class RuntimeConfiguration
{
public:
    RuntimeConfiguration();

    void setDataFileName(const QString &);
    QString dataFileName() const;

    QString flowDir() const;

    void setPluginsSupported(bool); // default true
    bool pluginsSupported() const;

    Settings *settings() const;
    void setSettings(Settings *);

    bool saveEnabled() const;
    void setSaveEnabled(bool);

    bool useSystray() const;
    void setUseSystray(bool);

    bool isMobileUI() const;
    void setMobileUI(bool);


private:
    QString m_dataFileName;
    QString m_flowDir;
    bool m_pluginsSupported = true;
    Settings *m_settings = nullptr;
    bool m_saveEnabled = true;
    bool m_useSystray = true;
    bool m_isMobileUI = false;
};

#endif
