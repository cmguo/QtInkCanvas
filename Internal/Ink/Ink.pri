!inkcanvas_core: include(InkSerializedFormat/InkSerializedFormat.pri)
!inkcanvas_core: win32: include(GestureRecognition/GestureRecognition.pri)

HEADERS += \
    $$PWD/bezier.h \
    $$PWD/contoursegment.h \
    $$PWD/cuspdata.h \
    $$PWD/ellipticalnodeoperations.h \
    $$PWD/erasingstroke.h \
    $$PWD/quad.h \
    $$PWD/lasso.h \
    $$PWD/strokefindices.h \
    $$PWD/strokenode.h \
    $$PWD/strokenodedata.h \
    $$PWD/strokenodeiterator.h \
    $$PWD/strokenodeoperations.h \
    $$PWD/strokerenderer.h \

SOURCES += \
    $$PWD/bezier.cpp \
    $$PWD/contoursegment.cpp \
    $$PWD/cuspdata.cpp \
    $$PWD/ellipticalnodeoperations.cpp \
    $$PWD/erasingstroke.cpp \
    $$PWD/quad.cpp \
    $$PWD/lasso.cpp \
    $$PWD/strokefindices.cpp \
    $$PWD/strokenode.cpp \
    $$PWD/strokenodedata.cpp \
    $$PWD/strokenodeiterator.cpp \
    $$PWD/strokenodeoperations.cpp \
    $$PWD/strokerenderer.cpp \

inkcanvas_core: {

HEADERS += \
    $$PWD/InkSerializedFormat/isftagandguidcache.h \

SOURCES += \
    $$PWD/InkSerializedFormat/isftagandguidcache.cpp \

}

!inkcanvas_core: {

HEADERS += \
    $$PWD/clipboardprocessor.h \
    $$PWD/editingbehavior.h \
    $$PWD/editingcoordinator.h \
    $$PWD/elementsclipboarddata.h \
    $$PWD/eraserbehavior.h \
    $$PWD/renderer.h \
    $$PWD/inkcanvasclipboardformat.h \
    $$PWD/inkcanvasselection.h \
    $$PWD/inkcollectionbehavior.h \
    $$PWD/isfclipboarddata.h \
    $$PWD/lassohelper.h \
    $$PWD/lassoselectionbehavior.h \
    $$PWD/pencursormanager.h \
    $$PWD/selectioneditingbehavior.h \
    $$PWD/selectioneditor.h \
    $$PWD/serializationflags.h \
    $$PWD/styluseditingbehavior.h \
    $$PWD/textclipboarddata.h

SOURCES += \
    $$PWD/clipboardprocessor.cpp \
    $$PWD/editingbehavior.cpp \
    $$PWD/editingcoordinator.cpp \
    $$PWD/elementsclipboarddata.cpp \
    $$PWD/eraserbehavior.cpp \
    $$PWD/renderer.cpp \
    $$PWD/inkcanvasclipboardformat.cpp \
    $$PWD/inkcanvasselection.cpp \
    $$PWD/inkcollectionbehavior.cpp \
    $$PWD/isfclipboarddata.cpp \
    $$PWD/lassohelper.cpp \
    $$PWD/lassoselectionbehavior.cpp \
    $$PWD/pencursormanager.cpp \
    $$PWD/selectioneditingbehavior.cpp \
    $$PWD/selectioneditor.cpp \
    $$PWD/serializationflags.cpp \
    $$PWD/styluseditingbehavior.cpp \
    $$PWD/textclipboarddata.cpp

}
