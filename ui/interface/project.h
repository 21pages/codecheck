#ifndef OPENPROJCET_H
#define OPENPROJCET_H

#include <QObject>
#include <QJsonObject>
#include <QFutureWatcher>
#include "projectfile.h"


#define OPENPROJECT_REGISTER {   \
     qmlRegisterUncreatableType<CC::Project>("CC", 1, 0, "Project", QStringLiteral("")); \
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
//        Q_PROPERTY(ProjType type READ )
        Q_INVOKABLE void open(const QString &filepath);
        Q_INVOKABLE void create(const QJsonObject& obj);
//        Q_INVOKABLE ProjType type(){return m_type;}
//        Q_INVOKABLE void setType(ProjType type) { m_type = type;}
        static Project* instance();
    private:
        void watcher_open_finished();
        void watcher_create_finished();
        void setProjectFile(ProjectFile *projectFile);
    private:
        static Project *Instance;
        QFutureWatcher<void> *m_watcher_open;
        QFutureWatcher<bool> *m_watcher_create;
    };
}



#endif // OPENPROJCET_H
