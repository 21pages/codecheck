#include "provider.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QtConcurrent>
#include <QMetaObject>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <vector>
#include <algorithm>
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

void Provider::getStatistic(int all0filter1)
{
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[all0filter1,this](){
        QStringList severityList;
        QStringList idList;
        if(all0filter1 == 0) {
            for(auto obj : Manager::instance()->errorJsonList) {
                severityList << obj->value(CONST_severityStr).toString();
                idList <<  obj->value(CONST_id).toString();
            }
        } else {
            for(int i = 0; i < items()->size(); i++) {
                const auto & obj = items()->at(i);
                severityList << obj->severity;
                idList <<  obj->id;
            }
        }
        QHash<QString,int> severity;
        QHash<QString,int> id;
        QHash<QString,int> error;
        QHash<QString,int> warning;
        QHash<QString,int> style;
        QHash<QString,int> performance;
        QHash<QString,int> portability;
        QHash<QString,int> information;
        for(int i = 0; i< severityList.size(); i++) {
            const QString& severityStr = severityList.at(i);
            const QString& idStr = idList.at(i);
            //severity
            if(severity.keys().contains(severityStr)) {
               severity[severityStr]++;
            } else {
                severity.insert(severityStr,1);
            }
            //id
            if(id.keys().contains(idStr)) {
                id[idStr]++;
            } else {
                id.insert(idStr,1);
            }
            if(severityStr == CONST_error) {
                if(error.keys().contains(idStr)) {
                          error[idStr]++;
                } else {
                          error.insert(idStr , 1);
                }
            } else if(severityStr == CONST_warning) {
                if(warning.keys().contains(idStr)) {
                          warning[idStr]++;
                } else {
                          warning.insert(idStr , 1);
                }
            }else if(severityStr == CONST_style) {
                if(style.keys().contains(idStr)) {
                          style[idStr]++;
                } else {
                          style.insert(idStr , 1);
                }
            }else if(severityStr == CONST_performance) {
                if(performance.keys().contains(idStr)) {
                          performance[idStr]++;
                } else {
                          performance.insert(idStr , 1);
                }
            }else if(severityStr == CONST_portability) {
                if(portability.keys().contains(idStr)) {
                          portability[idStr]++;
                } else {
                          portability.insert(idStr , 1);
                }
            }else if(severityStr == CONST_information) {
                if(information.keys().contains(idStr)) {
                          information[idStr]++;
                } else {
                          information.insert(idStr , 1);
                }
            }

        }

         QJsonObject severityObj;
         for(auto it = severity.constBegin(); it != severity.constEnd(); it++) {
             severityObj.insert(it.key(),it.value());
         }
         QJsonObject idObj;
         if(id.size() > 10) {
             int sizeMore = id.size() - 9;
             QList<QString> idKeys = id.keys();
             QList<int> idValues = id.values();
             int otherValue = 0;
             while(sizeMore-- > 0) {
                 std::vector<int> idValuesstd = idValues.toVector().toStdVector();
                 std::vector<int>::iterator min = std::min_element(idValuesstd.begin(),idValuesstd.end());
                 int index = std::distance(idValuesstd.begin(),min);
                 otherValue += idValuesstd.at(index);
                 idValues.removeAt(index);
                 idKeys.removeAt(index);
             }
             for(int i = 0; i < idKeys.size(); i++) {
                 idObj.insert(idKeys.at(i),idValues.at(i));
             }
             idObj.insert("other",otherValue);
         } else {
             for(auto it = id.constBegin(); it != id.constEnd();it++) {
                 idObj.insert(it.key(),it.value());
             }
         }


         QJsonObject errorObj;
         int max_error = 1;
         for(auto it = error.constBegin(); it != error.constEnd(); it++) {
             errorObj.insert(it.key(),it.value());
             if(it.value() > max_error) {
                 max_error = it.value();
             }
         }
         errorObj.insert("max",max_error);

         QJsonObject warningObj;
         int max_warning = 1;
         for(auto it = warning.constBegin(); it != warning.constEnd(); it++) {
             warningObj.insert(it.key(),it.value());
             if(it.value() > max_warning) {
                 max_warning =it.value();
             }
         }
         warningObj.insert("max",max_warning);

         QJsonObject styleObj;
         int max_style = 1;
         for(auto it = style.constBegin(); it != style.constEnd(); it++) {
             styleObj.insert(it.key(),it.value());
             if(it.value() > max_style) {
                 max_style = it.value();
             }
         }
         styleObj.insert("max",max_style);

         QJsonObject performanceObj;
         int max_performance  = 1;
         for(auto it = performance.constBegin(); it != performance.constEnd(); it++) {
             performanceObj.insert(it.key(),it.value());
             if(it.value() > max_performance) {
                 max_performance = it.value();
             }
         }
         performanceObj.insert("max",max_performance);
         QJsonObject portabilityObj;
         int max_portability = 1;
         for(auto it = portability.constBegin(); it != portability.constEnd(); it++) {
             portabilityObj.insert(it.key(),it.value());
             if(it.value() > max_portability) {
                 max_portability = it.value();
             }
         }
         portabilityObj.insert("max",max_portability);
         QJsonObject informationObj;
         int max_information = 1;
         for(auto it = information.constBegin(); it != information.constEnd(); it++) {
             informationObj.insert(it.key(),it.value());
             if(it.value() > max_information) {
                 max_information = it.value();
             }
         }
         informationObj.insert("max",max_information);
         QJsonObject obj;
         obj.insert(CONST_severity,severityObj);
         obj.insert(CONST_id,idObj);
         obj.insert(CONST_error,errorObj);
         obj.insert(CONST_warning,warningObj);
         obj.insert(CONST_style,styleObj);
         obj.insert(CONST_performance,performanceObj);
         obj.insert(CONST_portability,portabilityObj);
         obj.insert(CONST_information,informationObj);

         return obj;
    });
    m_watcher_statistics->setFuture(future);
}

void Provider::print(int all0filter1)
{
    QJsonObject obj = Project::instance()->projectInfo();
    QString dir = obj.value("dir").toString();
    if(!dir.endsWith("/")) {
        dir += "/";
    }
    QString fileName = dir + obj.value("name").toString() + "-report.pdf";
    m_printReport->setFile(fileName);
    m_printReport->startPrint(all0filter1);
}

}
