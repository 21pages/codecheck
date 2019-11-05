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

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QSettings>
#include <QProcess>
#include <QListWidget>
#include "common.h"
#include "projectfiledialog.h"
#include "checkthread.h"
#include "projectfile.h"
#include "library.h"
#include "platforms.h"

/** Return paths from QListWidget */
static QStringList getPaths(const QListWidget *list)
{
#if WGT
    const int count = list->count();
    QStringList paths;
    for (int i = 0; i < count; i++) {
        QListWidgetItem *item = list->item(i);
        paths << QDir::fromNativeSeparators(item->text());
    }
    return paths;
#else
    return QStringList();
#endif
}

/** Platforms shown in the platform combobox */
static const cppcheck::Platform::PlatformType builtinPlatforms[] = {
    cppcheck::Platform::Native,
    cppcheck::Platform::Win32A,
    cppcheck::Platform::Win32W,
    cppcheck::Platform::Win64,
    cppcheck::Platform::Unix32,
    cppcheck::Platform::Unix64
};

static const int numberOfBuiltinPlatforms = sizeof(builtinPlatforms) / sizeof(builtinPlatforms[0]);

ProjectFileDialog::ProjectFileDialog(ProjectFile *projectFile, QObject *parent)
    : QObject(parent)
    , mProjectFile(projectFile)
{
#if WGT
    mUI.setupUi(this);

    mUI.mToolClangAnalyzer->hide();

    const QFileInfo inf(projectFile->getFilename());
    QString filename = inf.fileName();
    QString title = tr("Project file: %1").arg(filename);
    setWindowTitle(title);
    loadSettings();

    // Checkboxes for the libraries..
    const QString applicationFilePath = QCoreApplication::applicationFilePath();
    const QString appPath = QFileInfo(applicationFilePath).canonicalPath();
    QSettings settings;
    const QString datadir = settings.value("DATADIR",QString()).toString();
    QStringList searchPaths;
    searchPaths << appPath << appPath + "/cfg" << inf.canonicalPath();
#ifdef FILESDIR
    if (FILESDIR[0])
        searchPaths << FILESDIR << FILESDIR "/cfg";
#endif
    if (!datadir.isEmpty())
        searchPaths << datadir << datadir + "/cfg";
    QStringList libs;
    // Search the std.cfg first since other libraries could depend on it
    QString stdLibraryFilename;
    foreach (const QString sp, searchPaths) {
        QDir dir(sp);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QStringList("*.cfg"));
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        foreach (QFileInfo item, dir.entryInfoList()) {
            QString library = item.fileName();
            if (library.compare("std.cfg", Qt::CaseInsensitive) != 0)
                continue;
            Library lib;
            const QString fullfilename = sp + "/" + library;
            const Library::Error err = lib.load(nullptr, fullfilename.toLatin1());
            if (err.errorcode != Library::OK)
                continue;
            // Working std.cfg found
            stdLibraryFilename = fullfilename;
            break;
        }
        if (!stdLibraryFilename.isEmpty())
            break;
    }
    // Search other libraries
    foreach (const QString sp, searchPaths) {
        QDir dir(sp);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QStringList("*.cfg"));
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        foreach (QFileInfo item, dir.entryInfoList()) {
            QString library = item.fileName();
            {
                Library lib;
                const QString fullfilename = sp + "/" + library;
                Library::Error err = lib.load(nullptr, fullfilename.toLatin1());
                if (err.errorcode != Library::OK) {
                    // Some libraries depend on std.cfg so load it first and test again
                    lib.load(nullptr, stdLibraryFilename.toLatin1());
                    err = lib.load(nullptr, fullfilename.toLatin1());
                }
                if (err.errorcode != Library::OK)
                    continue;
            }
            library.chop(4);
            if (library.compare("std", Qt::CaseInsensitive) == 0)
                continue;
            if (libs.indexOf(library) == -1)
                libs << library;
        }
    }
    libs.sort();
    mUI.mLibraries->clear();
    for (const QString &lib : libs) {
        QListWidgetItem* item = new QListWidgetItem(lib, mUI.mLibraries);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
    }

    // Platforms..
    Platforms platforms;
    for (int i = 0; i < numberOfBuiltinPlatforms; i++)
        mUI.mComboBoxPlatform->addItem(platforms.get(builtinPlatforms[i]).mTitle);
    QStringList platformFiles;
    foreach (QString sp, searchPaths) {
        if (sp.endsWith("/cfg"))
            sp = sp.mid(0,sp.length()-3) + "platforms";
        QDir dir(sp);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QStringList("*.xml"));
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        foreach (QFileInfo item, dir.entryInfoList()) {
            const QString platformFile = item.fileName();

            cppcheck::Platform plat2;
            if (!plat2.loadPlatformFile(applicationFilePath.toStdString().c_str(), platformFile.toStdString()))
                continue;

            if (platformFiles.indexOf(platformFile) == -1)
                platformFiles << platformFile;
        }
    }
    platformFiles.sort();
    mUI.mComboBoxPlatform->addItems(platformFiles);

    mUI.mEditTags->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9 ;]*"),this));

    const QRegExp undefRegExp("\\s*([a-zA-Z_][a-zA-Z0-9_]*[; ]*)*");
    mUI.mEditUndefines->setValidator(new QRegExpValidator(undefRegExp, this));

    connect(mUI.mButtons, &QDialogButtonBox::accepted, this, &ProjectFileDialog::ok);
    connect(mUI.mBtnBrowseBuildDir, &QPushButton::clicked, this, &ProjectFileDialog::browseBuildDir);
    connect(mUI.mBtnClearImportProject, &QPushButton::clicked, this, &ProjectFileDialog::clearImportProject);
    connect(mUI.mBtnBrowseImportProject, &QPushButton::clicked, this, &ProjectFileDialog::browseImportProject);
    connect(mUI.mBtnAddCheckPath, SIGNAL(clicked()), this, SLOT(addCheckPath()));
    connect(mUI.mBtnEditCheckPath, &QPushButton::clicked, this, &ProjectFileDialog::editCheckPath);
    connect(mUI.mBtnRemoveCheckPath, &QPushButton::clicked, this, &ProjectFileDialog::removeCheckPath);
    connect(mUI.mBtnAddInclude, SIGNAL(clicked()), this, SLOT(addIncludeDir()));
    connect(mUI.mBtnEditInclude, &QPushButton::clicked, this, &ProjectFileDialog::editIncludeDir);
    connect(mUI.mBtnRemoveInclude, &QPushButton::clicked, this, &ProjectFileDialog::removeIncludeDir);
    connect(mUI.mBtnAddIgnorePath, SIGNAL(clicked()), this, SLOT(addExcludePath()));
    connect(mUI.mBtnEditIgnorePath, &QPushButton::clicked, this, &ProjectFileDialog::editExcludePath);
    connect(mUI.mBtnRemoveIgnorePath, &QPushButton::clicked, this, &ProjectFileDialog::removeExcludePath);
    connect(mUI.mBtnIncludeUp, &QPushButton::clicked, this, &ProjectFileDialog::moveIncludePathUp);
    connect(mUI.mBtnIncludeDown, &QPushButton::clicked, this, &ProjectFileDialog::moveIncludePathDown);
    connect(mUI.mBtnRemoveSuppression, &QPushButton::clicked, this, &ProjectFileDialog::removeSuppression);
    connect(mUI.mBtnBrowseMisraFile, &QPushButton::clicked, this, &ProjectFileDialog::browseMisraFile);

    loadFromProjectFile(projectFile);
#endif
}

ProjectFileDialog::~ProjectFileDialog()
{
    saveSettings();
}

void ProjectFileDialog::loadSettings()
{
    QSettings settings;
}

void ProjectFileDialog::saveSettings() const
{
    QSettings settings;
}

static void updateAddonCheckBox(QCheckBox *cb, const ProjectFile *projectFile, const QString &dataDir, const QString &addon)
{
    if (projectFile)
        cb->setChecked(projectFile->getAddons().contains(addon));
    if (CheckThread::getAddonFilePath(dataDir, addon + ".py").isEmpty()) {
        cb->setEnabled(false);
        cb->setText(cb->text() + QObject::tr(" (Not found)"));
    }
}

void ProjectFileDialog::loadFromProjectFile(const ProjectFile *projectFile)
{
#if WGT
    setRootPath(projectFile->getRootPath());
    setBuildDir(projectFile->getBuildDir());
    setIncludepaths(projectFile->getIncludeDirs());
    setDefines(projectFile->getDefines());
    setUndefines(projectFile->getUndefines());
    setCheckPaths(projectFile->getCheckPaths());
    setImportProject(projectFile->getImportProject());
    mUI.mChkAllVsConfigs->setChecked(projectFile->getAnalyzeAllVsConfigs());
    mUI.mCheckHeaders->setChecked(projectFile->getCheckHeaders());
    mUI.mCheckUnusedTemplates->setChecked(projectFile->getCheckUnusedTemplates());
    mUI.mMaxCtuDepth->setValue(projectFile->getMaxCtuDepth());
    setExcludedPaths(projectFile->getExcludedPaths());
    setLibraries(projectFile->getLibraries());
    const QString platform = projectFile->getPlatform();
    if (platform.endsWith(".xml")) {
        int i;
        for (i = numberOfBuiltinPlatforms; i < mUI.mComboBoxPlatform->count(); ++i) {
            if (mUI.mComboBoxPlatform->itemText(i) == platform)
                break;
        }
        if (i < mUI.mComboBoxPlatform->count())
            mUI.mComboBoxPlatform->setCurrentIndex(i);
        else {
            mUI.mComboBoxPlatform->addItem(platform);
            mUI.mComboBoxPlatform->setCurrentIndex(i);
        }
    } else {
        int i;
        for (i = 0; i < numberOfBuiltinPlatforms; ++i) {
            const cppcheck::Platform::PlatformType p = builtinPlatforms[i];
            if (platform == cppcheck::Platform::platformString(p))
                break;
        }
        if (i < numberOfBuiltinPlatforms)
            mUI.mComboBoxPlatform->setCurrentIndex(i);
        else
            mUI.mComboBoxPlatform->setCurrentIndex(-1);
    }

    mUI.mComboBoxPlatform->setCurrentText(projectFile->getPlatform());
    setSuppressions(projectFile->getSuppressions());

    // Human knowledge..
    /*
    mUI.mListUnknownFunctionReturn->clear();
    mUI.mListUnknownFunctionReturn->addItem("rand()");
    for (int row = 0; row < mUI.mListUnknownFunctionReturn->count(); ++row) {
        QListWidgetItem *item = mUI.mListUnknownFunctionReturn->item(row);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        const bool unknownValues = projectFile->getCheckUnknownFunctionReturn().contains(item->text());
        item->setCheckState(unknownValues ? Qt::Checked : Qt::Unchecked); // AND initialize check state
    }
    mUI.mCheckSafeClasses->setChecked(projectFile->getSafeChecks().classes);
    mUI.mCheckSafeExternalFunctions->setChecked(projectFile->getSafeChecks().externalFunctions);
    mUI.mCheckSafeInternalFunctions->setChecked(projectFile->getSafeChecks().internalFunctions);
    mUI.mCheckSafeExternalVariables->setChecked(projectFile->getSafeChecks().externalVariables);
    */

    // Addons..
    QSettings settings;
    const QString dataDir = settings.value("DATADIR", QString()).toString();
    updateAddonCheckBox(mUI.mAddonThreadSafety, projectFile, dataDir, "threadsafety");
    updateAddonCheckBox(mUI.mAddonY2038, projectFile, dataDir, "y2038");
    updateAddonCheckBox(mUI.mAddonCert, projectFile, dataDir, "cert");
    updateAddonCheckBox(mUI.mAddonMisra, projectFile, dataDir, "misra");

    const QString &misraFile = settings.value(SETTINGS_MISRA_FILE, QString()).toString();
    mUI.mEditMisraFile->setText(misraFile);
    if (!mUI.mAddonMisra->isEnabled()) {
        mUI.mEditMisraFile->setEnabled(false);
        mUI.mBtnBrowseMisraFile->setEnabled(false);
    } else if (misraFile.isEmpty()) {
        mUI.mAddonMisra->setEnabled(false);
        mUI.mAddonMisra->setText(mUI.mAddonMisra->text() + ' ' + tr("(no rule texts file)"));
    }

    mUI.mToolClangAnalyzer->setChecked(projectFile->getClangAnalyzer());
    mUI.mToolClangTidy->setChecked(projectFile->getClangTidy());
    if (CheckThread::clangTidyCmd().isEmpty()) {
        mUI.mToolClangTidy->setText(tr("Clang-tidy (not found)"));
        mUI.mToolClangTidy->setEnabled(false);
    }
    mUI.mEditTags->setText(projectFile->getTags().join(';'));
    updatePathsAndDefines();
#endif
}

void ProjectFileDialog::saveToProjectFile(ProjectFile *projectFile) const
{
#if WGT
    projectFile->setRootPath(getRootPath());
    projectFile->setBuildDir(getBuildDir());
    projectFile->setImportProject(getImportProject());
    projectFile->setAnalyzeAllVsConfigs(mUI.mChkAllVsConfigs->isChecked());
    projectFile->setCheckHeaders(mUI.mCheckHeaders->isChecked());
    projectFile->setCheckUnusedTemplates(mUI.mCheckUnusedTemplates->isChecked());
    projectFile->setMaxCtuDepth(mUI.mMaxCtuDepth->value());
    projectFile->setIncludes(getIncludePaths());
    projectFile->setDefines(getDefines());
    projectFile->setUndefines(getUndefines());
    projectFile->setCheckPaths(getCheckPaths());
    projectFile->setExcludedPaths(getExcludedPaths());
    projectFile->setLibraries(getLibraries());
    if (mUI.mComboBoxPlatform->currentText().endsWith(".xml"))
        projectFile->setPlatform(mUI.mComboBoxPlatform->currentText());
    else {
        int i = mUI.mComboBoxPlatform->currentIndex();
        if (i < numberOfBuiltinPlatforms)
            projectFile->setPlatform(cppcheck::Platform::platformString(builtinPlatforms[i]));
        else
            projectFile->setPlatform(QString());
    }
    projectFile->setSuppressions(getSuppressions());
    // Addons
    QStringList list;
    if (mUI.mAddonThreadSafety->isChecked())
        list << "threadsafety";
    if (mUI.mAddonY2038->isChecked())
        list << "y2038";
    if (mUI.mAddonCert->isChecked())
        list << "cert";
    if (mUI.mAddonMisra->isChecked())
        list << "misra";
    projectFile->setAddons(list);
    projectFile->setClangAnalyzer(mUI.mToolClangAnalyzer->isChecked());
    projectFile->setClangTidy(mUI.mToolClangTidy->isChecked());
    projectFile->setTags(mUI.mEditTags->text().split(";", QString::SkipEmptyParts));
#endif
}

void ProjectFileDialog::ok()
{
    saveToProjectFile(mProjectFile);
    mProjectFile->write();
#if WGT
    accept();
#endif
}

QString ProjectFileDialog::getExistingDirectory(const QString &caption, bool trailingSlash)
{
    const QFileInfo inf(mProjectFile->getFilename());
    const QString rootpath = inf.absolutePath();
    QString selectedDir = QFileDialog::getExistingDirectory(nullptr,
                          caption,
                          rootpath);

    if (selectedDir.isEmpty())
        return QString();

    // Check if the path is relative to project file's path and if so
    // make it a relative path instead of absolute path.
    const QDir dir(rootpath);
    const QString relpath(dir.relativeFilePath(selectedDir));
    if (!relpath.startsWith("../.."))
        selectedDir = relpath;

    // Trailing slash..
    if (trailingSlash && !selectedDir.endsWith('/'))
        selectedDir += '/';

    return selectedDir;
}

void ProjectFileDialog::browseBuildDir()
{
#if WGT
    const QString dir(getExistingDirectory(tr("Select Cppcheck build dir"), false));
    if (!dir.isEmpty())
        mUI.mEditBuildDir->setText(dir);
#endif
}

void ProjectFileDialog::updatePathsAndDefines()
{
#if WGT
    const QString &fileName = mUI.mEditImportProject->text();
    bool importProject = !fileName.isEmpty();
    mUI.mBtnClearImportProject->setEnabled(importProject);
    mUI.mListCheckPaths->setEnabled(!importProject);
    mUI.mListIncludeDirs->setEnabled(!importProject);
    mUI.mBtnAddCheckPath->setEnabled(!importProject);
    mUI.mBtnEditCheckPath->setEnabled(!importProject);
    mUI.mBtnRemoveCheckPath->setEnabled(!importProject);
    mUI.mEditDefines->setEnabled(!importProject);
    mUI.mEditUndefines->setEnabled(!importProject);
    mUI.mBtnAddInclude->setEnabled(!importProject);
    mUI.mBtnEditInclude->setEnabled(!importProject);
    mUI.mBtnRemoveInclude->setEnabled(!importProject);
    mUI.mBtnIncludeUp->setEnabled(!importProject);
    mUI.mBtnIncludeDown->setEnabled(!importProject);
    mUI.mChkAllVsConfigs->setEnabled(fileName.endsWith(".sln") || fileName.endsWith(".vcxproj"));
#endif
}

void ProjectFileDialog::clearImportProject()
{
#if WGT
    mUI.mEditImportProject->clear();
    updatePathsAndDefines();
#endif
}

void ProjectFileDialog::browseImportProject()
{
#if WGT
    const QFileInfo inf(mProjectFile->getFilename());
    const QDir &dir = inf.absoluteDir();
    QMap<QString,QString> filters;
    filters[tr("Visual Studio")] = "*.sln *.vcxproj";
    filters[tr("Compile database")] = "compile_commands.json";
    filters[tr("Borland C++ Builder 6")] = "*.bpr";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Project"),
                       dir.canonicalPath(),
                       toFilterString(filters));
    if (!fileName.isEmpty()) {
        mUI.mEditImportProject->setText(dir.relativeFilePath(fileName));
        updatePathsAndDefines();
    }
#endif
}

QString ProjectFileDialog::getImportProject() const
{
#if WGT
    return mUI.mEditImportProject->text();
#else
    return QString();
#endif
}

void ProjectFileDialog::addIncludeDir(const QString &dir)
{
#if WGT
    if (dir.isNull() || dir.isEmpty())
        return;

    const QString newdir = QDir::toNativeSeparators(dir);
    QListWidgetItem *item = new QListWidgetItem(newdir);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    mUI.mListIncludeDirs->addItem(item);
#endif
}

void ProjectFileDialog::addCheckPath(const QString &path)
{
#if WGT
    if (path.isNull() || path.isEmpty())
        return;

    const QString newpath = QDir::toNativeSeparators(path);
    QListWidgetItem *item = new QListWidgetItem(newpath);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    mUI.mListCheckPaths->addItem(item);
#endif
}

void ProjectFileDialog::addExcludePath(const QString &path)
{
#if WGT
    if (path.isNull() || path.isEmpty())
        return;

    const QString newpath = QDir::toNativeSeparators(path);
    QListWidgetItem *item = new QListWidgetItem(newpath);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    mUI.mListExcludedPaths->addItem(item);
#endif
}

QString ProjectFileDialog::getRootPath() const
{
#if WGT
    QString root = mUI.mEditProjectRoot->text();
    root = root.trimmed();
    root = QDir::fromNativeSeparators(root);
    return root;
#else
    return QString();
#endif
}

QString ProjectFileDialog::getBuildDir() const
{
#if WGT
    return mUI.mEditBuildDir->text();
#else
    return QString();
#endif
}

QStringList ProjectFileDialog::getIncludePaths() const
{
#if WGT
    return getPaths(mUI.mListIncludeDirs);
#else
    return QStringList();
#endif
}

QStringList ProjectFileDialog::getDefines() const
{
#if WGT
    return mUI.mEditDefines->text().trimmed().split(QRegExp("\\s*;\\s*"), QString::SkipEmptyParts);
#else
    return QStringList();
#endif
}

QStringList ProjectFileDialog::getUndefines() const
{
#if WGT
    const QString undefine = mUI.mEditUndefines->text().trimmed();
    QStringList undefines = undefine.split(QRegExp("\\s*;\\s*"), QString::SkipEmptyParts);
    undefines.removeDuplicates();
    return undefines;
#else
    return QStringList();
#endif
}

QStringList ProjectFileDialog::getCheckPaths() const
{
#if WGT
    return getPaths(mUI.mListCheckPaths);
#else
    return QStringList();
#endif
}

QStringList ProjectFileDialog::getExcludedPaths() const
{
#if WGT
    return getPaths(mUI.mListExcludedPaths);
#else
    return QStringList();
#endif
}

QStringList ProjectFileDialog::getLibraries() const
{
#if WGT
    QStringList libraries;
    for (int row = 0; row < mUI.mLibraries->count(); ++row) {
        QListWidgetItem *item = mUI.mLibraries->item(row);
        if (item->checkState() == Qt::Checked)
            libraries << item->text();
    }
    return libraries;
#else
    return QStringList();
#endif
}

void ProjectFileDialog::setRootPath(const QString &root)
{
#if WGT
    mUI.mEditProjectRoot->setText(QDir::toNativeSeparators(root));
#endif
}

void ProjectFileDialog::setBuildDir(const QString &buildDir)
{
#if WGT
    mUI.mEditBuildDir->setText(buildDir);
#endif
}

void ProjectFileDialog::setImportProject(const QString &importProject)
{
#if WGT
    mUI.mEditImportProject->setText(importProject);
#endif
}

void ProjectFileDialog::setIncludepaths(const QStringList &includes)
{
#if WGT
    foreach (QString dir, includes) {
        addIncludeDir(dir);
    }
#endif
}

void ProjectFileDialog::setDefines(const QStringList &defines)
{
#if WGT
    mUI.mEditDefines->setText(defines.join(";"));
#endif
}

void ProjectFileDialog::setUndefines(const QStringList &undefines)
{
#if WGT
    mUI.mEditUndefines->setText(undefines.join(";"));
#endif
}

void ProjectFileDialog::setCheckPaths(const QStringList &paths)
{
#if WGT
    foreach (QString path, paths) {
        addCheckPath(path);
    }
#endif
}

void ProjectFileDialog::setExcludedPaths(const QStringList &paths)
{
#if WGT
    foreach (QString path, paths) {
        addExcludePath(path);
    }
#endif
}

void ProjectFileDialog::setLibraries(const QStringList &libraries)
{
#if WGT
    for (int row = 0; row < mUI.mLibraries->count(); ++row) {
        QListWidgetItem *item = mUI.mLibraries->item(row);
        item->setCheckState(libraries.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
    }
#endif
}

void ProjectFileDialog::setSuppressions(const QList<Suppressions::Suppression> &suppressions)
{
#if WGT
    mSuppressions = suppressions;

    QStringList s;
    foreach (const Suppressions::Suppression &suppression, mSuppressions) {
        s << QString::fromStdString(suppression.getText());
    }

    mUI.mListSuppressions->clear();
    mUI.mListSuppressions->addItems(s);
    mUI.mListSuppressions->sortItems();
#endif
}

void ProjectFileDialog::addCheckPath()
{
    QString dir = getExistingDirectory(tr("Select a directory to check"), false);
    if (!dir.isEmpty())
        addCheckPath(dir);
}

void ProjectFileDialog::editCheckPath()
{
#if WGT
    QListWidgetItem *item = mUI.mListCheckPaths->currentItem();
    mUI.mListCheckPaths->editItem(item);
#endif
}

void ProjectFileDialog::removeCheckPath()
{
#if WGT
    const int row = mUI.mListCheckPaths->currentRow();
    QListWidgetItem *item = mUI.mListCheckPaths->takeItem(row);
    delete item;
#endif
}

void ProjectFileDialog::addIncludeDir()
{
    const QString dir = getExistingDirectory(tr("Select include directory"), true);
    if (!dir.isEmpty())
        addIncludeDir(dir);
}

void ProjectFileDialog::removeIncludeDir()
{
#if WGT
    const int row = mUI.mListIncludeDirs->currentRow();
    QListWidgetItem *item = mUI.mListIncludeDirs->takeItem(row);
    delete item;
#endif
}

void ProjectFileDialog::editIncludeDir()
{
#if WGT
    QListWidgetItem *item = mUI.mListIncludeDirs->currentItem();
    mUI.mListIncludeDirs->editItem(item);
#endif
}

void ProjectFileDialog::addExcludePath()
{
    QString dir = getExistingDirectory(tr("Select directory to ignore"), true);
    if (!dir.isEmpty())
        addExcludePath(dir);
}

void ProjectFileDialog::editExcludePath()
{
#if WGT
    QListWidgetItem *item = mUI.mListExcludedPaths->currentItem();
    mUI.mListExcludedPaths->editItem(item);
#endif
}

void ProjectFileDialog::removeExcludePath()
{
#if WGT
    const int row = mUI.mListExcludedPaths->currentRow();
    QListWidgetItem *item = mUI.mListExcludedPaths->takeItem(row);
    delete item;
#endif
}

void ProjectFileDialog::moveIncludePathUp()
{
#if WGT
    int row = mUI.mListIncludeDirs->currentRow();
    QListWidgetItem *item = mUI.mListIncludeDirs->takeItem(row);
    row = row > 0 ? row - 1 : 0;
    mUI.mListIncludeDirs->insertItem(row, item);
    mUI.mListIncludeDirs->setCurrentItem(item);
#endif
}

void ProjectFileDialog::moveIncludePathDown()
{
#if WGT
    int row = mUI.mListIncludeDirs->currentRow();
    QListWidgetItem *item = mUI.mListIncludeDirs->takeItem(row);
    const int count = mUI.mListIncludeDirs->count();
    row = row < count ? row + 1 : count;
    mUI.mListIncludeDirs->insertItem(row, item);
    mUI.mListIncludeDirs->setCurrentItem(item);
#endif
}

void ProjectFileDialog::removeSuppression()
{
#if WGT
    const int row = mUI.mListSuppressions->currentRow();
    QListWidgetItem *item = mUI.mListSuppressions->takeItem(row);
    if (!item)
        return;

    int suppressionIndex = getSuppressionIndex(item->text());
    if (suppressionIndex >= 0)
        mSuppressions.removeAt(suppressionIndex);
    delete item;
#endif
}

int ProjectFileDialog::getSuppressionIndex(const QString &shortText) const
{
    const std::string s = shortText.toStdString();
    for (int i = 0; i < mSuppressions.size(); ++i) {
        if (mSuppressions[i].getText() == s)
            return i;
    }
    return -1;
}

void ProjectFileDialog::browseMisraFile()
{
#if WGT
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Select MISRA rule texts file"), QDir::homePath(), tr("Misra rule texts file (%1)").arg("*.txt"));
    if (!fileName.isEmpty()) {
        QSettings settings;
        mUI.mEditMisraFile->setText(fileName);
        settings.setValue(SETTINGS_MISRA_FILE, fileName);

        mUI.mAddonMisra->setText("MISRA C 2012");
        mUI.mAddonMisra->setEnabled(true);
        updateAddonCheckBox(mUI.mAddonMisra, nullptr, settings.value("DATADIR", QString()).toString(), "misra");
    }
#endif
}
