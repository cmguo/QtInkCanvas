QT =

TEMPLATE = lib
DEFINES += INKCANVAS_LIBRARY

CONFIG += c++17
CONFIG += inkcanvas_core

win32: CONFIG += inkcanvas_qt
android: CONFIG += inkcanvas_android
macos: CONFIG += inkcanvas_macos
ios: CONFIG += inkcanvas_ios

inkcanvas_core {
    DEFINES += INKCANVAS_CORE=1
    TARGET = InkCanvasCore
}

inkcanvas_qt {
    DEFINES += INKCANVAS_QT=1
    TARGET = InkCanvasQt
    QT += gui widgets svg
}

inkcanvas_android {
    DEFINES += INKCANVAS_ANDROID=1
    TARGET = InkCanvasAndroid
    INCLUDEPATH += "C:\Users\Brandon\AppData\Local\Android\Sdk\ndk-bundle\sources\cxx-stl\llvm-libc++\include"
    QMAKE_LFLAGS += -Wl,--version-script,$$PWD/InkCanvasJni.version
}

inkcanvas_ios {
    DEFINES += INKCANVAS_IOS=1
    TARGET = InkCanvasIos
}

inkcanvas_macos {
    DEFINES += INKCANVAS_MACOS=1
    TARGET = InkCanvasMacos
}

include(../config.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += DEBUG_RENDERING_FEEDBACK=0
DEFINES += DEBUG_LASSO_FEEDBACK=0
DEFINES += DEBUG_OUTPUT=0
DEFINES += OLD_ISF=0

SOURCES += \
    cmath.cpp \
    double.cpp \
    eventargs.cpp \
    guid.cpp \
    single.cpp \
    variant.cpp

HEADERS += \
    InkCanvas_global.h \
    cmath.h \
    double.h \
    single.h \
    eventargs.h \
    guid.h \
    variant.h

!inkcanvas_core {

RESOURCES += \
    inkcanvas.qrc

}

include(Windows/Windows.pri)
include(Internal/Internal.pri)
include(Activities/Activities.pri)
include(Collections/Collections.pri)
include(Landing/Landing.pri)

includes.files = $$PWD/*.h
includes.windows.files = $$PWD/Windows/*.h
includes.windows.control.files = $$PWD/Windows/Controls/*.h
includes.windows.ink.files = $$PWD/Windows/Ink/*.h
includes.windows.media.files = $$PWD/Windows/Media/*.h
includes.windows.input.files = $$PWD/Windows/Input/*.h
includes.internal.files = $$PWD/Internal/*.h
includes.internal.control.files = $$PWD/Internal/Controls/*.h
includes.internal.ink.files = $$PWD/Internal/Ink/*.h
includes.path = $$[QT_INSTALL_HEADERS]/InkCanvas
includes.windows.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Windows
includes.windows.control.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Windows/Controls
includes.windows.ink.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Windows/Ink
includes.windows.media.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Windows/Media
includes.windows.input.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Windows/Input
includes.internal.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Internal
includes.internal.control.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Internal/Controls
includes.internal.ink.path = $$[QT_INSTALL_HEADERS]/InkCanvas/Internal/Ink
target.path = $$[QT_INSTALL_LIBS]

INSTALLS += includes \
    includes.windows \
    includes.windows.control \
    includes.windows.ink \
    includes.windows.input \
    includes.windows.media \
    includes.internal \
    includes.internal.control \
    includes.internal.ink \

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32 {
    #INCLUDEPATH += "C:\Program Files (x86)\Microsoft Tablet PC Platform SDK\Include"
    LIBS += -ladvapi32
    #LIBS += -L$$PWD -lmshwgst
}

mac: LIBS += -framework AppKit
