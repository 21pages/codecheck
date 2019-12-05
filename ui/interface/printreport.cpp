#include "printreport.h"
#include <QTextDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QtConcurrent>
#include <QFuture>
#include <QThreadPool>
#include <QApplication>
#include "provider.h"
#include "project.h"
#include "manager.h"
#include "iglobal.h"

namespace CC {
PrintReport::PrintReport(QObject *parent) : QObject(parent)
{
    m_printer.setPageSize(QPrinter::A4);
    m_printer.setOutputFormat(QPrinter::PdfFormat);
    watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, &PrintReport::print_finished,Qt::QueuedConnection);

    hash_severity_color.insert("error","#FF4500");
    hash_severity_color.insert("warning","#FFA500");
    hash_severity_color.insert("style","#DA70D6");
    hash_severity_color.insert("performance","#00BFFF");
    hash_severity_color.insert("portability","#008000");
    hash_severity_color.insert("information","#20B2AA");

    hash_severity_cn.insert("error","错误");
    hash_severity_cn.insert("warning","警告");
    hash_severity_cn.insert("style","风格");
    hash_severity_cn.insert("performance","性能");
    hash_severity_cn.insert("portability","平台");
    hash_severity_cn.insert("information","提示");



}

void PrintReport::setFile(const QString &filePath)
{
    m_printer.setOutputFileName(filePath);
    QFileInfo info(filePath);
    if(info.exists()) {
        QFile::remove(filePath);
    }
}

void PrintReport::startPrint(int all0filter1)
{
    image_type_path_hash.clear();
    image_type_path_hash.insert("severity","severity.png");
    image_type_path_hash.insert("id","id.png");
    for(QString path:image_type_path_hash.values()) {
        QFileInfo info(path);
        if(info.exists()) {
            QFile::remove(path);
        }
    }
    bool img_ret = getImage();
    auto future = QtConcurrent::run(QThreadPool::globalInstance(),[this,all0filter1,img_ret](){
        QTextDocument textDocument;
        m_html.clear();
        if(img_ret){
            makeImageToHtml();
        }
        makeDataToHtml(all0filter1);
        textDocument.setHtml(m_html);
        textDocument.print(&m_printer);
    });
    watcher->setFuture(future);
}

void PrintReport::makeImageToHtml()
{
    while(true) {
        bool allok = true;
        for(QString path:image_type_path_hash.values()) {
            QFileInfo info(path);
            if(!info.exists()) {
                allok = false;
            }
        }
        if(allok) {
            break;
        }
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    m_html += "<div align = \"center\">";
    for(QString path:image_type_path_hash.values()) {
        m_html += QString("<img src=\"%1\" width=\"280\"  height=\"210\">").arg(Provider::instance()->exeDir() + "/" + path);
        qDebug()<<m_html;
    }
    m_html += "</div>";
}

void PrintReport::makeDataToHtml(int all0filter1)
{
    m_html += "<table width=100% border=1 cellspacing=0 text-align=center style=border-collapse:collapse>";
    m_html += "<tr>";
    m_html += "<th>ID</th>";
    m_html += "<th>行号</th>";
    m_html += "<th>等级</th>";
    m_html += "<th>问题</th>";
    m_html += "<th>描述</th>";
    m_html += "<th>相关行</th>";
    m_html += "</tr>";

    QString lastFile = "";
    if(all0filter1 == 1) {
        QObjectListModel_DataItemRO* items = Provider::instance()->items();
        for(int i = 0; i< items->size(); i++) {
            auto item = items->at(i).data();
            const QString &file = item->file;
            const int line = item->line;
            const QString &severity = item->severity;
            const QString &id = item->id;
            const QString &summary = item->summary;
            const QJsonArray &array = item->array;
            item2html(lastFile,i,file,line,severity,id,summary,array);
        }
    } else {
        auto list = Manager::instance()->errorJsonList;
        for(int i = 0; i < list.size(); i++) {
            const auto& obj = list.at(i);
            const QString& severity = obj->value(CONST_severityStr).toString();
            const QString id =  obj->value(CONST_id).toString();
            const QString &file = obj->value(CONST_file).toString();
            const int line = obj->value(CONST_line).toInt();
            const QString &summary = obj->value(CONST_summary).toString();
            const QJsonArray &array = obj->value(CONST_array).toArray();
            item2html(lastFile,i,file,line,severity,id,summary,array);
        }
    }
    m_html += "</table>";
}

bool PrintReport::getImage()
{
    QQuickItem *view = qobject_cast<QQuickItem*>(Manager::instance()->findQuick("statistics_view"));
    while(view->property("statisticsLoaded").toBool() != true) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        qDebug()<<"in while";
    }
    QTime time;
    time.start();
    while(time.elapsed() < 1000) {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    qDebug()<<"out while";
    QQuickItem * chartSeverity = qobject_cast<QQuickItem*>(Manager::instance()->findQuick("chartSeverity"));
    if(chartSeverity) {
        chartSeverityResult =  chartSeverity->grabToImage();
        chartSeverityGrabResult = chartSeverityResult.data();
        connect(chartSeverityGrabResult,&QQuickItemGrabResult::ready,this,[this](){
            QImage img = chartSeverityResult->image();//这里转成图片
            img.save(image_type_path_hash.value("severity"));
        });
    } else {
        qDebug()<<"obj null";
        return false;
    }

    QQuickItem * chartID = qobject_cast<QQuickItem*>(Manager::instance()->findQuick("chartID"));
    if(chartID) {
        chartIDResult =  chartID->grabToImage();
        chartIDGrabResult = chartIDResult.data();
        connect(chartIDGrabResult,&QQuickItemGrabResult::ready,this,[this](){
            QImage img = chartIDGrabResult->image();//这里转成图片
            img.save(image_type_path_hash.value("id"));
        });
    } else {
        qDebug()<<"obj null";
        return false;
    }
    return true;
}

void PrintReport::item2html(QString&lastFile, const int i,const QString &file, const int line, const QString &severity, const QString &id, const QString &summary, const QJsonArray &array)
{
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
}


