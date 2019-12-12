#include "project.h"
#include <QFileDialog>
#include <QDebug>
#include <QThreadPool>
#include <QtConcurrent>
#include <QFuture>
#include <QJsonValue>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
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
    QFileInfo info(filepath2);
    if(!info.exists() || !filepath2.endsWith(".cppcheck")) {
        emit openFinished(false);
    }
    close();
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[filepath2,info,this](){
        Manager::instance()->mainWindow->openProjectFile(filepath2);

        ProjectFile *pf = Manager::instance()->mainWindow->mProjectFile;
        QString source;
        if(pf->getImportProject() != "") {
            source = pf->getImportProject();
        } else {
            QStringList sourceList = pf->getCheckPaths();
            source= sourceList.size() > 0?sourceList.at(0): "";
        }
        QString platform = pf->getPlatform();
        QString name = info.baseName();
        QString dir = info.absoluteDir().path();
        QJsonObject obj;
        qDebug()<<"open-platform-source"<<platform<<source;
        obj.insert("name",name);
        obj.insert("dir",dir);
        obj.insert("source",source);
        obj.insert("platform",platform);
        if(pf->langccpplist.size() == 3) {
            obj.insert("lang",pf->langccpplist.at(0).toInt());
            obj.insert("c",pf->langccpplist.at(1));
            obj.insert("cpp",pf->langccpplist.at(2));
        }
        setProjectInfo(obj);
        return true;
    });
}

void Project::create(const QJsonObject& obj)
{
    close();
    QFuture<bool> future = QtConcurrent::run(QThreadPool::globalInstance(), [this,obj](){
        int type = obj.value("type").toInt();
        QString name = obj.value("name").toString();
        QString source = obj.value("source").toString();
        QString destination = obj.value("destination").toString();
        if(destination.at(destination.size() - 1) != "/") {
            destination += "/";
        }
        QString projectPath = destination + name;
        Manager::instance()->mainWindow->newProjectFile(projectPath);
        setProjectFile(Manager::instance()->mainWindow->mProjectFile,obj);
        {
            ProjectFile *pf = Manager::instance()->mainWindow->mProjectFile;
            QString platform = pf->getPlatform();
            QJsonObject projobj;
            projobj.insert("name",name);
            projobj.insert("dir",destination);
            projobj.insert("platform",platform);
            projobj.insert("source",source);
            projobj.insert("lang",obj.value("lang").toInt());
            projobj.insert("c",obj.value("c").toString());
            projobj.insert("cpp",obj.value("cpp").toString());

            setProjectInfo(projobj);
        }


        Manager::instance()->mainWindow->analyzeProject(Manager::instance()->mainWindow->mProjectFile);

        return true;
    });
}

void Project::close()
{
    Manager::instance()->clearErrorData();
    CC::Provider::instance()->clearItem();
    QJsonObject projobj;
    projobj.insert("name","");
    projobj.insert("dir","");
    projobj.insert("platform","");
    projobj.insert("source","");
    projobj.insert("lang",0);
    projobj.insert("c","");
    projobj.insert("cpp","");
    setProjectInfo(projobj);
}

void Project::openFile(const QString& exepath,const QString &filepath)
{
    QtConcurrent::run(QThreadPool::globalInstance(),[exepath,filepath](){
        QString exepath2 = exepath;
        QFileInfo infoExe(exepath2);
        if(!infoExe.exists()) {
            exepath2 = "C:/Windows/System32/notepad.exe";
        }
        QProcess process;
        QFileInfo info(filepath);
        if(info.exists()) {
            process.startDetached(exepath2,QStringList()<<filepath);
        }
    });
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
    QStringList libraryList;
    libraryList<<"avr"<<"boost"<<"bsd"<<"cairo"<<"cppunit"<<"daca"<<"embedded_sql" \
              <<"gnu"<<"googletest"<<"gtk"<<"libcerror"<<"libcurl"<<"lua"<<"mfc"<<"microsoft_atl"<<"microsoft_sal" \
             <<"motif"<<"nspr"<<"opengl"<<"openmp"<<"posix"<<"python"<<"qt"<<"ruby"<<"sdl"<<"sfml"<<"sqlite3"<<"std" \
            <<"tinyxml2"<<"windows"<<"wxwidgets"<<"zlib";/*<<"cppcheck-lib"*/
    projectFile->setLibraries(libraryList);
    projectFile->setCheckHeaders(true);
    int type = obj.value("type").toInt();
    QString source = obj.value("source").toString();
    int platform = obj.value("platform").toInt();
    QStringList platformList;
    platformList<<"Unspecified"<<"win32W"<<"win64"<<"unix32"<<"unix64";
    if(platform < platformList.size()) {
        projectFile->setPlatform(platformList.at(platform));
    } else {
        projectFile->setPlatform(platformList.at(0));
    }
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
    projectFile->langccpplist = QStringList()<<QString("%1").arg(obj.value("lang").toInt())
                                                                    <<obj.value("c").toString()
                                                                   <<obj.value("cpp").toString();
    projectFile->write();
}

void Project::setProjectInfo(const QJsonObject &obj)
{
    bool changed = false;
    if(obj.contains("name")) {
        QString old_name = m_projectInfo.value("name").toString();
        QString new_name = obj.value("name").toString();
        if(old_name != new_name) {
            m_projectInfo["name"] = new_name;
            changed = true;
        }
    }
    if(obj.contains("dir")) {
        QString old_dir = m_projectInfo.value("dir").toString();
        QString new_dir = obj.value("dir").toString();
        if(old_dir != new_dir) {
            m_projectInfo["dir"] = new_dir;
            changed = true;
        }
    }
    m_projectInfo["source"] = obj["source"];
    m_projectInfo["platform"] = obj["platform"];
    m_projectInfo["lang"] = obj["lang"];
    m_projectInfo["c"] = obj["c"];
    m_projectInfo["cpp"] = obj["cpp"];
    if(changed) {
        emit projectInfoChanged();
    }
}
