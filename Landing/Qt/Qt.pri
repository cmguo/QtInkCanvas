HEADERS += \
    $$PWD/inkcanvasqt.h \
    $$PWD/qtstreamgeometrycontext.h

SOURCES += \
    $$PWD/inkcanvasqt.cpp \
    $$PWD/qtstreamgeometrycontext.cpp

!inkcanvas_core {

    HEADERS += \
        $$PWD/qtpainterdrawingcontext.h

    SOURCES += \
        $$PWD/qtpainterdrawingcontext.cpp

}

