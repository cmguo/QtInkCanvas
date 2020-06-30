#ifndef INKCANVASQT_H
#define INKCANVASQT_H

#include "InkCanvas_global.h"
#include "sharedptr.h"

#include <QPointF>
#include <QList>

class QPainterPath;

INKCANVAS_BEGIN_NAMESPACE

class Stroke;
class INKCANVAS_EXPORT InkCanvasQt
{
public:
    static SharedPointer<Stroke> makeStroke(QList<QPointF> const & points,
                                            qreal width, bool fitToCorve, bool ellipseShape);

    static SharedPointer<Stroke> cloneStroke(SharedPointer<Stroke> stroke);

    static void transformStroke(SharedPointer<Stroke> stroke, QMatrix const & matrix);

    static void transformStroke(SharedPointer<Stroke> stroke, QRectF const & from, QRectF const & to);

    static bool hitTestStroke(SharedPointer<Stroke> stroke, QPointF const & point);

    static QPainterPath getStrokeGeometry(SharedPointer<Stroke> stroke, QRectF & bounds);
};

INKCANVAS_END_NAMESPACE

#endif // INKCANVASQT_H
