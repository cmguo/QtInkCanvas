HEADERS += \
    $$PWD/geometry.h \
    $$PWD/matrix.h \
    $$PWD/streamgeometry.h \
    $$PWD/streamgeometrycontext.h \

SOURCES += \
    $$PWD/geometry.cpp \
    $$PWD/matrix.cpp \
    $$PWD/streamgeometry.cpp \
    $$PWD/streamgeometrycontext.cpp \

!inkcanvas_core:  {

    HEADERS += \
        $$PWD/containervisual.h \
        $$PWD/drawing.h \
        $$PWD/drawingcontext.h \
        $$PWD/drawingdrawingcontext.h \
        $$PWD/drawingvisual.h \
        $$PWD/hittestparameters.h \
        $$PWD/hittestresult.h \
        $$PWD/pointhittestparameters.h \
        $$PWD/pointhittestresult.h \
        $$PWD/visual.h

    SOURCES += \
        $$PWD/containervisual.cpp \
        $$PWD/drawing.cpp \
        $$PWD/drawingcontext.cpp \
        $$PWD/drawingdrawingcontext.cpp \
        $$PWD/drawingvisual.cpp \
        $$PWD/hittestparameters.cpp \
        $$PWD/hittestresult.cpp \
        $$PWD/pointhittestparameters.cpp \
        $$PWD/pointhittestresult.cpp \
        $$PWD/visual.cpp
}
