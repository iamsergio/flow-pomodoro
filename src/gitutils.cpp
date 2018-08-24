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
#include <QProcess>

using namespace GitUtils;

static int runGit(const QStringList &args)
{
#if !defined(Q_OS_IOS)
    return QProcess::execute(QStringLiteral("git"), args);
#else
    return 0;
#endif
}

bool GitUtils::isInsideGit(const QString &filename)
{
    QFileInfo info(filename);
    if (!info.exists())
        return false;

    QDir parentDir = info.absoluteDir();
    if (parentDir.absolutePath() == filename)
        return false;

    const QString gitPath = parentDir.absolutePath() + QStringLiteral("/.git");
    QFileInfo gitInfo(gitPath);
    if (gitInfo.exists() && gitInfo.isDir())
        return true;

    return isInsideGit(parentDir.absolutePath());
}

GitUtils::GitRepo::GitRepo(const QString &repoPath)
    : m_repoPath(repoPath)
{
}

bool GitUtils::GitRepo::hasModifications(const QString &filename, bool &errorOcurred)
{
    if (!isInsideGit(filename) || filename.isEmpty())
        return false;

    int result = runGit({ QStringLiteral("-C"),
                          m_repoPath,
                          QStringLiteral("diff-index"),
                          QStringLiteral("--quiet"),
                          QStringLiteral("HEAD"),
                          filename });

    const bool hasModifications = result == 1;
    errorOcurred = result != 0 && result != 1;

    return hasModifications;
}

bool GitUtils::GitRepo::commit(const QString &message)
{
    if (message.isEmpty())
        return false;

    int result = runGit({ QStringLiteral("-C"),
                          m_repoPath,
                          QStringLiteral("commit"),
                          QStringLiteral("-m"),
                          message });
    return result == 0;
}

bool GitUtils::GitRepo::stage(const QString &filename)
{
    if (filename.isEmpty())
        return false;

    int result = runGit({ QStringLiteral("-C"),
                          m_repoPath,
                          QStringLiteral("add"),
                          filename });
    return result == 0;
}

bool GitUtils::GitRepo::push()
{
    int result = runGit({ QStringLiteral("-C"),
                          m_repoPath,
                          QStringLiteral("push") });
    return result == 0;
}

bool GitRepo::pull(bool rebase)
{
    QStringList args;
    args.reserve(4);
    args << QStringLiteral("-C") << m_repoPath << QStringLiteral("pull");
    if (rebase)
        args << QStringLiteral("--rebase");

    int result = runGit(args);
    return result == 0;
}

bool GitRepo::fetch(const QString &remote)
{
    QStringList args;
    args.reserve(4);
    args << QStringLiteral("-C") << m_repoPath << QStringLiteral("fetch");
    if (!remote.isEmpty())
        args << remote;

    int result = runGit(args);
    return result == 0;
}
