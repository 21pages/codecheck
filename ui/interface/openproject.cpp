#include "openproject.h"
#include <QFileDialog>
#include "manager.h"
#include "mainwindow.h"

using namespace CC;
OpenProject::OpenProject(QObject *parent) : QObject(parent)
{

}

QJsonObject CC::OpenProject::open()
{
    Manager::instance()->mainWindow->openProjectFile();
    return QJsonObject();
}
