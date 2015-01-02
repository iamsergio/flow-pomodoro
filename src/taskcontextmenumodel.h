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

#ifndef TASK_CONTEXT_MENUMODEL_H
#define TASK_CONTEXT_MENUMODEL_H

#include <QAbstractListModel>
#include <QHash>

class Task;

class TaskContextMenuModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool tagOnlyMenu READ tagOnlyMenu NOTIFY tagOnlyMenuChanged)
public:

    enum Role {
        TextRole = Qt::UserRole + 1,
        IconRole,
        CheckableRole,
        DismissRole
    };

    enum OptionType {
        OptionTypeNewTag = 0,
        OptionTypeEdit,
        OptionTypeDelete,
        OptionTypeQueue,
        OptionTypeCount
    };
    Q_ENUMS(OptionType)

    struct Option {
        QString text;
        QString icon;
        bool dismiss;
    };

    explicit TaskContextMenuModel(Task *task, QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int count() const;

    bool tagOnlyMenu() const;
    void setTagOnlyMenu(bool onlyTags);

Q_SIGNALS:
    void countChanged();
    void tagOnlyMenuChanged();

private Q_SLOTS:
    void onModelAboutToBeReset();
    void onModelReset();
    void onLayoutAboutToChange();
    void onLayoutChanged();
    void onDataChanged(const QModelIndex &, const QModelIndex &);
    void onRowsAboutToBeInserted(const QModelIndex &, int start, int end);
    void onRowsInserted();
    void onRowsAboutToBeRemoved(const QModelIndex &, int start, int end);
    void onRowsRemoved();

private:
    void toggleTag(int index);
    int rowOffset() const;
    QVariant staticData(OptionType row, int role) const;
    Task *m_task;
    QHash<OptionType, Option> m_staticOptions;
    Option m_moveToTodayOption;
    Option m_archiveOption;
    bool m_tagOnlyMenu;

    friend class SortedTaskContextMenuModel;
};

#endif
