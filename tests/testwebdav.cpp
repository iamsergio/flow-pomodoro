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

#include "testwebdav.h"
#include "webdavsyncer.h"

TestWebDav::TestWebDav()
    : TestBase()
    , m_webdav(0)
{
}

void TestWebDav::initTestCase()
{
    createNewKernel("empty.dat");
    m_webdav = m_kernel->webdavSyncer();

    m_controller->setIsHttps(false);
    m_controller->setPort(55580);
    m_controller->setHost("rpi");
    m_controller->setPath("/uploads");
    m_controller->setUser("admin");
    m_controller->setPassword("12345");
    connect(m_webdav, &WebDAVSyncer::testSettingsFinished,
            this, &TestWebDav::onTestSettingsFinished);
}

void TestWebDav::cleanupTestCase()
{
}

void TestWebDav::testConnect()
{
    m_webdav->testSettings();
    waitForIt();
}

void TestWebDav::onTestSettingsFinished(bool success, const QString &errorMsg)
{
    if (!errorMsg.isEmpty()) {
        qWarning() << errorMsg;
        QVERIFY(false);
    }

    QVERIFY(success);
    stopWaiting();
}
