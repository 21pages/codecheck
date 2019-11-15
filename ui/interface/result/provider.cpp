#include "provider.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextCodec>
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

static int getPos(const QString &fileData, int lineNumber)
{
    if (lineNumber <= 1)
        return 0;
    for (int pos = 0, line = 1; pos < fileData.size(); ++pos) {
        if (fileData[pos] != '\n')
            continue;
        ++line;
        if (line >= lineNumber)
            return pos + 1;
    }
    return fileData.size();
}

void Provider::onListViewClicked(const QJsonObject& obj)
{
    QString fileName = obj.value("file").toString();
    int line = obj.value("line").toInt();
    QString codec = obj.value("codec").toString();
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
    QString fileContent;
    if(QString::compare(codec,"gbk",Qt::CaseInsensitive)) {
        fileContent = QString::fromLocal8Bit(file.readAll());
    } else {
        fileContent = QString::fromUtf8(file.readAll());
    }
    /*QString::fromUtf8(file.readAll());*/
    doc->setPlainText(fileContent);
    file.close();
    Highlighter *highLighter = new Highlighter(doc,new CodeEditorStyle(defaultStyleLight));
    Q_UNUSED(highLighter)
    int cursorPosStart = getPos(fileContent,line);
    int cursorPosEnd = getPos(fileContent,line+1) - 1;
    emit sigSelectionPos({
                             {"start",cursorPosStart},
                             {"end",cursorPosEnd}
                         });


//    QFile file(filepath);
//    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QStringList symbols;
//        QRegularExpression re(".*: ([A-Za-z_][A-Za-z0-9_]*)$");
//        const QString errorMessage = data["message"].toString();
//        QRegularExpressionMatch match = re.match(errorMessage);
//        if (match.hasMatch()) {
//            symbols << match.captured(1);
//        }
//        mUI.mCode->setError(fileContent, lineNumber, symbols);


}

}
