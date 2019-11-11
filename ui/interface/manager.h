#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QSettings>

class MainWindow;
class ResultsView;
class ResultsTree;
class ErrorItem;

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
    static Manager *instance();
    void addErrorItem(const ErrorItem& item);
    void beginResetModel();
    void endResetModel();
public:
    QSettings* settings;
    MainWindow* mainWindow;
    ResultsView* resultView;
    ResultsTree *resultsTree;
private:
//    Manager *sInstance;
};

#endif // MANAGER_H
