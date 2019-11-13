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
    bool listAddErrorItem(const ErrorItem &item);
public:
    QSettings* settings;
    MainWindow* mainWindow;
    ResultsView* resultView;
    ResultsTree *resultsTree;
private:
    static Manager *sInstance;
    QList<QSharedPointer<ErrorItem>> errorItemList;

    //for provideer
    QStringList mHiddenMessageId;
    QString mFilter;
    bool mHasVisibleErrors = false;
};

#endif // MANAGER_H
