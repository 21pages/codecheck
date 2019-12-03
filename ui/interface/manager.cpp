#include "manager.h"
#include <QApplication>
#include <QJsonObject>
#include "translationhandler.h"
#include "mainwindow.h"
#include "resultsview.h"
#include "resultstree.h"
#include "provider.h"
#include "helper.h"
#include "iglobal.h"
#include "projectfile.h"
#include "report.h"
#include "project.h"

Manager *Manager::sInstance = nullptr;

Manager::Manager(QObject *parent) : QObject(parent)
{
    QCoreApplication *app = QApplication::instance();
    qRegisterMetaType<ErrorItem>("ErrorItem");
    settings = new QSettings("WSD", "codecheck", app);
    settings->setValue("DATADIR", "DATADIR");
    mainWindow = new MainWindow(new TranslationHandler(this), settings);
    resultView = new ResultsView(this);
    resultsTree = new ResultsTree(this);
    mainWindow->mThread->initialize(resultView);
}

Manager *Manager::instance()
{
    if(!sInstance) {
        sInstance = new Manager(nullptr);
    }
    return sInstance;
}

void Manager::addErrorItem(const ErrorItem &item)
{
    listAddErrorItem(item);
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
//    CC::Provider::instance()->addItem(loc.file,severityStr,item.errorId,loc.line,item.summary,array);
    errorJsonList<<QSharedPointer<QJsonObject>(new QJsonObject({
                                                                   {CONST_file,loc.file},
                                                                   {CONST_severityStr,severityStr},
                                                                   {CONST_id,item.errorId},
                                                                   {CONST_line,loc.line},
                                                                   {CONST_summary,item.summary},
                                                                   {CONST_array,array}
                                                               }));
    errorItemList<< QSharedPointer<ErrorItem>(new ErrorItem(item));
    return true;
}

void Manager::clearErrorData()
{
    errorJsonList.clear();
    errorItemList.clear();
}

void Manager::setEngine(QQmlApplicationEngine *engine)
{
    Engine = engine;
}

QQmlApplicationEngine *Manager::engine()
{
    return Engine;
}

QObject *Manager::findQuick(QString objname)
{
    return reinterpret_cast<QObject*>(Engine->rootObjects().first()->findChild<QObject*>(objname)) ;
}

void Manager::saveResult(Report *report)
{
    for(auto item:errorItemList) {
        report->writeError(*item);
    }

}

void Manager::analysisDone()
{
    ProjectFile *pf = mainWindow->mProjectFile;
    if (pf && !pf->getBuildDir().isEmpty()) {
        const QString prjpath = QFileInfo(pf->getFilename()).absolutePath();
        const QString buildDir = prjpath + '/' + pf->getBuildDir();
        if (QDir(buildDir).exists()) {
//            resultView->saveStatistics(buildDir + "/statistics.txt");
            resultView->updateFromOldReport(buildDir + "/lastResults.xml");
            resultView->save(buildDir + "/lastResults.xml", Report::XMLV2);
            CC::Provider::instance()->data2ui();
        }
        QDir dir(buildDir);
        QFileInfoList infoList = dir.entryInfoList();
//        QRegExp exp("^[\S\s]+.a\d+$");
        for(const QFileInfo& info:infoList) {
            QString fileName = info.fileName();
//            if(exp.exactMatch(fileName)) {
//                QFile::remove(info.absolutePath());
//            }
            if(fileName.endsWith(".a1")) {
                QFile::remove(info.filePath());
            }
        }
        emit sig_analysisDone(true);
    }
}
