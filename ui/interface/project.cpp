#include "project.h"
#include <QFileDialog>
#include <QDebug>
#include <QThreadPool>
#include <QtConcurrent>
#include <QFuture>
#include <QJsonValue>
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

void Project::create()
{

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
