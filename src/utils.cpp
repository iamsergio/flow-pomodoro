#include "utils.h"

#include <QDebug>
#include <QElapsedTimer>

void printTimeInfo(const QString &location)
{
#ifdef FLOW_DEBUG_TIMESTAMPS
    static QElapsedTimer timer;
    static int total = 0;
    static int count = 0;

    if (count == 0)
        timer.start();

    count++;
    const int elapsed = timer.elapsed();
    total += elapsed;

    qDebug() << "Profiler:" << location << "; step=" << elapsed << "total=" << total;
    timer.restart();
#else
    Q_UNUSED(location);
#endif
}
