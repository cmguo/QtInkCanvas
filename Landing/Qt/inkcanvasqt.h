#ifndef INKCANVASQT_H
#define INKCANVASQT_H

#include "InkCanvas_global.h"

#include <QSharedPointer>
#include <QPointF>
#include <QList>

class QPainterPath;

INKCANVAS_BEGIN_NAMESPACE

class Stroke;

class INKCANVAS_EXPORT InkCanvasQt
{
public:
    static void makeStroke(QSharedPointer<Stroke> & stroke, QList<QPointF> const & points, QList<float> const & pressures,
                                            qreal width, bool fitToCorve = true, bool ellipseShape = true, bool addPressure = true);

    static void cloneStroke(QSharedPointer<Stroke> & out, QSharedPointer<Stroke> const & stroke);

    // may release old from out
    static void shareStroke(QSharedPointer<Stroke> & out, QSharedPointer<Stroke> const & stroke);

    static void transformStroke(QSharedPointer<Stroke> const & stroke, QMatrix const & matrix);

    static bool hitTestStroke(QSharedPointer<Stroke> const & stroke, QPointF const & point);

    static void getStrokeGeometry(QSharedPointer<Stroke> const & stroke, QPainterPath & path, QRectF & bounds);

    static void freeStroke(QSharedPointer<Stroke> & stroke);
};

INKCANVAS_END_NAMESPACE

#endif // INKCANVASQT_H
