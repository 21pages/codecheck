#include "provider.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QtConcurrent>
#include <QMetaObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "manager.h"
#include "codeeditor.h"
#include "codeeditorstyle.h"
#include "resultsview.h"
#include "helper.h"
#include "iglobal.h"
#include "printreport.h"
#include "project.h"

namespace CC {

Provider *Provider::Instance = nullptr;

Provider::Provider( QObject* parent )
    : QObject( parent )
      ,m_printReport(new PrintReport(this))
    ,m_exeDir(QApplication::applicationDirPath())
{
    m_watcher_listClick = new QFutureWatcher<QVariant>(this);
    m_watcher_statistics = new QFutureWatcher<QJsonObject>(this);
    connect(m_watcher_listClick,&QFutureWatcher<QVariant>::finished,this,&Provider::watchFinished_listClick);
    connect(m_watcher_statistics,&QFutureWatcher<QJsonObject>::finished, this, &Provider::watchFinished_statistics,Qt::QueuedConnection);
    connect(this, &Provider::typeShowChanged, this, &Provider::data2ui);
    connect(this, &Provider::searchChanged,this,&Provider::data2ui);
    connect(m_printReport,&PrintReport::print_finished, this, &Provider::print_finished,Qt::QueuedConnection);
}

void Provider::watchFinished_listClick()
{
    QObject *root = Manager::instance()->engine()->rootObjects().first();
    if(root) {
        QMetaObject::invokeMethod(root,"textSelect",Qt::QueuedConnection,Q_ARG(QVariant,m_watcher_listClick->result()));
    }
}

void Provider::watchFinished_statistics()
{
    emit statistics(m_watcher_statistics->result());
}

Provider *Provider::instance()
{
    if(!Instance) {
        Instance = new Provider;
    }
    return Instance;
}

void Provider::data2ui()
{
    clearItem();
    QStringList list;
    if(m_type_show & (0x01 << 0))list<<CONST_error;
    if(m_type_show & (0x01 << 1))list<<CONST_warning;
    if(m_type_show & (0x01 << 2))list<<CONST_style;
    if(m_type_show & (0x01 << 3))list<<CONST_performance;
    if(m_type_show & (0x01 << 4))list<<CONST_portability;
    if(m_type_show & (0x01 << 5))list<<CONST_information;
    for(auto i : Manager::instance()->errorJsonList) {
        const QString file = i->value(CONST_file).toString();
        const QString severityStr = i->value(CONST_severityStr).toString();
        const QString id =  i->value(CONST_id).toString();
        const int line =  i->value(CONST_line).toInt();
        const QString summary = i->value(CONST_summary).toString();
        const QJsonArray array =  i->value(CONST_array).toArray();
        if(list.indexOf(severityStr) == -1) {
            continue;
        }
        if(m_search != "") {
            if(file.contains(m_search,Qt::CaseInsensitive)
                    || severityStr.contains(m_search,Qt::CaseInsensitive)
                    || id.contains(m_search,Qt::CaseInsensitive)
                    || summary.contains(m_search,Qt::CaseInsensitive)) {
                //do nothing
            } else {
                continue;
            }
        }
        addItem(file,severityStr,id,line,summary,array);
    }
}
void Provider::addItem(const QString& file, const QString& severity,
                       const QString& id, int line, const QString& summary,const QJsonArray &array) {
    auto item = QSharedPointer<DataItemRO>( new DataItemRO(
                                                file, severity, id, line, summary,array));
    m_items << item;
}

void Provider::removeItem(DataItemRO *item) {
    for(int i = 0; i < m_items.size(); i++) {
        if(m_items.at(i).data() == item) {
            m_items.removeAt(i);
            break;
        }
    }
}

void Provider::clearItem()
{
    m_items.clear();
}

void Provider::initProviderFromUI(QJsonObject obj)
{
    QTextDocument *doc = m_document->textDocument();
    Highlighter *highLighter = new Highlighter(doc,new CodeEditorStyle(defaultStyleLight));
    Q_UNUSED(highLighter)

    if(obj.contains("typeShow")) {
        int typeShow = obj.value("typeShow").toInt();
        if(typeShow <= 0x3F) {setTypeShow(typeShow);}
    }
}

void Provider::onListViewClicked(const QJsonObject& obj)
{
    auto  future = QtConcurrent::run(QThreadPool::globalInstance(),[obj](){
        QString fileName = obj.value("file").toString();
        int line = obj.value("line").toInt();
        QString codec = obj.value("codec").toString();
        QJsonObject retobj = Helper::getShowContent(fileName,codec,line);
        return QVariant::fromValue<QJsonObject>(retobj);

    });
    m_watcher_listClick->setFuture(future);
}

void Provider::getStatistic()
{
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[](){
        QHash<QString,int> severity;
        QHash<QString,int> id;
         for(auto i : Manager::instance()->errorJsonList) {
             const QString severityStr = i->value(CONST_severityStr).toString();
             const QString idStr =  i->value(CONST_id).toString();
             if(severity.keys().contains(severityStr)) {
                severity[severityStr]++;
             } else {
                 severity.insert(severityStr,1);
             }
             if(id.keys().contains(idStr)) {
                 id[idStr]++;
             } else {
                 id.insert(idStr,1);
             }
         }
         QJsonObject severityObj;
         for(auto it = severity.constBegin(); it != severity.constEnd(); it++) {
             severityObj.insert(it.key(),it.value());
         }
         QJsonObject idObj;
         for(auto it = id.constBegin(); it != id.constEnd();it++) {
             idObj.insert(it.key(),it.value());
         }
         QJsonObject obj;
         obj.insert("severity",severityObj);
         obj.insert("id",idObj);

         return obj;
    });
    m_watcher_statistics->setFuture(future);
}

void Provider::print()
{
    QJsonObject obj = Project::instance()->projectInfo();
    QString dir = obj.value("dir").toString();
    if(!dir.endsWith("/")) {
        dir += "/";
    }
    QString fileName = dir + obj.value("name").toString() + "-report.pdf";
    m_printReport->setFile(fileName);
    m_printReport->startPrint();
}

}
