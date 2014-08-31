#ifndef SIGNALSPY_H
#define SIGNALSPY_H

#include <QObject>
#include <QStringList>

// Better version of QSignalSpy, which supports order and multiple signals

class SignalSpy : public QObject
{
    Q_OBJECT
public:
    explicit SignalSpy(QObject *parent = 0);

    template <typename Func1>
    inline void listenTo(const typename QtPrivate::FunctionPointer<Func1>::Object *sender,
                         Func1 signal)
    {
        connect(sender, signal, this, &SignalSpy::onSignalTriggered);

    }

    template <typename Func1>
    inline void unlistenTo(const typename QtPrivate::FunctionPointer<Func1>::Object *sender,
                           Func1 signal)
    {
        disconnect(sender, signal, this, &SignalSpy::onSignalTriggered);
    }

    void clear();
    int count() const;
    QString at(int) const;

public Q_SLOTS:
    void onSignalTriggered();

private:
    QStringList m_caughtSignals;
};

#endif
