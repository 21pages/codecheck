#ifndef OPENPROJCET_H
#define OPENPROJCET_H

#include <QObject>
#include <QJsonObject>

#define OPENPROJECT_REGISTER {   \
    qmlRegisterType<CC::Project>("CC",1,0,"ProjectManager");    \
}

namespace CC {
    class Project : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Project)
    public:
        explicit Project(QObject *parent = nullptr);
        Q_INVOKABLE QJsonObject open();
        Q_INVOKABLE QJsonObject create();
    };
}



#endif // OPENPROJCET_H
