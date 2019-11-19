QT += gui

TEMPLATE = lib
DEFINES += INKCANVAS_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bezier.cpp \
    cuspdata.cpp \
    doubleutil.cpp \
    drawingattributes.cpp \
    drawingattributeserializer.cpp \
    drawingflags.cpp \
    inkcanvas.cpp \
    knownidcache.cpp \
    knownids.cpp \
    stroke.cpp \
    strokecollection.cpp \
    strokecollectionserializer.cpp \
    strokefindices.cpp \
    styluspoint.cpp \
    styluspointcollection.cpp \
    styluspointdescription.cpp \
    styluspointproperties.cpp \
    styluspointproperty.cpp \
    styluspointpropertyids.cpp \
    styluspointpropertyinfo.cpp \
    styluspointpropertyinfodefaults.cpp \
    stylusshape.cpp \
    stylustip.cpp

HEADERS += \
    InkCanvas_global.h \
    bezier.h \
    cuspdata.h \
    doubleutil.h \
    drawingattributes.h \
    drawingattributeserializer.h \
    drawingflags.h \
    inkcanvas.h \
    knownidcache.h \
    knownids.h \
    stroke.h \
    strokecollection.h \
    strokecollectionserializer.h \
    strokefindices.h \
    styluspoint.h \
    styluspointcollection.h \
    styluspointdescription.h \
    styluspointproperties.h \
    styluspointproperty.h \
    styluspointpropertyids.h \
    styluspointpropertyinfo.h \
    styluspointpropertyinfodefaults.h \
    stylusshape.h \
    stylustip.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
