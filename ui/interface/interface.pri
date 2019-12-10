INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/iglobal.h \
    $$PWD/interface.h \
    $$PWD/manager.h \
    $$PWD/printreport.h \
    $$PWD/project.h

SOURCES += \
    $$PWD/manager.cpp \
    $$PWD/printreport.cpp \
    $$PWD/project.cpp

include(result/result.pri)
include(filefolder/filefolder.pri)

