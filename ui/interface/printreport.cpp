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
    QDir imgDir(Provider::instance()->exeDir() + "/tmp/");
    if(!imgDir.exists()) {
        imgDir.mkpath(imgDir.absolutePath());
    }
    for(const auto &  name : pngNameList) {
        image_type_path_hash.insert(name,Provider::instance()->exeDir() + "/tmp/" + name + ".png");
    }
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
        makeTitleToHtml(all0filter1);
        if(img_ret){
            makeImageToHtml();
        }
        makeDataToHtml(all0filter1);
        textDocument.setHtml(m_html);
        textDocument.print(&m_printer);
    });
    watcher->setFuture(future);
}

void PrintReport::makeTitleToHtml(int all0filter1)
{
    QJsonObject obj = Project::instance()->projectInfo();
    m_html += QString("<div align=\"center\"><h1>代码质量测试报告</h1></div>");
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;项目名称:</strong>%1</div>").arg(obj.value("name").toString());
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;源码路径:</strong>%1</div>").arg(obj.value("source").toString());
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;输出目录:</strong>%1</div>").arg(obj.value("dir").toString());
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;检测平台:</strong>%1</div>").arg(obj.value("platform").toString());
    int lang = obj.value("lang").toInt();
    QString langStr = "";
    if(lang < 3) {
        QStringList list;
        list << "通用"<<"C"<<"C++";
        langStr = list.at(lang);
    }
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;语言选择:</strong>%1</div>").arg(langStr);
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;C&nbsp;规范:</strong>%1</div>").arg(obj.value("c").toString());
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;C++规范:</strong>%1</div>").arg(obj.value("cpp").toString());
    if(all0filter1 == 0) {
        m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;报告内容:</strong>全部</div>");
    } else {
        int typeShow = Provider::instance()->typeShow();
        QString typeShowString;
        QStringList typeList = QStringList()<<"错误"<<"警告"<<"风格"<<"性能"<<"平台"<<"提醒";
        for(int i = 0; i < typeList.size(); i++) {
            if((typeShow & (1<<i)) == 0) {
                typeShowString += "无\"" + typeList.at(i) + "\",";
            }
        }
        if(typeShowString == "") {
            typeShowString = "无";
        }
        m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;内容筛选:</strong>%1</div>").arg(typeShowString);
        QString search = Provider::instance()->search();
        if(search == "") {
            search = "无";
        }
        m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;搜索条件:</strong>%1</div>").arg(search);
    }
    m_html += QString("<div><strong>&nbsp;&nbsp;&nbsp;&nbsp;报告时间:</strong>%1</div>").arg(QDateTime::currentDateTime().toString("yy年MM月dd日HH时mm分"));
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
    for(int i = 0; i < pngNameList.size(); i += 2) {
        m_html += "<div align = \"center\">";
        m_html += QString("<img src=\"%1\" width=\"280\"  height=\"210\">") \
                .arg(image_type_path_hash.value(pngNameList.at(i)));
        m_html += QString("<img src=\"%1\" width=\"280\"  height=\"210\">") \
                .arg(image_type_path_hash.value(pngNameList.at(i + 1)));
        m_html += "</div>";
    }

    for(QString path:image_type_path_hash.values()) {

        qDebug()<<m_html;
    }

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

#define GET_IMAGE_FN(big,small) {  \
    QQuickItem * chart##big = qobject_cast<QQuickItem*>(Manager::instance()->findQuick("chartView"#big)); \
    if(chart##big) { \
        const QString& key = CONST_##small; \
        grHash[key] =  chart##big->grabToImage(); \
        pGrHash[key] = grHash[key].data(); \
        connect(pGrHash[key],&QQuickItemGrabResult::ready,this,[this,key](){ \
            QImage img = pGrHash[key]->image(); \
            img.save(image_type_path_hash.value(key)); \
        }); \
    } else { \
        qDebug()<<"obj null"; \
        return false; \
    }   \
}

bool PrintReport::getImage()
{
    QQuickItem *view = qobject_cast<QQuickItem*>(Manager::instance()->findQuick("SwipeViewStatistics"));
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

    GET_IMAGE_FN(Severity,severity)
    GET_IMAGE_FN(ID,id)
    GET_IMAGE_FN(Error,error)
    GET_IMAGE_FN(Warning,warning)
    GET_IMAGE_FN(Style,style)
    GET_IMAGE_FN(Performance,performance)
    GET_IMAGE_FN(Portability,portability)
    GET_IMAGE_FN(Information,information)
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


