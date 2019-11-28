#include "provider.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QtConcurrent>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include "manager.h"
#include "codeeditor.h"
#include "codeeditorstyle.h"
#include "resultsview.h"
#include "helper.h"
#include "iglobal.h"

namespace CC {

Provider *Provider::Instance = nullptr;

Provider::Provider( QObject* parent )
    : QObject( parent ) {
    m_watcher_listClick = new QFutureWatcher<QVariant>(this);
    connect(m_watcher_listClick,&QFutureWatcher<QVariant>::finished,this,&Provider::watchFinished_listClick);
    connect(this, &Provider::typeShowChanged, this, &Provider::data2ui);
}

void Provider::watchFinished_listClick()
{
    QObject *root = Manager::instance()->engine()->rootObjects().first();
    if(root) {
        QMetaObject::invokeMethod(root,"textSelect",Qt::QueuedConnection,Q_ARG(QVariant,m_watcher_listClick->result()));
    }
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
        if(list.indexOf(i->value(CONST_severityStr).toString()) != -1) {
            addItem(i->value(CONST_file).toString(),
                          i->value(CONST_severityStr).toString(),
                          i->value(CONST_id).toString(),
                          i->value(CONST_line).toInt(),
                          i->value(CONST_summary).toString(),
                          i->value(CONST_array).toArray());
        }
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

}
