lessThan(QT_MAJOR_VERSION, 5): error(requires >= Qt 5 (You used: $$QT_VERSION))

#TEMPLATE = app
#TARGET = cppcheck-gui
#CONFIG += warn_on debug
DEPENDPATH += \
    $$PWD/../lib \
    $$PWD
INCLUDEPATH += \
    $$PWD/../lib \
    $$PWD
QT += widgets
QT += printsupport

contains(LINKCORE, [yY][eE][sS]) {
    LIBS += -l$$PWD/../bin/cppcheck-core
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
      DESTDIR = $$PWD/../bin
      RCC_DIR = $$PWD/temp/generated
      MOC_DIR = $$PWD/temp/generated
      OBJECTS_DIR = $$PWD/temp/generated
      UI_DIR = $$PWD/temp/generated
   } else {
      DESTDIR = $$PWD/../Build/gui
      RCC_DIR = $$PWD/../BuildTmp/gui
      MOC_DIR = $$PWD/../BuildTmp/gui
      OBJECTS_DIR = $$PWD/../BuildTmp/gui
      UI_DIR = $$PWD/../BuildTmp/gui
   }
}

#RESOURCES = $$PWD/gui.qrc
FORMS = $$PWD/about.ui \
        $$PWD/application.ui \
        $$PWD/file.ui \
        $$PWD/mainwindow.ui \
        $$PWD/projectfiledialog.ui \
        $$PWD/resultsview.ui \
        $$PWD/scratchpad.ui \
        $$PWD/settings.ui \
        $$PWD/stats.ui \
        $$PWD/librarydialog.ui \
        $$PWD/libraryaddfunctiondialog.ui \
        $$PWD/libraryeditargdialog.ui \
        $$PWD/newsuppressiondialog.ui

TRANSLATIONS =  $$PWD/cppcheck_de.ts \
                $$PWD/cppcheck_es.ts \
                $$PWD/cppcheck_fi.ts \
                $$PWD/cppcheck_fr.ts \
                $$PWD/cppcheck_it.ts \
                $$PWD/cppcheck_ja.ts \
                $$PWD/cppcheck_ko.ts \
                $$PWD/cppcheck_nl.ts \
                $$PWD/cppcheck_ru.ts \
                $$PWD/cppcheck_sr.ts \
                $$PWD/cppcheck_sv.ts \
                $$PWD/cppcheck_zh_CN.ts

# Windows-specific options
#CONFIG += embed_manifest_exe

contains(LINKCORE, [yY][eE][sS]) {
} else {
    BASEPATH = $$PWD/../lib/
    include($$PWD/../lib/lib.pri)
}

HEADERS += $$PWD/aboutdialog.h \
           $$PWD/application.h \
           $$PWD/applicationdialog.h \
           $$PWD/applicationlist.h \
           $$PWD/checkstatistics.h \
           $$PWD/checkthread.h \
           $$PWD/codeeditstylecontrols.h \
           $$PWD/codeeditorstyle.h \
           $$PWD/codeeditstyledialog.h \
           $$PWD/codeeditor.h \
           $$PWD/common.h \
           $$PWD/csvreport.h \
           $$PWD/erroritem.h \
           $$PWD/filelist.h \
           $$PWD/fileviewdialog.h \
           $$PWD/mainwindow.h \
           $$PWD/platforms.h \
           $$PWD/printablereport.h \
           $$PWD/projectfile.h \
           $$PWD/projectfiledialog.h \
           $$PWD/report.h \
           $$PWD/resultstree.h \
           $$PWD/resultsview.h \
           $$PWD/scratchpad.h \
           $$PWD/settingsdialog.h \
           $$PWD/showtypes.h \
           $$PWD/statsdialog.h \
           $$PWD/threadhandler.h \
           $$PWD/threadresult.h \
           $$PWD/translationhandler.h \
           $$PWD/txtreport.h \
           $$PWD/xmlreport.h \
           $$PWD/xmlreportv2.h \
           $$PWD/librarydialog.h \
           $$PWD/cppchecklibrarydata.h \
           $$PWD/libraryaddfunctiondialog.h \
           $$PWD/libraryeditargdialog.h \
           $$PWD/newsuppressiondialog.h

SOURCES += $$PWD/aboutdialog.cpp \
           $$PWD/application.cpp \
           $$PWD/applicationdialog.cpp \
           $$PWD/applicationlist.cpp \
           $$PWD/checkstatistics.cpp \
           $$PWD/checkthread.cpp \
           $$PWD/codeeditorstyle.cpp \
           $$PWD/codeeditstylecontrols.cpp \
           $$PWD/codeeditstyledialog.cpp \
           $$PWD/codeeditor.cpp \
           $$PWD/common.cpp \
           $$PWD/csvreport.cpp \
           $$PWD/erroritem.cpp \
           $$PWD/filelist.cpp \
           $$PWD/fileviewdialog.cpp \
           $$PWD/mainwindow.cpp\
           $$PWD/platforms.cpp \
           $$PWD/printablereport.cpp \
           $$PWD/projectfile.cpp \
           $$PWD/projectfiledialog.cpp \
           $$PWD/report.cpp \
           $$PWD/resultstree.cpp \
           $$PWD/resultsview.cpp \
           $$PWD/scratchpad.cpp \
           $$PWD/settingsdialog.cpp \
           $$PWD/showtypes.cpp \
           $$PWD/statsdialog.cpp \
           $$PWD/threadhandler.cpp \
           $$PWD/threadresult.cpp \
           $$PWD/translationhandler.cpp \
           $$PWD/txtreport.cpp \
           $$PWD/xmlreport.cpp \
           $$PWD/xmlreportv2.cpp \
           $$PWD/librarydialog.cpp \
           $$PWD/cppchecklibrarydata.cpp \
           $$PWD/libraryaddfunctiondialog.cpp \
           $$PWD/libraryeditargdialog.cpp \
           $$PWD/newsuppressiondialog.cpp

win32 {
    RC_FILE = $$PWD/cppcheck-gui.rc
    HEADERS += $$PWD/../lib/version.h
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

