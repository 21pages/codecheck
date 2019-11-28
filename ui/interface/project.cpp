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
#include "resultsview.h"

using namespace CC;

Project* Project::Instance = nullptr;
Project::Project(QObject *parent) : QObject(parent)
{
    connect(Manager::instance()->resultView,&ResultsView::sig_progress,this,&Project::progress,Qt::QueuedConnection);
    connect(Manager::instance(),&Manager::sig_analysisDone,this, &Project::on_create_finished,Qt::QueuedConnection);
    connect(Manager::instance()->resultView,&ResultsView::open_finished,this,&Project::on_open_finished,Qt::QueuedConnection);
}

void CC::Project::open(const QString& filepath)
{
    QString filepath2 = filepath;
    if(filepath2.contains("file:///")) {
        filepath2.replace("file:///","");
    }
    qDebug()<<filepath2;
    close();
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[this,filepath2](){
        Manager::instance()->mainWindow->openProjectFile(filepath2);
        return true;
    });
}

void Project::create(const QJsonObject& obj)
{
    close();
    QFuture<bool> future = QtConcurrent::run(QThreadPool::globalInstance(), [this,obj](){
        QJsonObject obj2 = obj;
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
}

void Project::close()
{
    Manager::instance()->clearErrorData();
    CC::Provider::instance()->clearItem();
}

Project *Project::instance()
{
    if(!Instance) {
        Instance = new Project();
    }
    return Instance;
}

void Project::on_open_finished(bool ret)
{
     CC::Provider::instance()->data2ui();
    emit openFinished(ret);
}

void Project::on_create_finished(bool ret)
{
    emit createFinished(ret);
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
