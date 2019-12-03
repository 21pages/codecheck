#pragma once

#include <QObject>
#include <QJsonArray>

namespace CC {
class PrintReport;
class DataItemRO : public QObject {
    Q_OBJECT
    Q_PROPERTY( QString file MEMBER file CONSTANT )
    Q_PROPERTY( QString severity MEMBER severity CONSTANT )
    Q_PROPERTY( QString id MEMBER id CONSTANT )
    Q_PROPERTY( int line MEMBER line CONSTANT )
    Q_PROPERTY( QString summary MEMBER summary CONSTANT )
    Q_PROPERTY(QJsonArray array MEMBER array CONSTANT)
public:
    DataItemRO(const QString& file, const QString& severity,
               const QString& id, int line, const QString& summary, const QJsonArray &array)
        :file(file),severity(severity),id(id),line(line),summary(summary),array(array){}
    friend class PrintReport;
private:
    QString file;
    QString severity;
    QString id;
    int line;
    QString summary;
    QJsonArray array;
};
}
