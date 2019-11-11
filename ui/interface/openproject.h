#ifndef OPENPROJCET_H
#define OPENPROJCET_H

#include <QObject>
#include <QJsonObject>

#define OPENPROJECT_REGISTER {   \
    qmlRegisterType<CC::OpenProject>("CC",1,0,"OpenProjectManager");    \
}

namespace CC {
    class OpenProject : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(OpenProject)
    public:
        explicit OpenProject(QObject *parent = nullptr);

        Q_INVOKABLE QJsonObject open();
    };
}



#endif // OPENPROJCET_H
