#ifndef PRINTREPORT_H
#define PRINTREPORT_H

#include <QObject>
#include <QPrinter>
#include <QFutureWatcher>

namespace CC {
    class PrintReport : public QObject
    {
        Q_OBJECT
    public:
        explicit PrintReport(QObject *parent = nullptr);
        void setFile(const QString& filePath);
        void startPrint();
    private:
        void makeDataToHtml();
    signals:
        void print_finished();
    private:
        QPrinter m_printer;
        QString m_html;
        QFutureWatcher<void> *watcher;
    };
}


#endif // PRINTREPORT_H
