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
    signals:
        void openFinished(bool ret);
        void createFinished(bool ret);
        void progress(int value,QString description);
    public slots:
        void on_open_finished(bool ret);
        void on_create_finished(bool ret);
    private:
        static Project *Instance;
    };
}



#endif // OPENPROJCET_H
