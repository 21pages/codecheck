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
        Q_INVOKABLE void open(const QString &filepath);
        Q_INVOKABLE void create(const QJsonObject& obj);
        Q_INVOKABLE void close();
        static Project* instance();
        void setProjectFile(ProjectFile *projectFile, const QJsonObject &obj);
        void data2ui();
    signals:
        void openFinished(bool ret);
        void createFinished(bool ret);
    private:
        void watcher_open_finished();
        void watcher_create_finished();
    private:
        static Project *Instance;
        QFutureWatcher<bool> *m_watcher_open;
        QFutureWatcher<bool> *m_watcher_create;
    };
}



#endif // OPENPROJCET_H
