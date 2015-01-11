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

#include "hosts.h"

#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>
#include <QFileInfo>
#include <QSettings>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickItem>
#include <QSaveFile>

const char *const s_startTag = "# Start Flow-pomodoro specific hosts, do not remove this tag";
const char *const s_endTag = "# End Flow-pomodoro specific hosts, do not remove this tag";

#if defined(Q_OS_WIN)
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

struct ScoppedSetter {
    ScoppedSetter()
    {
#if defined(Q_OS_WIN)
        qt_ntfs_permission_lookup++;
#endif
    }

    ~ScoppedSetter()
    {
#if defined(Q_OS_WIN)
    qt_ntfs_permission_lookup--;
#endif
    }
};

static QString hostsFileName()
{
#if defined(Q_OS_WIN)
    return "C:\\Windows\\System32\\drivers\\etc\\hosts";
#else
    return "/etc/hosts";
#endif
}

HostsPlugin::HostsPlugin() : QObject(), PluginInterface()
  , m_enabled(false)
  , m_qmlEngine(0)
  , m_configItem(0)
  , m_settings(0)
{
    // Fixes crash in static mode, because qqmlimport calls QPluginLoader::staticPlugins() before us.
    moveToThread(qApp->thread());
}

void HostsPlugin::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        update(m_enabled);
    }
}

bool HostsPlugin::enabled() const
{
    return m_enabled;
}

void HostsPlugin::update(bool allowDistractions)
{
    setLastError("");
    updateHosts(allowDistractions);
}

void HostsPlugin::setTaskStatus(TaskStatus status)
{
    if (m_enabled)
        update(status != TaskStarted);
}

QString HostsPlugin::text() const
{
    return QStringLiteral("Hosts");
}

QString HostsPlugin::helpText() const
{
    static QString help = tr("Blocks access to websites.\nMake sure you have write access to <b>%1</b>").arg(hostsFileName());
    return help;
}

QObject *HostsPlugin::controller()
{
    return this;
}

void HostsPlugin::setQmlEngine(QQmlEngine *engine)
{
    Q_ASSERT(!m_qmlEngine && engine);
    m_qmlEngine = engine;

    QQmlComponent *component = new QQmlComponent(engine, QUrl("qrc:/plugins/hosts/Config.qml"),
                                                 QQmlComponent::PreferSynchronous, this);

    if (component->isError()) {
        setLastError("Error creating component: " + component->errorString());
        return;
    }

    QQmlContext *subContext = new QQmlContext(engine->rootContext());
    m_configItem = qobject_cast<QQuickItem*>(component->create(subContext));
    subContext->setContextProperty("_plugin", this);

    if (!m_configItem) {
        setLastError("Error creating item");
        return;
    }
}

QQuickItem *HostsPlugin::configureItem() const
{
    return m_configItem;
}

void HostsPlugin::setSettings(QSettings *settings)
{
    Q_ASSERT(!m_settings && settings);
    m_settings = settings;
    m_settings->beginGroup("hosts");
    bool firstRun = m_settings->value("firstRun", /*default=*/ true).toBool();
    if (firstRun) {
        m_settings->setValue("firstRun", false);
        QString examples = "www.facebook.com\nwww.9gag.com\n";
        m_settings->setValue("hosts", examples);
        m_settings->sync();
    }

    setHosts(m_settings->value("hosts").toString());
    m_settings->endGroup();
}

bool HostsPlugin::enabledByDefault() const
{
    return false;
}

void HostsPlugin::setLastError(const QString &lastError)
{
    if (!lastError.isEmpty())
        qWarning() << "Hosts:" << lastError;
    if (lastError != m_lastError) {
        m_lastError = lastError;
        emit lastErrorChanged();
    }
}

QString HostsPlugin::lastError() const
{
    return m_lastError;
}

bool HostsPlugin::checkSanity()
{
    ScoppedSetter setter;

    QFileInfo info(hostsFileName());
    if (!info.exists()) {
        setLastError(tr("Hosts file doesn't exist."));
        return false;
    }

    if (!info.isWritable()) {
        setLastError(tr("Hosts file isn't writable."));
        return false;
    }

    return true;
}

void HostsPlugin::updateHosts(bool allow)
{
    if (!checkSanity())
        return;

    QFile file(hostsFileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setLastError(tr("Failed to open %1 because %2 (%3)").arg(hostsFileName(), file.errorString()).arg(file.error()));
        return;
    }

    QByteArray data;
    bool ignoring = false;
    bool foundFlowTags = false; // for optimization
    while (!file.atEnd()) {
        QString line = file.readLine();
        if (ignoring && line.trimmed() == s_endTag) {
            ignoring = false;
            foundFlowTags = true;
            continue;
        }

        if (!ignoring && line.trimmed() == s_startTag) {
            ignoring = true;
            foundFlowTags = true;
            continue;
        }

        if (ignoring)
            continue;

        data += line;
    }

    file.close();

    if (allow && !foundFlowTags) {
        // Hosts file already is prestine, nothing to do
        return;
    }

    if (!allow && !foundFlowTags && m_hosts.isEmpty()) {
        // User doesn't want to block anything, nothing to do
        return;
    }

    if (!allow) {
        data += s_startTag;
        QStringList hosts = m_hosts.split("\n");
        foreach (QString host, hosts) {
            QUrl url(host); // In case user enters http://
            host = url.host().isEmpty() ? host : url.host();
            if (!host.trimmed().isEmpty())
                data += "\n127.0.0.1 " + host;
        }

        data += "\n" + QString(s_endTag) + "\n";
    }

    QSaveFile fileWriter(hostsFileName());
    if (!fileWriter.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setLastError(tr("Failed to open %1 for saving because: %2 (%3)").arg(hostsFileName(), fileWriter.errorString()).arg(fileWriter.error()));
        return;
    }
    fileWriter.write(data);

    if (fileWriter.error() != QFileDevice::NoError) {
        setLastError(tr("Failed to write to %1 because: %2 (%3)").arg(hostsFileName(), fileWriter.errorString()).arg(fileWriter.error()));
        return;
    }

    fileWriter.commit();
}

void HostsPlugin::setHosts(const QString &hosts)
{
    if (hosts != m_hosts) {
        m_hosts = hosts;
        m_settings->beginGroup("hosts");
        m_settings->setValue("hosts", hosts);
        m_settings->endGroup();
        m_settings->sync();
        emit hostsChanged();
    }
}

QString HostsPlugin::hosts() const
{
    return m_hosts;
}
