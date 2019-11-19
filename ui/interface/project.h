#ifndef OPENPROJCET_H
#define OPENPROJCET_H

#include <QObject>
#include <QJsonObject>
#include <QFutureWatcher>

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
        enum ProjType{
            ProjTypeVS,
            ProjTypeDIR,
        };
        Q_ENUM(ProjType)
        explicit Project(QObject *parent = nullptr);
        Q_INVOKABLE void open(const QString &filepath);
        Q_INVOKABLE void create();
        static Project* instance();
    private:
        void watcher_open_finished();
    private:
        static Project *Instance;
        QFutureWatcher<void> *m_watcher_open;
    };
}



#endif // OPENPROJCET_H
