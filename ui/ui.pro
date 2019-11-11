lessThan(QT_MAJOR_VERSION, 5): error(requires >= Qt 5 (You used: $$QT_VERSION))

TEMPLATE = app
TARGET = codecheck
#DEPENDPATH += . \
#    ../lib
#INCLUDEPATH += . \
#    ../lib
QT += widgets printsupport quick qml
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
CONFIG += embed_manifest_exe

win32 {
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

SOURCES += \
    main.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

include(material/material.pri)
OPTIONS += roboto
include(interface/interface.pri)
include(../gui/gui.pri)
include(tool/tool.pri)

RESOURCES += qml.qrc  \
                         image/demo.qrc \
                        image/icons/icons.qrc
#RESOURCES = ../gui/gui.qrc


