#ifndef OPENPROJCET_H
#define OPENPROJCET_H

#include <QObject>
#include <QJsonObject>

//qmlRegisterType<CC::Project>("CC",1,0,"ProjectManager");    \


#define OPENPROJECT_REGISTER {   \
    auto projectManager = CC::Project::instance();                                   \
    engine.rootContext()->setContextProperty("projectManager", projectManager);   \
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
        static Project* instance();
    private:
        static Project *Instance;
    };
}



#endif // OPENPROJCET_H
