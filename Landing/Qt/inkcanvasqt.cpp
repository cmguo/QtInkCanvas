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

void InkCanvasQt::InkCanvasQt::makeStroke(SharedPointer<Stroke> & stroke, const QList<QPointF> &points,
                                          qreal width, bool fitToCorve, bool ellipseShape, bool addPressure)
{
    SharedPointer<DrawingAttributes> da(new MyDrawingAttribute(width, fitToCorve, ellipseShape));
    QSharedPointer<StylusPointCollection> stylusPoints(
                new StylusPointCollection);
    for (QPointF const & pt : points) {
        StylusPoint point(pt.x(), pt.y(), 1.0 /*pressure*/);
        stylusPoints->Add(point);
    }
    if (addPressure) {
        int n = 16;
        if (stylusPoints->Count() > n) {
            for (int i = 1; i < n; ++i) {
                int m = stylusPoints->Count() + i - n;
                StylusPoint point = (*stylusPoints)[m];
                float d = static_cast<float>(i) / static_cast<float>(n);
                point.SetPressureFactor(point.PressureFactor() * (1.0f - d * d));
                stylusPoints->SetItem(m, point);
            }
            --n;
        } else {
            n = 0;
        }
    }
    stroke.reset(new Stroke(stylusPoints, da));
}

void InkCanvasQt::cloneStroke(SharedPointer<Stroke> & out, SharedPointer<Stroke> const & stroke)
{
    out = stroke->Clone();
}

void InkCanvasQt::shareStroke(SharedPointer<Stroke> &out, const SharedPointer<Stroke> & stroke)
{
    out = stroke;
}

void InkCanvasQt::transformStroke(SharedPointer<Stroke> const & stroke, QMatrix const &matrix)
{
    stroke->Transform(matrix, false);
}

bool InkCanvasQt::hitTestStroke(SharedPointer<Stroke> const & stroke, const QPointF &point)
{
    return stroke->HitTest(point);
}

void InkCanvasQt::getStrokeGeometry(SharedPointer<Stroke> const & stroke, QPainterPath & path, QRectF & bounds)
{
    stroke->GetGeometry();
    path = static_cast<StreamGeometry*>(stroke->GetGeometry())->path();
    bounds = stroke->GetBounds();
}

INKCANVAS_END_NAMESPACE
