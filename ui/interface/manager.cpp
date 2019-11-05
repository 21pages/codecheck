#include "manager.h"
#include <QApplication>
#include "translationhandler.h"
#include "mainwindow.h"
#include "resultsview.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    QCoreApplication *app = QApplication::instance();
    settings = new QSettings("WSD", "codecheck", app);
    settings->setValue("DATADIR", "DATADIR");
    mainWindow = new MainWindow(new TranslationHandler(this), settings);
    resultView = new ResultsView(this);
}

Manager *Manager::instance()
{
    static Manager manager;
    return &manager;
}
