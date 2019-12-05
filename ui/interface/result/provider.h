#pragma once

#include "dataitemro.h"
#include "qobject_list_model.h"
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QQuickTextDocument>
#include <QFutureWatcher>

#define Provider_Register \
{       \
   qmlRegisterUncreatableType<CC::DataItemRO>("CC",1,0,"DataItemRO","interface"); \
   qmlRegisterUncreatableType<CC::QObjectListModel_DataItemRO>("CC",1,0,"QObjectListModel_DataItemRO","interface"); \
    auto provider = CC::Provider::instance();                                   \
    engine.rootContext()->setContextProperty("provider", provider);   \
    QStringList dirveList; \
    for(auto info :QDir::drives()) {dirveList<<info.path();} \
    engine.rootContext()->setContextProperty("driveModel",dirveList);\
}

// "app" namespace is defined on purpose to show the case of using
// list model within namespaces

namespace CC {

//    DECLARE_Q_OBJECT_LIST_MODEL( DataItem )
DECLARE_Q_OBJECT_LIST_MODEL( DataItemRO)
class PrintReport;

// Provider is an example of class with some business logic that
// needs to expose some collection of items of class DataItem
class Provider : public QObject {
    Q_OBJECT
    Q_PROPERTY( QObjectListModel_DataItemRO* items READ items CONSTANT )
    Q_PROPERTY(QQuickTextDocument* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QString exeDir READ exeDir CONSTANT)
public:
    Q_INVOKABLE void addItem(const QString& file, const QString& severity,
                             const QString& id, int line, const QString& summary, const QJsonArray &array);
    Q_INVOKABLE void removeItem(DataItemRO *item);
    Q_INVOKABLE void clearItem();
    Q_INVOKABLE void initProviderFromUI(QJsonObject obj);
    Q_INVOKABLE void onListViewClicked(const QJsonObject &obj);
    Q_INVOKABLE void getStatistic(int all0filter1);
    Q_INVOKABLE void print(int all0filter1);
    Q_PROPERTY(int typeShow READ typeShow WRITE setTypeShow NOTIFY typeShowChanged)
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)
public:
    int typeShow() {return m_type_show;}
    void setTypeShow(int typeShow) {if(m_type_show != typeShow) {m_type_show = typeShow; emit typeShowChanged();}}
    QString search(){return m_search;}
    void setSearch(QString search) {search = search.trimmed(); if(m_search != search){m_search = search; emit searchChanged();}}
    static Provider* instance();
    void data2ui();
    QString exeDir(){return m_exeDir;}
    friend class PrintReport;
private:
    explicit Provider( QObject* parent = Q_NULLPTR );
    void watchFinished_listClick();
    void watchFinished_statistics();
    void onTypeShowChanged();
signals:
    void documentChanged();
    void typeShowChanged();
    void searchChanged();
    void statistics(QJsonObject obj);
    void print_finished();
private:
    // Since this getter is not safe (ownership remains to c++)
    // and it is used for QML only it'd better to make it private.
    QObjectListModel_DataItemRO* items() {return &m_items;}
    QObjectListModel_DataItemRO  m_items;
    QQuickTextDocument *document() {return m_document;}
    void setDocument(QQuickTextDocument *document) {m_document = document;}
    QQuickTextDocument *m_document;
    static Provider *Instance;
    QFutureWatcher<QVariant> *m_watcher_listClick;
    QFutureWatcher<QJsonObject> *m_watcher_statistics;
    int m_type_show = 0x3F;
    QString m_search = "";
    PrintReport *m_printReport;
    QString m_exeDir;
};
}
