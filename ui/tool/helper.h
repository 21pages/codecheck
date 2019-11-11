#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include "errorlogger.h"

class Helper : public QObject
{
    Q_OBJECT
public:
    static QString GetStripPath(const QString &fullFilePath);
    static void Severity2Icon(Severity::SeverityType type, QString &icon, QString &severityStr);
};

#endif // HELPER_H
