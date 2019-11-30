QT += gui widgets

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
    applicationgesture.cpp \
    bezier.cpp \
    clipboarddata.cpp \
    clipboardprocessor.cpp \
    containervisual.cpp \
    contoursegment.cpp \
    cuspdata.cpp \
    debug.cpp \
    dependencyobject.cpp \
    dependencyproperty.cpp \
    dependencypropertychangedeventargs.cpp \
    doubleutil.cpp \
    drawing.cpp \
    drawingattributes.cpp \
    drawingattributeserializer.cpp \
    drawingcontext.cpp \
    drawingdrawingcontext.cpp \
    drawingflags.cpp \
    drawingvisual.cpp \
    dynamicrenderer.cpp \
    editingbehavior.cpp \
    editingcoordinator.cpp \
    editingmode.cpp \
    effectivevalueentry.cpp \
    elementsclipboarddata.cpp \
    ellipticalnodeoperations.cpp \
    eraserbehavior.cpp \
    erasingstroke.cpp \
    eventargs.cpp \
    extendedproperty.cpp \
    finallyhelper.cpp \
    geometry.cpp \
    gesturerecognitionresult.cpp \
    gesturerecognizer.cpp \
    hittestparameters.cpp \
    hittestresult.cpp \
    incrementalhittester.cpp \
    inkcanvas.cpp \
    inkcanvasclipboardformat.cpp \
    inkcanvasfeedbackadorner.cpp \
    inkcanvasinnercanvas.cpp \
    inkcanvasselection.cpp \
    inkcanvasselectionadorner.cpp \
    inkcanvasselectionhitresult.cpp \
    inkcollectionbehavior.cpp \
    inkevents.cpp \
    inkpresenter.cpp \
    inkrenderer.cpp \
    inputdevice.cpp \
    inputeventargs.cpp \
    isfclipboarddata.cpp \
    knownidcache.cpp \
    knownids.cpp \
    lasso.cpp \
    lassohelper.cpp \
    lassoselectionbehavior.cpp \
    mousebuttoneventargs.cpp \
    mouseeventargs.cpp \
    nativerecognizer.cpp \
    notifycollectionchangedeventargs.cpp \
    pencursormanager.cpp \
    pointhittestparameters.cpp \
    pointhittestresult.cpp \
    propertymetadata.cpp \
    qtstreamgeometrycontext.cpp \
    quad.cpp \
    querycursoreventargs.cpp \
    routedeventargs.cpp \
    selectioneditingbehavior.cpp \
    selectioneditor.cpp \
    streamgeometry.cpp \
    streamgeometrycontext.cpp \
    stroke.cpp \
    strokecollection.cpp \
    strokecollectionserializer.cpp \
    strokefindices.cpp \
    strokeintersection.cpp \
    strokenode.cpp \
    strokenodedata.cpp \
    strokenodeiterator.cpp \
    strokenodeoperations.cpp \
    strokerenderer.cpp \
    styluseditingbehavior.cpp \
    stylusplugin.cpp \
    styluspoint.cpp \
    styluspointcollection.cpp \
    styluspointdescription.cpp \
    styluspointproperties.cpp \
    styluspointproperty.cpp \
    styluspointpropertyids.cpp \
    styluspointpropertyinfo.cpp \
    styluspointpropertyinfodefaults.cpp \
    stylusshape.cpp \
    stylustip.cpp \
    textclipboarddata.cpp \
    uielement.cpp \
    visual.cpp

HEADERS += \
    InkCanvas_global.h \
    applicationgesture.h \
    bezier.h \
    clipboarddata.h \
    clipboardprocessor.h \
    containervisual.h \
    contoursegment.h \
    cuspdata.h \
    debug.h \
    dependencyobject.h \
    dependencyproperty.h \
    dependencypropertychangedeventargs.h \
    doubleutil.h \
    drawing.h \
    drawingattributes.h \
    drawingattributeserializer.h \
    drawingcontext.h \
    drawingdrawingcontext.h \
    drawingflags.h \
    drawingvisual.h \
    dynamicrenderer.h \
    editingbehavior.h \
    editingcoordinator.h \
    editingmode.h \
    effectivevalueentry.h \
    elementsclipboarddata.h \
    ellipticalnodeoperations.h \
    eraserbehavior.h \
    erasingstroke.h \
    eventargs.h \
    events.h \
    extendedproperty.h \
    finallyhelper.h \
    geometry.h \
    gesturerecognitionresult.h \
    gesturerecognizer.h \
    hittestparameters.h \
    hittestresult.h \
    incrementalhittester.h \
    inkcanvas.h \
    inkcanvasclipboardformat.h \
    inkcanvasfeedbackadorner.h \
    inkcanvasinnercanvas.h \
    inkcanvasselection.h \
    inkcanvasselectionadorner.h \
    inkcanvasselectionhitresult.h \
    inkcollectionbehavior.h \
    inkevents.h \
    inkpresenter.h \
    inkrenderer.h \
    inputdevice.h \
    inputeventargs.h \
    isfclipboarddata.h \
    knownidcache.h \
    knownids.h \
    lasso.h \
    lassohelper.h \
    lassoselectionbehavior.h \
    mousebuttoneventargs.h \
    mouseeventargs.h \
    nativerecognizer.h \
    notifycollectionchangedeventargs.h \
    pencursormanager.h \
    pointhittestparameters.h \
    pointhittestresult.h \
    propertymetadata.h \
    qtstreamgeometrycontext.h \
    quad.h \
    querycursoreventargs.h \
    routedeventargs.h \
    selectioneditingbehavior.h \
    selectioneditor.h \
    streamgeometry.h \
    streamgeometrycontext.h \
    stroke.h \
    strokecollection.h \
    strokecollectionserializer.h \
    strokefindices.h \
    strokeintersection.h \
    strokenode.h \
    strokenodedata.h \
    strokenodeiterator.h \
    strokenodeoperations.h \
    strokerenderer.h \
    styluseditingbehavior.h \
    stylusplugin.h \
    styluspoint.h \
    styluspointcollection.h \
    styluspointdescription.h \
    styluspointproperties.h \
    styluspointproperty.h \
    styluspointpropertyids.h \
    styluspointpropertyinfo.h \
    styluspointpropertyinfodefaults.h \
    stylusshape.h \
    stylustip.h \
    textclipboarddata.h \
    uielement.h \
    visual.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32 {
    INCLUDEPATH += "C:\Program Files (x86)\Microsoft Tablet PC Platform SDK\Include"
    LIBS += -ladvapi32 -L$$PWD -lmshwgst
}
