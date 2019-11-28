/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2019 Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVariant>
#include <QString>
#include <QModelIndex>
#include <QSettings>
#include <QDir>
#include <QDate>
#include <QMenu>
#include <QClipboard>
#include "resultsview.h"
#include "common.h"
#include "erroritem.h"
#include "txtreport.h"
#include "xmlreport.h"
#include "xmlreportv2.h"
#include "csvreport.h"
#include "printablereport.h"
#include "applicationlist.h"
#include "checkstatistics.h"
#include "path.h"
#include "codeeditorstyle.h"
#include "manager.h"
#include "resultstree.h"

ResultsView::ResultsView(QObject *parent) :
    QObject(parent),
    mShowNoErrorsMessage(true),
    mStatistics(new CheckStatistics(this))
{
#if WGT
    mUI.setupUi(this);

    connect(mUI.mTree, &ResultsTree::resultsHidden, this, &ResultsView::resultsHidden);
    connect(mUI.mTree, &ResultsTree::checkSelected, this, &ResultsView::checkSelected);
    connect(mUI.mTree, &ResultsTree::treeSelectionChanged, this, &ResultsView::updateDetails);
    connect(mUI.mTree, &ResultsTree::tagged, this, &ResultsView::tagged);
    connect(mUI.mTree, &ResultsTree::suppressIds, this, &ResultsView::suppressIds);
    connect(this, &ResultsView::showResults, mUI.mTree, &ResultsTree::showResults);
    connect(this, &ResultsView::showCppcheckResults, mUI.mTree, &ResultsTree::showCppcheckResults);
    connect(this, &ResultsView::showClangResults, mUI.mTree, &ResultsTree::showClangResults);
    connect(this, &ResultsView::collapseAllResults, mUI.mTree, &ResultsTree::collapseAll);
    connect(this, &ResultsView::expandAllResults, mUI.mTree, &ResultsTree::expandAll);
    connect(this, &ResultsView::showHiddenResults, mUI.mTree, &ResultsTree::showHiddenResults);

    mUI.mListLog->setContextMenuPolicy(Qt::CustomContextMenu);
#endif
}

void ResultsView::initialize(QSettings *settings, ApplicationList *list, ThreadHandler *checkThreadHandler)
{
//    mUI.mProgress->setMinimum(0);
//    mUI.mProgress->setVisible(false);

//    CodeEditorStyle theStyle(CodeEditorStyle::loadSettings(settings));
//    mUI.mCode->setStyle(theStyle);

//    QByteArray state = settings->value(SETTINGS_MAINWND_SPLITTER_STATE).toByteArray();
//    mUI.mVerticalSplitter->restoreState(state);
//    mShowNoErrorsMessage = settings->value(SETTINGS_SHOW_NO_ERRORS, true).toBool();

//    mUI.mTree->initialize(settings, list, checkThreadHandler);
}

ResultsView::~ResultsView()
{
    //dtor
}

void ResultsView::clear(bool results)
{
#if WGT
    if (results) {
        mUI.mTree->clear();
    }

    mUI.mDetails->setText(QString());

    mStatistics->clear();

    //Clear the progressbar
    mUI.mProgress->setMaximum(PROGRESS_MAX);
    mUI.mProgress->setValue(0);
    mUI.mProgress->setFormat("%p%");
#endif
}

void ResultsView::clear(const QString &filename)
{
#if WGT
    mUI.mTree->clear(filename);
#endif
}

void ResultsView::clearRecheckFile(const QString &filename)
{
#if WGT
    mUI.mTree->clearRecheckFile(filename);
#endif
}

void ResultsView::progress(int value, const QString& description)
{
#if WGT
    mUI.mProgress->setValue(value);
    mUI.mProgress->setFormat(QString("%p% (%1)").arg(description));
#endif
    emit sig_progress(value, description);
}

void ResultsView::error(const ErrorItem &item)
{
#if WGT
    if (mUI.mTree->addErrorItem(item)) {
        emit gotResults();
        mStatistics->addItem(item.tool(), ShowTypes::SeverityToShowType(item.severity));
    }
#endif
    Manager::instance()->addErrorItem(item);
}

void ResultsView::filterResults(const QString& filter)
{
#if WGT
    mUI.mTree->filterResults(filter);
#endif

}

void ResultsView::saveStatistics(const QString &filename) const
{
    QFile f(filename);
    if (!f.open(QIODevice::Text | QIODevice::Append))
        return;
    QTextStream ts(&f);
    ts <<  '[' << QDate::currentDate().toString("dd.MM.yyyy") << "]\n";
    ts << QDateTime::currentMSecsSinceEpoch() << '\n';
    foreach (QString tool, mStatistics->getTools()) {
        ts << tool << "-error:" << mStatistics->getCount(tool, ShowTypes::ShowErrors) << '\n';
        ts << tool << "-warning:" << mStatistics->getCount(tool, ShowTypes::ShowWarnings) << '\n';
        ts << tool << "-style:" << mStatistics->getCount(tool, ShowTypes::ShowStyle) << '\n';
        ts << tool << "-performance:" << mStatistics->getCount(tool, ShowTypes::ShowPerformance) << '\n';
        ts << tool << "-portability:" << mStatistics->getCount(tool, ShowTypes::ShowPortability) << '\n';
    }
}

void ResultsView::updateFromOldReport(const QString &filename) const
{
#if WGT
    mUI.mTree->updateFromOldReport(filename);
#endif
}

void ResultsView::save(const QString &filename, Report::Type type) const
{
#if WGT
    if (!hasResults()) {
        QMessageBox msgBox;
        msgBox.setText(tr("No errors found, nothing to save."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
#endif
    Report *report = nullptr;

    switch (type) {
    case Report::CSV:
        report = new CsvReport(filename);
        break;
    case Report::TXT:
        report = new TxtReport(filename);
        break;
    case Report::XMLV2:
        report = new XmlReportV2(filename);
        break;
    }
    if(report) {
        if(report->create()) {
            Manager::instance()->resultsTree->saveResults(report);
        }
        delete report;
        report = nullptr;
    }
#if WGT
    if (report) {
        if (report->create())
            mUI.mTree->saveResults(report);
        else {
            QMessageBox msgBox;
            msgBox.setText(tr("Failed to save the report."));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
        delete report;
        report = nullptr;
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("Failed to save the report."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
#endif
}

void ResultsView::print()
{
#if WGT
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Report"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    print(&printer);
#endif
}

void ResultsView::printPreview()
{
#if WGT
    QPrinter printer;
    QPrintPreviewDialog dialog(&printer, this);
    connect(&dialog, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)));
    dialog.exec();
#endif
}

void ResultsView::print(QPrinter* printer)
{
#if WGT

    if (!hasResults()) {
        QMessageBox msgBox;
        msgBox.setText(tr("No errors found, nothing to print."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    PrintableReport report;
    mUI.mTree->saveResults(&report);
    QTextDocument doc(report.getFormattedReportText());
    doc.print(printer);
#endif
}

void ResultsView::updateSettings(bool showFullPath,
                                 bool saveFullPath,
                                 bool saveAllErrors,
                                 bool showNoErrorsMessage,
                                 bool showErrorId,
                                 bool showInconclusive)
{
#if WGT
    mUI.mTree->updateSettings(showFullPath, saveFullPath, saveAllErrors, showErrorId, showInconclusive);
    mShowNoErrorsMessage = showNoErrorsMessage;
#endif
}

void ResultsView::updateStyleSetting(QSettings *settings)
{
#if WGT
    CodeEditorStyle theStyle(CodeEditorStyle::loadSettings(settings));
    mUI.mCode->setStyle(theStyle);
#endif
}

void ResultsView::setCheckDirectory(const QString &dir)
{
#if WGT
    mUI.mTree->setCheckDirectory(dir);
#endif
}

QString ResultsView::getCheckDirectory(void)
{
#if WGT
    return mUI.mTree->getCheckDirectory();
#else
    return QString();
#endif
}

void ResultsView::checkingStarted(int count)
{
#if WGT
    mUI.mProgress->setVisible(true);
    mUI.mProgress->setMaximum(PROGRESS_MAX);
    mUI.mProgress->setValue(0);
    mUI.mProgress->setFormat(tr("%p% (%1 of %2 files checked)").arg(0).arg(count));
#endif
}

void ResultsView::checkingFinished()
{
#if WGT
    mUI.mProgress->setVisible(false);
    mUI.mProgress->setFormat("%p%");

    //Should we inform user of non visible/not found errors?
    if (mShowNoErrorsMessage) {
        //Tell user that we found no errors
        if (!hasResults()) {
            QMessageBox msg(QMessageBox::Information,
                            tr("Cppcheck"),
                            tr("No errors found."),
                            QMessageBox::Ok,
                            this);

            msg.exec();
        } //If we have errors but they aren't visible, tell user about it
        else if (!mUI.mTree->hasVisibleResults()) {
            QString text = tr("Errors were found, but they are configured to be hidden.\n"\
                              "To toggle what kind of errors are shown, open view menu.");
            QMessageBox msg(QMessageBox::Information,
                            tr("Cppcheck"),
                            text,
                            QMessageBox::Ok,
                            this);

            msg.exec();
        }
    }
#endif
}

bool ResultsView::hasVisibleResults() const
{
#if WGT
    return mUI.mTree->hasVisibleResults();
#else
    return false;
#endif
}

bool ResultsView::hasResults() const
{
#if WGT
    return mUI.mTree->hasResults();
#else
    return false;
#endif
}

void ResultsView::saveSettings(QSettings *settings)
{
#if WGT
    mUI.mTree->saveSettings();
    QByteArray state = mUI.mVerticalSplitter->saveState();
    settings->setValue(SETTINGS_MAINWND_SPLITTER_STATE, state);
    mUI.mVerticalSplitter->restoreState(state);
#endif
}

void ResultsView::translate()
{
#if WGT
    mUI.mTree->translate();
#endif
}

void ResultsView::disableProgressbar()
{
#if WGT
    mUI.mProgress->setEnabled(false);
#endif
}

void ResultsView::readErrorsXml(const QString &filename)
{
    XmlReportV2 report(filename);
    QList<ErrorItem> errors;
    if (report.open()) {
        errors = report.read();
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("Failed to read the report."));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    ErrorItem item;
    foreach (item, errors) {
            Manager::instance()->addErrorItem(item);
    }
    emit open_finished(true);
    QString dir;
    if (!errors.isEmpty() && !errors[0].errorPath.isEmpty()) {
        QString relativePath = QFileInfo(filename).canonicalPath();
        if (QFileInfo(relativePath + '/' + errors[0].errorPath[0].file).exists())
            dir = relativePath;
    }

    Manager::instance()->resultsTree->setCheckDirectory(dir);
}

void ResultsView::updateDetails(const QModelIndex &index)
{
#if WGT
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(mUI.mTree->model());
    QStandardItem *item = model->itemFromIndex(index);

    mUI.mCode->setPlainText(QString());

    if (!item) {
        mUI.mDetails->setText(QString());
        return;
    }

    // Make sure we are working with the first column
    if (item->parent() && item->column() != 0)
        item = item->parent()->child(item->row(), 0);

    QVariantMap data = item->data().toMap();

    // If there is no severity data then it is a parent item without summary and message
    if (!data.contains("severity")) {
        mUI.mDetails->setText(QString());
        return;
    }

    const QString message = data["message"].toString();
    QString formattedMsg = message;

    const QString file0 = data["file0"].toString();
    if (!file0.isEmpty() && Path::isHeader(data["file"].toString().toStdString()))
        formattedMsg += QString("\n\n%1: %2").arg(tr("First included by")).arg(QDir::toNativeSeparators(file0));

    if (mUI.mTree->showIdColumn())
        formattedMsg.prepend(tr("Id") + ": " + data["id"].toString() + "\n");
    mUI.mDetails->setText(formattedMsg);

    const int lineNumber = data["line"].toInt();

    QString filepath = data["file"].toString();
    if (!QFileInfo(filepath).exists() && QFileInfo(mUI.mTree->getCheckDirectory() + '/' + filepath).exists())
        filepath = mUI.mTree->getCheckDirectory() + '/' + filepath;

    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QStringList symbols;
        QRegularExpression re(".*: ([A-Za-z_][A-Za-z0-9_]*)$");
        const QString errorMessage = data["message"].toString();
        QRegularExpressionMatch match = re.match(errorMessage);
        if (match.hasMatch()) {
            symbols << match.captured(1);
        }

        QTextStream in(&file);
        mUI.mCode->setError(in.readAll(), lineNumber, symbols);
    }
#endif
}

void ResultsView::log(const QString &str)
{
#if WGT
    mUI.mListLog->addItem(str);
#endif
}

void ResultsView::debugError(const ErrorItem &item)
{
#if WGT
    mUI.mListLog->addItem(item.ToString());
#endif
}

void ResultsView::logClear()
{
#if WGT
    mUI.mListLog->clear();
#endif
}

void ResultsView::logCopyEntry()
{
#if WGT
    const QListWidgetItem * item = mUI.mListLog->currentItem();
    if (nullptr != item) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(item->text());
    }
#endif
}

void ResultsView::logCopyComplete()
{
#if WGT
    QString logText;
    for (int i=0; i < mUI.mListLog->count(); ++i) {
        const QListWidgetItem * item = mUI.mListLog->item(i);
        if (nullptr != item) {
            logText += item->text();
        }
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(logText);
#endif
}

void ResultsView::on_mListLog_customContextMenuRequested(const QPoint &pos)
{
#if WGT
    if (mUI.mListLog->count() <= 0)
        return;

    const QPoint globalPos = mUI.mListLog->mapToGlobal(pos);

    QMenu contextMenu;
    contextMenu.addAction(tr("Clear Log"), this, SLOT(logClear()));
    contextMenu.addAction(tr("Copy this Log entry"), this, SLOT(logCopyEntry()));
    contextMenu.addAction(tr("Copy complete Log"), this, SLOT(logCopyComplete()));

    contextMenu.exec(globalPos);
#endif
}
