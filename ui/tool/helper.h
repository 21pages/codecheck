#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QJsonObject>
#include "errorlogger.h"

class Helper : public QObject
{
    Q_OBJECT
public:
    static QString GetStripPath(const QString &fullFilePath);
    static void Severity2Icon(Severity::SeverityType type, QString &icon, QString &severityStr);
    static QJsonObject getFileContent(const QString &fileName, const QString &codec, int line);
};

#endif // HELPER_H
