#include "manager.h"
#include <QApplication>
#include <QJsonObject>
#include "translationhandler.h"
#include "mainwindow.h"
#include "resultsview.h"
#include "resultstree.h"
#include "treemodel.h"
#include "provider.h"
#include "helper.h"

Manager *Manager::sInstance = new Manager(nullptr);

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
    return sInstance;
}

void Manager::addErrorItem(const ErrorItem &item)
{
    CC::TreeModel::instance()->addErrorItem(item);
    listAddErrorItem(item);
}

void Manager::beginResetModel()
{
    CC::TreeModel::instance()->BeginResetModel();
}

void Manager::endResetModel()
{
    CC::TreeModel::instance()->EndResetModel();
}

bool Manager::listAddErrorItem(const ErrorItem &item)
{
    if (item.errorPath.isEmpty()) {
        return false;
    }
    const QErrorPathItem &loc = item.errorId.startsWith("clang") ? item.errorPath.front() : item.errorPath.back();
    QString relativeFile = Helper::GetStripPath(loc.file);
    if (relativeFile.isEmpty()) {
        relativeFile = tr("Undefined file");
    }
    bool hide = false;
    if (mHiddenMessageId.contains(item.errorId)) {
        hide = true;
    }
    if (!hide && !mFilter.isEmpty()) {
        if (!item.summary.contains(mFilter, Qt::CaseInsensitive) &&
            !item.message.contains(mFilter, Qt::CaseInsensitive) &&
            !item.errorPath.back().file.contains(mFilter, Qt::CaseInsensitive) &&
            !item.errorId.contains(mFilter, Qt::CaseInsensitive)) {
            hide = true;
        }
    }
    if (!hide) {
        mHasVisibleErrors = true;
    }

    for(auto i : errorItemList) {
        if(ErrorItem::sameCID(*i.get(),item)) { return false;}
    }
    QString iconStr;
    QString severityStr;
    Helper::Severity2Icon(item.severity,iconStr,severityStr);
    QJsonArray array;
    if (item.errorPath.size() > 1) {
        for (int i = 0; i < item.errorPath.size(); i++) {
            const QErrorPathItem &e = item.errorPath[i];
            QJsonObject obj;
            obj.insert("file",e.file);
            obj.insert("line",e.line);
            obj.insert("column",e.column);
            obj.insert("info",e.info);
            array.append(obj);
        }
    }
    CC::Provider::instance()->addItem(loc.file,severityStr,item.errorId,loc.line,item.summary,array);
    errorItemList<< QSharedPointer<ErrorItem>(new ErrorItem(item));
    return true;
}

void Manager::setEngine(QQmlApplicationEngine *engine)
{
    Engine = engine;
}

QQmlApplicationEngine *Manager::engine()
{
    return Engine;
}

QObject *Manager::findChild(QString objname)
{
    return reinterpret_cast<QObject*>(Engine->rootObjects().first()->findChild<QObject*>(objname)) ;
}
