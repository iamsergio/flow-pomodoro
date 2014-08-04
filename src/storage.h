/*
  This file is part of Flow.

  Copyright (C) 2014 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_STORAGE_H
#define FLOW_STORAGE_H

#include "task.h"
#include "tag.h"

#include <QObject>

class TagStorage;
class TaskStorage;

class Storage : public QObject
{
    Q_OBJECT
public:
    static Storage *instance();

    TagStorage *tagStorage();
    TaskStorage *taskStorage();

    void load();
    void save();

protected:
    GenericListModel<Tag::Ptr> m_tags;
    GenericListModel<Task::Ptr> m_tags;
    void load_impl() = 0;
    void save_impl() = 0;

private:
    explicit Storage(QObject *parent = 0);
    TagStorage *m_tagStorage;
    TaskStorage *m_taskStorage;


};

#endif
