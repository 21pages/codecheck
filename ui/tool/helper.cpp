#include "helper.h"
#include <QDir>
#include <QTextStream>
#include <QtDebug>


QString Helper::GetStripPath(const QString &fullFilePath)
{
    QDir dir(fullFilePath);
    return dir.relativeFilePath(fullFilePath);
}

void Helper::Severity2Icon(Severity::SeverityType type, QString &icon, QString &severityStr)
{
    switch (type) {
    case Severity::none:
        severityStr = "none";
        break;
    case Severity::error:
        icon = "qrc:/old/dialog-error.png";
        severityStr = "error";
        break;
    case Severity::warning:
        icon = "qrc:/old/dialog-warning.png";
        severityStr = "warning";
        break;
    case Severity::style:
        icon = "qrc:/old/applications-development.png";
        severityStr = "style";
        break;
    case Severity::performance:
        icon = "qrc:/old/utilities-system-monitor.png";
        severityStr = "performance";
        break;
    case Severity::portability:
        icon = "qrc:/old/applications-system.png";
        severityStr = "portability";
        break;
    case Severity::information:
        icon = "qrc:/old/dialog-information.png";
        severityStr = "information";
        break;
    case Severity::debug:
        severityStr = "debug";
        break;
    }
}

static int getFileLineNum(const QString &fileData)
{
    int line = 0;
    for (int pos = 0; pos < fileData.size(); ++pos) {
        if (fileData[pos] != '\n')
            continue;
        ++line;
    }
    if(fileData.at(fileData.length() - 1) != "/") {
        ++line;
    }
    return line;

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

QJsonObject Helper::getShowContent(const QString &fileName, const QString &codec, int line)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        QJsonObject obj;
        obj.insert("fileName","");
        obj.insert("start",0);
        obj.insert("end",0);
        obj.insert("content","");
        return obj;
    }
    QString fileContent;
    if(QString::compare(codec,"gbk",Qt::CaseInsensitive) == 0) {
        fileContent = QString::fromLocal8Bit(file.readAll());
    } else {
        fileContent = QString::fromUtf8(file.readAll());
    }
    QTextStream stream(&fileContent);
    QStringList contentList;
    while(!stream.atEnd()) {
        contentList.append(stream.readLine());
    }
    int lineNumAll = contentList.size();
    if(line > lineNumAll || line < 1) {
        QJsonObject obj;
        obj.insert("fileName","");
        obj.insert("start",0);
        obj.insert("end",0);
        obj.insert("content","");
        return obj;
    }
    int startLine,endLine;
    startLine = line>10?line-10:1;
    endLine = (lineNumAll - line > 10) ? line + 10 :lineNumAll;
    QStringList showList = contentList.mid(startLine-1,endLine-startLine + 1);
    QString showString;
    int startPos = 0;
    int endPos = 0;
    for(int i = 0; i< showList.size(); i++) {
        if(startLine + i == line) {
            startPos = showString.size();
        }
        showString.append(showList.at(i));
        if(startLine + i == line) {
            endPos = showString.size();
        }
        showString.append("\n");
    }
    QJsonObject obj;
    obj.insert("fileName",fileName);
    obj.insert("start",startPos);
    obj.insert("end",endPos);
    obj.insert("content",showString);
    return obj;
}
