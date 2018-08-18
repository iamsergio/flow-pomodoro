/*
  This file is part of Flow.

  Copyright (C) 2018 Sérgio Martins <iamsergio@gmail.com>

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

#ifndef FLOW_FILE_DOWNLOADER_H
#define FLOW_FILE_DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>

class QNetworkReply;

class FileDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool downloadInProgress READ downloadInProgress NOTIFY downloadInProgressChanged)
public:
    explicit FileDownloader(QObject *parent = nullptr);
    bool downloadInProgress() const;
    void downloadFile(const QUrl &);
Q_SIGNALS:
    void downloadInProgressChanged(bool);
    void fileDownloaded(const QByteArray &data);
    void downloadError(const QString &);
private:
    void onFileDownloaded(QNetworkReply *reply);
    QNetworkAccessManager m_nam;
    bool m_downloadInProgress = false;
};

#endif
