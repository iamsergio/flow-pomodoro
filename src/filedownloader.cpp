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

#include "filedownloader.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

FileDownloader::FileDownloader(QObject *parent)
    : QObject(parent)
{
    connect(&m_nam, &QNetworkAccessManager::finished,
            this, &FileDownloader::onFileDownloaded);
}

bool FileDownloader::downloadInProgress() const
{
    return m_downloadInProgress;
}

void FileDownloader::downloadFile(const QUrl &url)
{
    if (downloadInProgress())
        return;

    m_downloadInProgress = true;

    QNetworkRequest request(url);
    QNetworkReply *reply = m_nam.get(request);

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, [this, reply] {
        emit downloadError(reply->errorString());
    });

    emit downloadInProgressChanged(m_downloadInProgress);
}

void FileDownloader::onFileDownloaded(QNetworkReply *reply)
{
    m_downloadInProgress = false;

    QByteArray contents = reply->readAll();
    reply->deleteLater();

    if (!contents.isEmpty()) {
        qDebug() << "File downloaded" << reply->url() << "; bytes=" << contents.size();
        emit fileDownloaded(contents);
    }

    emit downloadInProgressChanged(m_downloadInProgress);
}
