#include "project.h"
#include <QFileDialog>
#include <QDebug>
#include <QThreadPool>
#include <QtConcurrent>
#include <QFuture>
#include <QJsonValue>
#include <QFileInfo>
#include <QDir>
#include "manager.h"
#include "mainwindow.h"
#include "provider.h"
#include "iglobal.h"

using namespace CC;

Project* Project::Instance = new Project();
Project::Project(QObject *parent) : QObject(parent)
{
    m_watcher_open = new QFutureWatcher<void>(this);
    connect(m_watcher_open, &QFutureWatcher<void>::finished, this, &Project::watcher_open_finished);
    m_watcher_create = new QFutureWatcher<bool>(this);
    connect(m_watcher_create, &QFutureWatcher<bool>::finished, this, &Project::watcher_create_finished);
}

void CC::Project::open(const QString& filepath)
{
    QString filepath2 = filepath;
    if(filepath2.contains("file:///")) {
        filepath2.replace("file:///","");
    }
    qDebug()<<filepath2;
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[filepath2](){
        Manager::instance()->mainWindow->openProjectFile(filepath2);
    });
    m_watcher_open->setFuture(future);
}

void Project::create(const QJsonObject& obj)
{
    QFuture<bool> future = QtConcurrent::run(QThreadPool::globalInstance(), [this,obj](){
        qDebug()<<"create";

        QJsonObject obj2 = obj;
        obj2["type"] = 1;
        obj2["name"] = "hello";
        obj2["source"] = "C:/wisdom/done/56/codecheck/samples/memleak2";
        obj2["destination"] = "C:/wisdom/done/56/test";

        int type = obj2.value("type").toInt();
        QString name = obj2.value("name").toString();
        QString source = obj2.value("source").toString();
        QString destination = obj2.value("destination").toString();
        if(destination.at(destination.size() - 1) != "/") {
            destination += "/";
        }
        QString projectPath = destination + name;
        Manager::instance()->mainWindow->newProjectFile(projectPath);
        setProjectFile(Manager::instance()->mainWindow->mProjectFile,obj2);
        Manager::instance()->mainWindow->analyzeProject(Manager::instance()->mainWindow->mProjectFile);

        return true;
    });
    m_watcher_create->setFuture(future);
}

Project *Project::instance()
{
    return Instance;
}

void Project::watcher_open_finished()
{
    for(auto i : Manager::instance()->errorJsonList) {
        Provider::instance()->addItem(i->value(CONST_file).toString(),
                                      i->value(CONST_severityStr).toString(),
                                      i->value(CONST_id).toString(),
                                      i->value(CONST_line).toInt(),
                                      i->value(CONST_summary).toString(),
                                      i->value(CONST_array).toArray());
    }

}

void Project::watcher_create_finished()
{

}

void Project::setProjectFile(ProjectFile *projectFile, const QJsonObject& obj)
{
    projectFile->setBuildDir(obj.value("name").toString() + "-cppcheck-build-dir");
    QStringList libraryList;
    libraryList<<"avr"<<"boost"<<"bsd"<<"cairo"<<"cppcheck-lib"<<"cppunit"<<"daca"<<"embedded_sql" \
              <<"gnu"<<"googletest"<<"gtk"<<"libcerror"<<"libcurl"<<"lua"<<"mfc"<<"microsoft_atl"<<"microsoft_sal" \
             <<"motif"<<"nspr"<<"opengl"<<"openmp"<<"posix"<<"python"<<"qt"<<"ruby"<<"sdl"<<"sfml"<<"sqlite3"<<"std" \
            <<"tinyxml2"<<"windows"<<"wxwidgets"<<"zlib";
    projectFile->setLibraries(libraryList);
    projectFile->setCheckHeaders(true);
    int type = obj.value("type").toInt();
    QString source = obj.value("source").toString();
    if(type == ProjTypeVS) {
        projectFile->setImportProject(source);
        projectFile->setAnalyzeAllVsConfigs(true);
    } else {
        projectFile->setCheckPaths(QStringList()<<source);
    }

    QStringList list;
    list << "threadsafety"<<"y2038"<<"cert"<<"misra";
    projectFile->setAddons(list);
    projectFile->setClangAnalyzer(false);
    projectFile->setClangTidy(true);
    projectFile->write();
}
