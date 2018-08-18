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

#ifndef LOADMANAGER_H
#define LOADMANAGER_H

#include <QObject>

class LoadManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool inlineEditorRequested READ inlineEditorRequested WRITE setInlineEditorRequested NOTIFY inlineEditorRequestedChanged)
    Q_PROPERTY(bool questionPopupRequested READ questionPopupRequested WRITE setQuestionPopupRequested NOTIFY questionPopupRequestedChanged)
    Q_PROPERTY(bool archiveRequested READ archiveRequested WRITE setArchiveRequested NOTIFY archiveRequestedChanged)
    Q_PROPERTY(bool taskListRequested READ taskListRequested WRITE setTaskListRequested NOTIFY taskListRequestedChanged)
    Q_PROPERTY(bool configurePopupRequested READ configurePopupRequested WRITE setConfigurePopupRequested NOTIFY configurePopupRequestedChanged)
    Q_PROPERTY(bool configurePageRequested READ configurePageRequested WRITE setConfigurePageRequested NOTIFY configurePageRequestedChanged)
    Q_PROPERTY(bool aboutPageRequested READ aboutPageRequested WRITE setAboutPageRequested NOTIFY aboutPageRequestedChanged)
    Q_PROPERTY(bool taskContextMenuRequested READ taskContextMenuRequested WRITE setTaskContextMenuRequested NOTIFY taskContextMenuRequestedChanged)
public:
    explicit LoadManager(QObject *parent = nullptr);

    void setTaskContextMenuRequested(bool);
    bool taskContextMenuRequested() const;

    bool useDelayedLoading() const;

    void setAboutPageRequested(bool);
    bool aboutPageRequested() const;

    void setConfigurePageRequested(bool);
    bool configurePageRequested() const;

    void setConfigurePopupRequested(bool);
    bool configurePopupRequested() const;

    void setArchiveRequested(bool);
    bool archiveRequested() const;

    void setTaskListRequested(bool);
    bool taskListRequested() const;

    void setQuestionPopupRequested(bool);
    bool questionPopupRequested() const;

    void setInlineEditorRequested(bool);
    bool inlineEditorRequested() const;

Q_SIGNALS:
    void inlineEditorRequestedChanged();
    void questionPopupRequestedChanged();
    void archiveRequestedChanged();
    void taskListRequestedChanged();
    void configurePopupRequestedChanged();
    void configurePageRequestedChanged();
    void aboutPageRequestedChanged();
    void taskContextMenuRequestedChanged();

private:

    bool m_taskContextMenuRequested;
    bool m_aboutPageRequested;
    bool m_configurePageRequested;
    bool m_configurePopupRequested;
    bool m_archiveRequested;
    bool m_taskListRequested = true;
    bool m_questionPopupRequested;
    bool m_inlineEditorRequested;
};

#endif
