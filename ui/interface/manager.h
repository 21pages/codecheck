#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QSettings>

class MainWindow;
class ResultsView;

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
    static Manager *instance();
public:
    QSettings* settings;
    MainWindow* mainWindow;
    ResultsView* resultView;
private:
//    Manager *sInstance;
};

#endif // MANAGER_H
