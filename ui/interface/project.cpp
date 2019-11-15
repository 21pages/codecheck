#include "project.h"
#include <QFileDialog>
#include "manager.h"
#include "mainwindow.h"

using namespace CC;
Project::Project(QObject *parent) : QObject(parent)
{

}

QJsonObject CC::Project::open()
{
    Manager::instance()->mainWindow->openProjectFile();
    return QJsonObject();
}

QJsonObject Project::create()
{
    return QJsonObject();
}
