#ifndef OPENPROJCET_H
#define OPENPROJCET_H

#include <QObject>
#include <QJsonObject>

namespace CC {
    class OpenProjcet : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(OpenProjcet)
    public:
        explicit OpenProjcet(QObject *parent = nullptr);

        Q_INVOKABLE QJsonObject open();
    };
}



#endif // OPENPROJCET_H
