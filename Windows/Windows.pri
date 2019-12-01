include(Media/Media.pri)
include(Ink/Ink.pri)
include(Controls/Controls.pri)
include(Input/Input.pri)

HEADERS += \
    $$PWD/dependencyobject.h \
    $$PWD/dependencyproperty.h \
    $$PWD/dependencypropertychangedeventargs.h \
    $$PWD/effectivevalueentry.h \
    $$PWD/propertymetadata.h \
    $$PWD/routedeventargs.h \
    $$PWD/uielement.h

SOURCES += \
    $$PWD/dependencyobject.cpp \
    $$PWD/dependencyproperty.cpp \
    $$PWD/dependencypropertychangedeventargs.cpp \
    $$PWD/effectivevalueentry.cpp \
    $$PWD/propertymetadata.cpp \
    $$PWD/routedeventargs.cpp \
    $$PWD/uielement.cpp
