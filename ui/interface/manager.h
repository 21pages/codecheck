#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QSettings>
#include <QQmlApplicationEngine>

class MainWindow;
class ResultsView;
class ResultsTree;
class ErrorItem;
class ProjectFile;
class Report;

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
    static Manager *instance();
    void addErrorItem(const ErrorItem& item);
    bool listAddErrorItem(const ErrorItem &item);
    void clearErrorData();
    void setEngine(QQmlApplicationEngine *engine);
    QQmlApplicationEngine * engine();
    QObject* findQuick(QString objname);
    void saveResult(Report *report);
    void analysisDone();
public:
    QSettings* settings;
    MainWindow* mainWindow;
    ResultsView* resultView;
    ResultsTree *resultsTree;
    QList<QSharedPointer<QJsonObject>> errorJsonList;
private:
    static Manager *sInstance;
    QQmlApplicationEngine * Engine;
    QList<QSharedPointer<ErrorItem>> errorItemList;
    //for provideer
    QStringList mHiddenMessageId;
    QString mFilter;
    bool mHasVisibleErrors = false;
};

#endif // MANAGER_H
