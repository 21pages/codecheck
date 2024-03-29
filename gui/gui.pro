lessThan(QT_MAJOR_VERSION, 5): error(requires >= Qt 5 (You used: $$QT_VERSION))

TEMPLATE = app
TARGET = cppcheck-gui
#CONFIG += warn_on debug
DEPENDPATH += . \
    ../lib
INCLUDEPATH += . \
    ../lib
QT += widgets
QT += printsupport

contains(LINKCORE, [yY][eE][sS]) {
    LIBS += -l../bin/cppcheck-core
    DEFINES += CPPCHECKLIB_IMPORT
}
LIBS += -L$$PWD/../externals

DESTDIR = .
RCC_DIR = temp
MOC_DIR = temp
OBJECTS_DIR = temp
UI_DIR = temp

isEmpty(QMAKE_CXX) {
    isEmpty(CXX)) {
        QMAKE_CXX = gcc
    } else {
        QMAKE_CXX = $$(CXX)
    }
}

win32 {
   CONFIG += windows
   contains(LINKCORE, [yY][eE][sS]) {
      DESTDIR = ../bin
      RCC_DIR = temp/generated
      MOC_DIR = temp/generated
      OBJECTS_DIR = temp/generated
      UI_DIR = temp/generated
   } else {
      DESTDIR = ../Build/gui
      RCC_DIR = ../BuildTmp/gui
      MOC_DIR = ../BuildTmp/gui
      OBJECTS_DIR = ../BuildTmp/gui
      UI_DIR = ../BuildTmp/gui
   }
}

RESOURCES = gui.qrc
FORMS = about.ui \
        application.ui \
        file.ui \
        mainwindow.ui \
        projectfiledialog.ui \
        resultsview.ui \
        scratchpad.ui \
        settings.ui \
        stats.ui \
        librarydialog.ui \
        libraryaddfunctiondialog.ui \
        libraryeditargdialog.ui \
        newsuppressiondialog.ui

TRANSLATIONS =  cppcheck_de.ts \
                cppcheck_es.ts \
                cppcheck_fi.ts \
                cppcheck_fr.ts \
                cppcheck_it.ts \
                cppcheck_ja.ts \
                cppcheck_ko.ts \
                cppcheck_nl.ts \
                cppcheck_ru.ts \
                cppcheck_sr.ts \
                cppcheck_sv.ts \
                cppcheck_zh_CN.ts

# Windows-specific options
CONFIG += embed_manifest_exe

contains(LINKCORE, [yY][eE][sS]) {
} else {
    BASEPATH = ../lib/
    include($$PWD/../lib/lib.pri)
}

HEADERS += aboutdialog.h \
           application.h \
           applicationdialog.h \
           applicationlist.h \
           checkstatistics.h \
           checkthread.h \
           codeeditstylecontrols.h \
           codeeditorstyle.h \
           codeeditstyledialog.h \
           codeeditor.h \
           common.h \
           csvreport.h \
           erroritem.h \
           filelist.h \
           fileviewdialog.h \
           mainwindow.h \
           platforms.h \
           printablereport.h \
           projectfile.h \
           projectfiledialog.h \
           report.h \
           resultstree.h \
           resultsview.h \
           scratchpad.h \
           settingsdialog.h \
           showtypes.h \
           statsdialog.h \
           threadhandler.h \
           threadresult.h \
           translationhandler.h \
           txtreport.h \
           xmlreport.h \
           xmlreportv2.h \
           librarydialog.h \
           cppchecklibrarydata.h \
           libraryaddfunctiondialog.h \
           libraryeditargdialog.h \
           newsuppressiondialog.h

SOURCES += aboutdialog.cpp \
           application.cpp \
           applicationdialog.cpp \
           applicationlist.cpp \
           checkstatistics.cpp \
           checkthread.cpp \
           codeeditorstyle.cpp \
           codeeditstylecontrols.cpp \
           codeeditstyledialog.cpp \
           codeeditor.cpp \
           common.cpp \
           csvreport.cpp \
           erroritem.cpp \
           filelist.cpp \
           fileviewdialog.cpp \
           main.cpp \
           mainwindow.cpp\
           platforms.cpp \
           printablereport.cpp \
           projectfile.cpp \
           projectfiledialog.cpp \
           report.cpp \
           resultstree.cpp \
           resultsview.cpp \
           scratchpad.cpp \
           settingsdialog.cpp \
           showtypes.cpp \
           statsdialog.cpp \
           threadhandler.cpp \
           threadresult.cpp \
           translationhandler.cpp \
           txtreport.cpp \
           xmlreport.cpp \
           xmlreportv2.cpp \
           librarydialog.cpp \
           cppchecklibrarydata.cpp \
           libraryaddfunctiondialog.cpp \
           libraryeditargdialog.cpp \
           newsuppressiondialog.cpp

win32 {
    RC_FILE = cppcheck-gui.rc
    HEADERS += ../lib/version.h
    contains(LINKCORE, [yY][eE][sS]) {
    } else {
        LIBS += -lshlwapi
    }
}

contains(QMAKE_CC, gcc) {
    QMAKE_CXXFLAGS += -std=c++11 -Wno-missing-field-initializers -Wno-missing-braces -Wno-sign-compare -Wno-deprecated-declarations
}

contains(QMAKE_CXX, clang++) {
    QMAKE_CXXFLAGS += -std=c++11
}

contains(HAVE_QCHART, [yY][eE][sS]) {
    QT += charts
    DEFINES += HAVE_QCHART
} else {
    message("Charts disabled - to enable it pass HAVE_QCHART=yes to qmake.")
}

