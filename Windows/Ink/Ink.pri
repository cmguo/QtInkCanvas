HEADERS += \
    $$PWD/applicationgesture.h \
    $$PWD/drawingattributes.h \
    $$PWD/drawingflags.h \
    $$PWD/events.h \
    $$PWD/extendedproperty.h \
    $$PWD/extendedpropertycollection.h \
    $$PWD/gesturerecognitionresult.h \
    $$PWD/incrementalhittester.h \
    $$PWD/knownids.h \
    $$PWD/stroke.h \
    $$PWD/strokecollection.h \
    $$PWD/strokeintersection.h \
    $$PWD/stylusshape.h \
    $$PWD/stylustip.h

SOURCES += \
    $$PWD/applicationgesture.cpp \
    $$PWD/drawingattributes.cpp \
    $$PWD/drawingflags.cpp \
    $$PWD/extendedproperty.cpp \
    $$PWD/extendedpropertycollection.cpp \
    $$PWD/gesturerecognitionresult.cpp \
    $$PWD/incrementalhittester.cpp \
    $$PWD/knownids.cpp \
    $$PWD/stroke.cpp \
    $$PWD/strokecollection.cpp \
    $$PWD/strokeintersection.cpp \
    $$PWD/stylusshape.cpp \
    $$PWD/stylustip.cpp

win32 {
HEADERS += \
    $$PWD/gesturerecognizer.h \

SOURCES += \
    $$PWD/gesturerecognizer.cpp \
}
