#include "printreport.h"
#include <QTextDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QtConcurrent>
#include <QFuture>
#include <QThreadPool>
#include "provider.h"
#include "project.h"

namespace CC {
PrintReport::PrintReport(QObject *parent) : QObject(parent)
{
    m_printer.setPageSize(QPrinter::A4);
    m_printer.setOutputFormat(QPrinter::PdfFormat);
    watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, &PrintReport::print_finished,Qt::QueuedConnection);
}

void PrintReport::setFile(const QString &filePath)
{
    m_printer.setOutputFileName(filePath);
    QFileInfo info(filePath);
    if(info.exists()) {
        QFile::remove(filePath);
    }
}

void PrintReport::startPrint()
{
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[this](){
        QTextDocument textDocument;
        makeDataToHtml();
        textDocument.setHtml(m_html);
        textDocument.print(&m_printer);
    });
    watcher->setFuture(future);
}

void PrintReport::makeDataToHtml()
{
    m_html.clear();



    m_html = "<table width=100% border=1 cellspacing=0 text-align=center style=border-collapse:collapse>";
    m_html += "<tr>";
    m_html += "<th>ID</th>";
    m_html += "<th>行号</th>";
    m_html += "<th>等级</th>";
    m_html += "<th>问题</th>";
    m_html += "<th>描述</th>";
    m_html += "<th>相关行</th>";
    m_html += "</tr>";
    QHash<QString,QString> hash_severity_color;
    hash_severity_color.insert("error","#FF4500");
    hash_severity_color.insert("warning","#FFA500");
    hash_severity_color.insert("style","#DA70D6");
    hash_severity_color.insert("performance","#00BFFF");
    hash_severity_color.insert("portability","#008000");
    hash_severity_color.insert("information","#20B2AA");
    QHash<QString,QString> hash_severity_cn;
    hash_severity_cn.insert("error","错误");
    hash_severity_cn.insert("warning","警告");
    hash_severity_cn.insert("style","风格");
    hash_severity_cn.insert("performance","性能");
    hash_severity_cn.insert("portability","平台");
    hash_severity_cn.insert("information","提示");
    QObjectListModel_DataItemRO* items = Provider::instance()->items();
    QHash<QString,QJsonObject> hash;
    QString lastFile = "";
    for(int i = 0; i< items->size(); i++) {
        auto item = items->at(i).data();
        const QString &file = item->file;
        const int line = item->line;
        const QString &severity = item->severity;
        const QString &id = item->id;
        const QString &summary = item->summary;
        const QJsonArray &array = item->array;
        if(lastFile != file) {
            m_html += "<tr><td align=center colspan=\"6\" bgcolor=\"#B0C4DE\">" + file + "</td></tr>";
            lastFile = file;
        }
        m_html += "<tr>";
        m_html += "<td align=center>" + QString("%1").arg(i+1) + "</td>";
        m_html += "<td align=center>" + QString("%1").arg(line) + "</td>";
        m_html += "<td align=center style=\"color:"+hash_severity_color.value(severity,"#ff0000")+";\">" + hash_severity_cn.value(severity,severity) + "</td>";
        m_html += "<td align=center>" + id + "</td>";
        m_html += "<td align=left>" + summary + "</td>";
        if(array.size() == 0) {
            m_html += "<td align=left></td>";
        } else {
            QString lines;
            lines += "<ul>";
            for(int i = 0; i< array.size(); i++) {
                lines += QString("<li>%1:").arg(array.at(i).toObject().value("line").toInt());
                lines += array.at(i).toObject().value("info").toString();
                lines += "</li>";
            }
            lines += "</ul>";
            m_html += "<td align=left>" + lines + "</td>";
        }
        m_html += "</tr>";
    }
    m_html += "</table>";
}
}


