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

#include "gitutils.h"

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

using namespace GitUtils;

bool GitUtils::isInsideGit(const QString &filename)
{
    QFileInfo info(filename);
    if (!info.exists())
        return false;

    QDir parentDir = info.absoluteDir();

    const QString gitPath = parentDir.absolutePath() + QStringLiteral("/.git");
    QFileInfo gitInfo(gitPath);
    if (gitInfo.exists() && gitInfo.isDir())
        return true;

    return isInsideGit(parentDir.absolutePath());
}
