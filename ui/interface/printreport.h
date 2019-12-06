#ifndef PRINTREPORT_H
#define PRINTREPORT_H

#include <QObject>
#include <QPrinter>
#include <QFutureWatcher>
#include <QImage>
#include <QQuickItemGrabResult>


namespace CC {
    class PrintReport : public QObject
    {
        Q_OBJECT
    public:
        explicit PrintReport(QObject *parent = nullptr);
        void setFile(const QString& filePath);
        void startPrint(int all0filter1);
    private:
        void makeTitleToHtml(int all0filter1);
        void makeImageToHtml();
        void makeDataToHtml(int all0filter1);
        bool getImage();
        void item2html(QString&lastFile,const int i,const QString &file,const int line,const QString &severity,const QString &id,const QString &summary,const QJsonArray &array);
    signals:
        void print_finished();
    private:
        QPrinter m_printer;
        QString m_html;
        QFutureWatcher<void> *watcher;
        QHash<QString,QString> hash_severity_color;
        QHash<QString,QString> hash_severity_cn;

        QHash<QString,QSharedPointer<QQuickItemGrabResult>> grHash;
        QHash<QString,QQuickItemGrabResult*> pGrHash;

        QHash<QString,QString> image_type_path_hash;

        QStringList pngNameList = QStringList()<<"severity"<<"id"<<"error"  \
                                              <<"warning"<<"style"<<"performance"<<"portability"<<"information";
    };
}


#endif // PRINTREPORT_H
