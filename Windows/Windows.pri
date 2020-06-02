include(Media/Media.pri)
include(Ink/Ink.pri)
!inkcanvas_core: include(Controls/Controls.pri)
include(Input/Input.pri)

HEADERS += \
    $$PWD/point.h \
    $$PWD/rect.h \
    $$PWD/size.h \
    $$PWD/vector.h

SOURCES += \
    $$PWD/point.cpp \
    $$PWD/rect.cpp \
    $$PWD/size.cpp \
    $$PWD/vector.cpp

!inkcanvas_core: {

    HEADERS += \
        $$PWD/dependencyobject.h \
        $$PWD/dependencyproperty.h \
        $$PWD/dependencypropertychangedeventargs.h \
        $$PWD/dispatcher.h \
        $$PWD/effectivevalueentry.h \
        $$PWD/propertymetadata.h \
        $$PWD/routedeventargs.h \
        $$PWD/uielement.h \

    SOURCES += \
        $$PWD/dependencyobject.cpp \
        $$PWD/dependencyproperty.cpp \
        $$PWD/dependencypropertychangedeventargs.cpp \
        $$PWD/dispatcher.cpp \
        $$PWD/effectivevalueentry.cpp \
        $$PWD/propertymetadata.cpp \
        $$PWD/routedeventargs.cpp \
        $$PWD/uielement.cpp \

}
