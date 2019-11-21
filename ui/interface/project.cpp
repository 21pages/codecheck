#include "project.h"
#include <QFileDialog>
#include <QDebug>
#include <QThreadPool>
#include <QtConcurrent>
#include <QFuture>
#include <QJsonValue>
#include <QFileInfo>
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
    QFuture<bool> future = QtConcurrent::run(QThreadPool::globalInstance(), [obj](){
        int type = obj.value("type").toInt();
        QString source = obj.value("source").toString();
        QString destination = obj.value("destination").toString();
        if(type == ProjTypeVS) {
            Manager::instance()->mainWindow->newProjectFile(destination);
        }


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

void Project::setProjectFile(ProjectFile *projectFile)
{
    QString stdLibraryFilename = "qrc:/cfg/std.cfg";
    QStringList libs;
//    QDir dir("qrc:/cfg/");
//    dir.setSorting(QDir::Name);
//    dir.setNameFilters(QStringList("*.cfg"));
//    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//    foreach (QFileInfo item, dir.entryInfoList()) {
//        QString library = item.fileName();
//        if (library.compare("std.cfg", Qt::CaseInsensitive) != 0)
//            continue;
//        Library lib;
//        const QString fullfilename = "qrc:/cfg/" + library;
//        const Library::Error err = lib.load(nullptr, fullfilename.toLatin1());
//        if (err.errorcode != Library::OK)
//            continue;
//        // Working std.cfg found
//        stdLibraryFilename = fullfilename;
//        break;
//    }

}
