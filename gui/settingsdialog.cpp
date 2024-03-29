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


#include <QDialog>
#include <QWidget>
#include <QList>
#include <QListWidgetItem>
#include <QSettings>
#include <QFileDialog>
#include <QThread>
#include "settingsdialog.h"
#include "applicationlist.h"
#include "translationhandler.h"
#include "codeeditorstyle.h"
#include "common.h"

SettingsDialog::SettingsDialog(ApplicationList *list,
                               TranslationHandler *translator,
                               QObject *parent) :
    QObject(parent),
    mApplications(list),
    mTempApplications(new ApplicationList(this)),
    mTranslator(translator)
{
#if WGT
    mUI.setupUi(this);
    QSettings settings;
    mTempApplications->copy(list);

    mUI.mJobs->setText(settings.value(SETTINGS_CHECK_THREADS, 1).toString());
    mUI.mForce->setCheckState(boolToCheckState(settings.value(SETTINGS_CHECK_FORCE, false).toBool()));
    mUI.mShowFullPath->setCheckState(boolToCheckState(settings.value(SETTINGS_SHOW_FULL_PATH, false).toBool()));
    mUI.mShowNoErrorsMessage->setCheckState(boolToCheckState(settings.value(SETTINGS_SHOW_NO_ERRORS, false).toBool()));
    mUI.mShowDebugWarnings->setCheckState(boolToCheckState(settings.value(SETTINGS_SHOW_DEBUG_WARNINGS, false).toBool()));
    mUI.mSaveAllErrors->setCheckState(boolToCheckState(settings.value(SETTINGS_SAVE_ALL_ERRORS, false).toBool()));
    mUI.mSaveFullPath->setCheckState(boolToCheckState(settings.value(SETTINGS_SAVE_FULL_PATH, false).toBool()));
    mUI.mInlineSuppressions->setCheckState(boolToCheckState(settings.value(SETTINGS_INLINE_SUPPRESSIONS, false).toBool()));
    mUI.mEnableInconclusive->setCheckState(boolToCheckState(settings.value(SETTINGS_INCONCLUSIVE_ERRORS, false).toBool()));
    mUI.mShowStatistics->setCheckState(boolToCheckState(settings.value(SETTINGS_SHOW_STATISTICS, false).toBool()));
    mUI.mShowErrorId->setCheckState(boolToCheckState(settings.value(SETTINGS_SHOW_ERROR_ID, false).toBool()));
    mUI.mEditPythonPath->setText(settings.value(SETTINGS_PYTHON_PATH, QString()).toString());
    mUI.mEditMisraFile->setText(settings.value(SETTINGS_MISRA_FILE, QString()).toString());

#ifdef Q_OS_WIN
    //mUI.mTabClang->setVisible(true);
    mUI.mEditClangPath->setText(settings.value(SETTINGS_CLANG_PATH, QString()).toString());
    mUI.mEditVsIncludePaths->setText(settings.value(SETTINGS_VS_INCLUDE_PATHS, QString()).toString());
    connect(mUI.mBtnBrowseClangPath, &QPushButton::released, this, &SettingsDialog::browseClangPath);
#else
    mUI.mTabClang->setVisible(false);
#endif
    mCurrentStyle = new CodeEditorStyle(CodeEditorStyle::loadSettings(&settings));
    manageStyleControls();

    connect(mUI.mButtons, &QDialogButtonBox::accepted, this, &SettingsDialog::ok);
    connect(mUI.mButtons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);

    connect(mUI.mBtnBrowsePythonPath, &QPushButton::clicked, this, &SettingsDialog::browsePythonPath);
    connect(mUI.mBtnBrowseMisraFile, &QPushButton::clicked, this, &SettingsDialog::browseMisraFile);
    mUI.mListWidget->setSortingEnabled(false);
    populateApplicationList();

    const int count = QThread::idealThreadCount();
    if (count != -1)
        mUI.mLblIdealThreads->setText(QString::number(count));
    else
        mUI.mLblIdealThreads->setText(tr("N/A"));

    loadSettings();
    initTranslationsList();
#endif
}

SettingsDialog::~SettingsDialog()
{
    saveSettings();
}

void SettingsDialog::initTranslationsList()
{
#if WGT
    const QString current = mTranslator->getCurrentLanguage();
    QList<TranslationInfo> translations = mTranslator->getTranslations();
    foreach (TranslationInfo translation, translations) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(translation.mName);
        item->setData(mLangCodeRole, QVariant(translation.mCode));
        mUI.mListLanguages->addItem(item);
        if (translation.mCode == current || translation.mCode == current.mid(0, 2))
            mUI.mListLanguages->setCurrentItem(item);
    }
#endif
}

Qt::CheckState SettingsDialog::boolToCheckState(bool yes)
{
    if (yes) {
        return Qt::Checked;
    }
    return Qt::Unchecked;
}

bool SettingsDialog::checkStateToBool(Qt::CheckState state)
{
    if (state == Qt::Checked) {
        return true;
    }
    return false;
}


void SettingsDialog::loadSettings()
{
    QSettings settings;
}

void SettingsDialog::saveSettings() const
{
    QSettings settings;
}

void SettingsDialog::saveSettingValues() const
{
#if WGT
    int jobs = mUI.mJobs->text().toInt();
    if (jobs <= 0) {
        jobs = 1;
    }

    QSettings settings;
    settings.setValue(SETTINGS_CHECK_THREADS, jobs);
    saveCheckboxValue(&settings, mUI.mForce, SETTINGS_CHECK_FORCE);
    saveCheckboxValue(&settings, mUI.mSaveAllErrors, SETTINGS_SAVE_ALL_ERRORS);
    saveCheckboxValue(&settings, mUI.mSaveFullPath, SETTINGS_SAVE_FULL_PATH);
    saveCheckboxValue(&settings, mUI.mShowFullPath, SETTINGS_SHOW_FULL_PATH);
    saveCheckboxValue(&settings, mUI.mShowNoErrorsMessage, SETTINGS_SHOW_NO_ERRORS);
    saveCheckboxValue(&settings, mUI.mShowDebugWarnings, SETTINGS_SHOW_DEBUG_WARNINGS);
    saveCheckboxValue(&settings, mUI.mInlineSuppressions, SETTINGS_INLINE_SUPPRESSIONS);
    saveCheckboxValue(&settings, mUI.mEnableInconclusive, SETTINGS_INCONCLUSIVE_ERRORS);
    saveCheckboxValue(&settings, mUI.mShowStatistics, SETTINGS_SHOW_STATISTICS);
    saveCheckboxValue(&settings, mUI.mShowErrorId, SETTINGS_SHOW_ERROR_ID);
    settings.setValue(SETTINGS_PYTHON_PATH, mUI.mEditPythonPath->text());
    settings.setValue(SETTINGS_MISRA_FILE, mUI.mEditMisraFile->text());

#ifdef Q_OS_WIN
    settings.setValue(SETTINGS_CLANG_PATH, mUI.mEditClangPath->text());
    settings.setValue(SETTINGS_VS_INCLUDE_PATHS, mUI.mEditVsIncludePaths->text());
#endif

    const QListWidgetItem *currentLang = mUI.mListLanguages->currentItem();
    if (currentLang) {
        const QString langcode = currentLang->data(mLangCodeRole).toString();
        settings.setValue(SETTINGS_LANGUAGE, langcode);
    }
    CodeEditorStyle::saveSettings(&settings, *mCurrentStyle);
#endif
}

void SettingsDialog::saveCheckboxValue(QSettings *settings, QCheckBox *box,
                                       const QString &name)
{
    settings->setValue(name, checkStateToBool(box->checkState()));
}

void SettingsDialog::populateApplicationList()
{
#if WGT
    const int defapp = mTempApplications->getDefaultApplication();
    for (int i = 0; i < mTempApplications->getApplicationCount(); i++) {
        const Application& app = mTempApplications->getApplication(i);
        QString name = app.getName();
        if (i == defapp) {
            name += " ";
            name += tr("[Default]");
        }
        mUI.mListWidget->addItem(name);
    }

    // Select default application, or if there is no default app then the
    // first item.
    if (defapp == -1)
        mUI.mListWidget->setCurrentRow(0);
    else {
        if (mTempApplications->getApplicationCount() > defapp)
            mUI.mListWidget->setCurrentRow(defapp);
        else
            mUI.mListWidget->setCurrentRow(0);
    }
#endif
}

void SettingsDialog::ok()
{
    mApplications->copy(mTempApplications);
#if WGT
    accept();
#endif
}

bool SettingsDialog::showFullPath() const
{
#if WGT
    return checkStateToBool(mUI.mShowFullPath->checkState());
#else
    return false;
#endif
}

bool SettingsDialog::saveFullPath() const
{
#if WGT
    return checkStateToBool(mUI.mSaveFullPath->checkState());
#else
    return false;
#endif
}

bool SettingsDialog::saveAllErrors() const
{
#if WGT
    return checkStateToBool(mUI.mSaveAllErrors->checkState());
#else
    return false;
#endif
}

bool SettingsDialog::showNoErrorsMessage() const
{
#if WGT
    return checkStateToBool(mUI.mShowNoErrorsMessage->checkState());
#else
    return false;
#endif
}

bool SettingsDialog::showErrorId() const
{
#if WGT
    return checkStateToBool(mUI.mShowErrorId->checkState());
#else
    return false;
#endif
}

bool SettingsDialog::showInconclusive() const
{
#if WGT
    return checkStateToBool(mUI.mEnableInconclusive->checkState());
#else
    return false;
#endif
}

void SettingsDialog::browsePythonPath()
{
#if WGT
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select python binary"), QDir::rootPath());
    if (fileName.contains("python", Qt::CaseInsensitive))
        mUI.mEditPythonPath->setText(fileName);
#endif
}

void SettingsDialog::browseMisraFile()
{
#if WGT
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Select MISRA File"), QDir::homePath(), "Misra File (*.pdf *.txt)");
    if (!fileName.isEmpty())
        mUI.mEditMisraFile->setText(fileName);
#endif
}

void SettingsDialog::browseClangPath()
{
#if WGT
    QString selectedDir = QFileDialog::getExistingDirectory(this,
                          tr("Select clang path"),
                          QDir::rootPath());

    if (!selectedDir.isEmpty()) {
        mUI.mEditClangPath->setText(selectedDir);
    }
#endif
}

void SettingsDialog::manageStyleControls()
{
#if WGT
    bool isSystemTheme = mCurrentStyle->isSystemTheme();
    bool isDefaultLight = !isSystemTheme && *mCurrentStyle == defaultStyleLight;
    bool isDefaultDark =  !isSystemTheme && *mCurrentStyle == defaultStyleDark;
    mUI.mThemeSystem->setChecked(isSystemTheme);
    mUI.mThemeLight->setChecked(isDefaultLight && !isDefaultDark);
    mUI.mThemeDark->setChecked(!isDefaultLight && isDefaultDark);
    mUI.mThemeCustom->setChecked(!isSystemTheme && !isDefaultLight && !isDefaultDark);
    mUI.mBtnEditTheme->setEnabled(!isSystemTheme && !isDefaultLight && !isDefaultDark);
#endif
}

