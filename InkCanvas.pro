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
    bezier.cpp \
    contoursegment.cpp \
    cuspdata.cpp \
    dependencyobject.cpp \
    dependencyproperty.cpp \
    dependencypropertychangedeventargs.cpp \
    doubleutil.cpp \
    drawingattributes.cpp \
    drawingattributeserializer.cpp \
    drawingcontext.cpp \
    drawingflags.cpp \
    dynamicrenderer.cpp \
    editingbehavior.cpp \
    editingcoordinator.cpp \
    effectivevalueentry.cpp \
    ellipticalnodeoperations.cpp \
    eraserbehavior.cpp \
    erasingstroke.cpp \
    extendedproperty.cpp \
    geometry.cpp \
    incrementalhittester.cpp \
    inkcanvas.cpp \
    inkcanvaseditingmode.cpp \
    inkcanvasselectioneditingeventargs.cpp \
    inkcanvasselectionhitresult.cpp \
    inkcollectionbehavior.cpp \
    knownidcache.cpp \
    knownids.cpp \
    lasso.cpp \
    lassoselectionbehavior.cpp \
    propertymetadata.cpp \
    qtstreamgeometrycontext.cpp \
    quad.cpp \
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
    contoursegment.h \
    cuspdata.h \
    dependencyobject.h \
    dependencyproperty.h \
    dependencypropertychangedeventargs.h \
    doubleutil.h \
    drawingattributes.h \
    drawingattributeserializer.h \
    drawingcontext.h \
    drawingflags.h \
    dynamicrenderer.h \
    editingbehavior.h \
    editingcoordinator.h \
    editingmode.h \
    effectivevalueentry.h \
    ellipticalnodeoperations.h \
    eraserbehavior.h \
    erasingstroke.h \
    events.h \
    extendedproperty.h \
    geometry.h \
    incrementalhittester.h \
    inkcanvas.h \
    inkcanvaseditingmode.h \
    inkcanvasselectioneditingeventargs.h \
    inkcanvasselectionhitresult.h \
    inkcollectionbehavior.h \
    knownidcache.h \
    knownids.h \
    lasso.h \
    lassoselectionbehavior.h \
    propertymetadata.h \
    qtstreamgeometrycontext.h \
    quad.h \
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
