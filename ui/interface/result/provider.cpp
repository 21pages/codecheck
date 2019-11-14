#include "provider.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include "manager.h"
#include "codeeditor.h"
#include "codeeditorstyle.h"
#include "resultsview.h"

namespace CC {

Provider *Provider::Instance = new Provider;

Provider::Provider( QObject* parent )
    : QObject( parent ) {
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

void Provider::initHighlighter(const QString& fileName)
{
    qDebug()<<"fileName:"<<fileName;
    QString tmp = fileName;
    tmp.replace("\\","/");
//    QString fullFileName = Manager::instance()->resultView->getCheckDirectory() + "/" + tmp;
    QString fullFileName = QString("C:/wisdom/done/56/codecheck") + "/" +tmp;
    qDebug()<<fullFileName;
    QFile file(fullFileName);
    QTextDocument *doc = m_document->textDocument();
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        doc->setPlainText(QString());
    }
    doc->setPlainText(QString::fromUtf8(file.readAll()));
    file.close();
    Highlighter *highLighter = new Highlighter(doc,new CodeEditorStyle(defaultStyleLight));
    Q_UNUSED(highLighter)
}

}
