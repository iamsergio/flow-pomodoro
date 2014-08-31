#include "signalspy.h"
#include <QDebug>
#include <QMetaMethod>

SignalSpy::SignalSpy(QObject *parent)
    : QObject(parent)
{
}

void SignalSpy::clear()
{
    m_caughtSignals.clear();
}

int SignalSpy::count() const
{
    return m_caughtSignals.count();
}

QString SignalSpy::at(int index) const
{
    return m_caughtSignals.at(index);
}

void SignalSpy::onSignalTriggered()
{
    QMetaMethod method = sender()->metaObject()->method(senderSignalIndex());
    m_caughtSignals << method.name();
}
