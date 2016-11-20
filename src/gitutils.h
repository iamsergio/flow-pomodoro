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

#include <QObject>
#include <QString>

namespace GitUtils
{

bool isInsideGit(const QString &filename);

class GitRepo
{
public:
    explicit GitRepo(const QString &repoPath);
    bool hasModifications(const QString &filename, bool &errorOcurred);
    bool commit(const QString &message);
    bool stage(const QString &filename);
    bool push();
private:
    const QString m_repoPath;
};

}
