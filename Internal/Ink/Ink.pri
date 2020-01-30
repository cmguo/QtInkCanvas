include(InkSerializedFormat/InkSerializedFormat.pri)
win32: include(GestureRecognition/GestureRecognition.pri)

HEADERS += \
    $$PWD/bezier.h \
    $$PWD/clipboardprocessor.h \
    $$PWD/contoursegment.h \
    $$PWD/cuspdata.h \
    $$PWD/editingbehavior.h \
    $$PWD/editingcoordinator.h \
    $$PWD/elementsclipboarddata.h \
    $$PWD/ellipticalnodeoperations.h \
    $$PWD/eraserbehavior.h \
    $$PWD/erasingstroke.h \
    $$PWD/inkcanvasclipboardformat.h \
    $$PWD/inkcanvasselection.h \
    $$PWD/inkcollectionbehavior.h \
    $$PWD/isfclipboarddata.h \
    $$PWD/lasso.h \
    $$PWD/lassohelper.h \
    $$PWD/lassoselectionbehavior.h \
    $$PWD/pencursormanager.h \
    $$PWD/quad.h \
    $$PWD/renderer.h \
    $$PWD/selectioneditingbehavior.h \
    $$PWD/selectioneditor.h \
    $$PWD/serializationflags.h \
    $$PWD/strokefindices.h \
    $$PWD/strokenode.h \
    $$PWD/strokenodedata.h \
    $$PWD/strokenodeiterator.h \
    $$PWD/strokenodeoperations.h \
    $$PWD/strokerenderer.h \
    $$PWD/styluseditingbehavior.h \
    $$PWD/textclipboarddata.h

SOURCES += \
    $$PWD/bezier.cpp \
    $$PWD/clipboardprocessor.cpp \
    $$PWD/contoursegment.cpp \
    $$PWD/cuspdata.cpp \
    $$PWD/editingbehavior.cpp \
    $$PWD/editingcoordinator.cpp \
    $$PWD/elementsclipboarddata.cpp \
    $$PWD/ellipticalnodeoperations.cpp \
    $$PWD/eraserbehavior.cpp \
    $$PWD/erasingstroke.cpp \
    $$PWD/inkcanvasclipboardformat.cpp \
    $$PWD/inkcanvasselection.cpp \
    $$PWD/inkcollectionbehavior.cpp \
    $$PWD/isfclipboarddata.cpp \
    $$PWD/lasso.cpp \
    $$PWD/lassohelper.cpp \
    $$PWD/lassoselectionbehavior.cpp \
    $$PWD/pencursormanager.cpp \
    $$PWD/quad.cpp \
    $$PWD/renderer.cpp \
    $$PWD/selectioneditingbehavior.cpp \
    $$PWD/selectioneditor.cpp \
    $$PWD/serializationflags.cpp \
    $$PWD/strokefindices.cpp \
    $$PWD/strokenode.cpp \
    $$PWD/strokenodedata.cpp \
    $$PWD/strokenodeiterator.cpp \
    $$PWD/strokenodeoperations.cpp \
    $$PWD/strokerenderer.cpp \
    $$PWD/styluseditingbehavior.cpp \
    $$PWD/textclipboarddata.cpp
