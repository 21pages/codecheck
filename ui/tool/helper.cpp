#include "helper.h"
#include <QDir>


QString Helper::GetStripPath(const QString &fullFilePath)
{
    QDir dir(fullFilePath);
    return dir.relativeFilePath(fullFilePath);
}

void Helper::Severity2Icon(Severity::SeverityType type, QString &icon, QString &severityStr)
{
    switch (type) {
    case Severity::none:
        severityStr = "无";
        break;
    case Severity::error:
        icon = "qrc:/old/dialog-error.png";
        severityStr = "错误";
        break;
    case Severity::warning:
        icon = "qrc:/old/dialog-warning.png";
        severityStr = "警告";
        break;
    case Severity::style:
        icon = "qrc:/old/applications-development.png";
        severityStr = "代码样式";
        break;
    case Severity::performance:
        icon = "qrc:/old/utilities-system-monitor.png";
        severityStr = "性能";
        break;
    case Severity::portability:
        icon = "qrc:/old/applications-system.png";
        severityStr = "可移植性";
        break;
    case Severity::information:
        icon = "qrc:/old/dialog-information.png";
        severityStr = "信息";
        break;
    case Severity::debug:
        severityStr = "调试";
        break;
    }
}
