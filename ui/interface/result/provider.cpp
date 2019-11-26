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

namespace CC {

Provider *Provider::Instance = new Provider;

Provider::Provider( QObject* parent )
    : QObject( parent ) {
    m_watcher_listClick = new QFutureWatcher<QVariant>(this);
    connect(m_watcher_listClick,&QFutureWatcher<QVariant>::finished,this,&Provider::watchFinished_listClick);
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
    return Instance;
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

void Provider::initDocument()
{
    QTextDocument *doc = m_document->textDocument();
    Highlighter *highLighter = new Highlighter(doc,new CodeEditorStyle(defaultStyleLight));
    Q_UNUSED(highLighter)
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
