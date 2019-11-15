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
