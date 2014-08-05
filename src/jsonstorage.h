#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include "storage.h"

class JsonStorage : public Storage
{
    Q_OBJECT
public:
    explicit JsonStorage(QObject *parent);
protected:
    void load_impl() override;
    void save_impl() override;

private:
    QVariantMap toJsonVariantMap() const;
    QByteArray toJson() const;
};

#endif
