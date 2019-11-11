#include "manager.h"
#include <QApplication>
#include "translationhandler.h"
#include "mainwindow.h"
#include "resultsview.h"
#include "resultstree.h"
#include "treemodel.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    QCoreApplication *app = QApplication::instance();
    settings = new QSettings("WSD", "codecheck", app);
    settings->setValue("DATADIR", "DATADIR");
    mainWindow = new MainWindow(new TranslationHandler(this), settings);
    resultView = new ResultsView(this);
    resultsTree = new ResultsTree(this);
}

Manager *Manager::instance()
{
    static Manager manager;
    return &manager;
}

void Manager::addErrorItem(const ErrorItem &item)
{
    CC::TreeModel::instance()->addErrorItem(item);
}

void Manager::beginResetModel()
{
    CC::TreeModel::instance()->BeginResetModel();
}

void Manager::endResetModel()
{
    CC::TreeModel::instance()->EndResetModel();
}
