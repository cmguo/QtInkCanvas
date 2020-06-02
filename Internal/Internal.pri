include(Ink/Ink.pri)
!inkcanvas_core: include(Controls/Controls.pri)

HEADERS += \
    $$PWD/debug.h \
    $$PWD/doubleutil.h \
    $$PWD/finallyhelper.h \
    $$PWD/matrixutil.h

SOURCES += \
    $$PWD/debug.cpp \
    $$PWD/doubleutil.cpp \
    $$PWD/finallyhelper.cpp \
    $$PWD/matrixutil.cpp
