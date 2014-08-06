#include "imageprovider.h"
#include <QQuickItem>

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *, const QSize &)
{
    return QPixmap(QString(":/img/16x16/%1").arg(id));
}
