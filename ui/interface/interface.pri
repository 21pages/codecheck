INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/iglobal.h \
    $$PWD/interface.h \
    $$PWD/manager.h \
    $$PWD/project.h

SOURCES += \
    $$PWD/manager.cpp \
    $$PWD/project.cpp

include(result/result.pri)

