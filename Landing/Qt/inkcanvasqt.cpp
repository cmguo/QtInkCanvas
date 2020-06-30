#include "inkcanvasqt.h"

#include <Internal/Ink/inkcanvasselection.h>
#include <QPainterPath>
#include <Windows/Ink/drawingattributes.h>
#include <Windows/Ink/stroke.h>
#include <Windows/Input/styluspointcollection.h>
#include <Windows/Media/streamgeometry.h>

INKCANVAS_BEGIN_NAMESPACE

class MyDrawingAttribute : public DrawingAttributes
{
public:
    MyDrawingAttribute(qreal width, bool fitToCorve, bool ellipseShape)
    {
        SetWidth(width);
        SetHeight(width);
        SetFitToCurve(fitToCorve);
        SetStylusTip(ellipseShape ? StylusTip::Ellipse : StylusTip::Rectangle);
    }
};

SharedPointer<Stroke> InkCanvasQt::InkCanvasQt::makeStroke(const QList<QPointF> &points, qreal width, bool fitToCorve, bool ellipseShape)
{
    SharedPointer<DrawingAttributes> da(new MyDrawingAttribute(width, fitToCorve, ellipseShape));
    QSharedPointer<StylusPointCollection> stylusPoints(
                new StylusPointCollection);
    for (QPointF const & pt : points) {
        StylusPoint point(pt.x(), pt.y(), 1.0 /*pressure*/);
        stylusPoints->Add(point);
    }
    SharedPointer<Stroke> st(new Stroke(stylusPoints, da));
    return st;
}

SharedPointer<Stroke> InkCanvasQt::cloneStroke(SharedPointer<Stroke> stroke)
{
    return stroke->Clone();
}

void InkCanvasQt::transformStroke(SharedPointer<Stroke> stroke, QMatrix const &matrix)
{
    stroke->Transform(matrix, false);
}

void InkCanvasQt::transformStroke(SharedPointer<Stroke> stroke, const QRectF &from, const QRectF &to)
{
    stroke->Transform(InkCanvasSelection::MapRectToRect(from, to), false);
}

bool InkCanvasQt::hitTestStroke(SharedPointer<Stroke> stroke, const QPointF &point)
{
    return stroke->HitTest(point);
}

QPainterPath InkCanvasQt::getStrokeGeometry(SharedPointer<Stroke> stroke, QRectF & bounds)
{
    stroke->GetGeometry();
    bounds = stroke->GetBounds();
    return static_cast<StreamGeometry*>(stroke->GetGeometry())->path();
}

INKCANVAS_END_NAMESPACE
