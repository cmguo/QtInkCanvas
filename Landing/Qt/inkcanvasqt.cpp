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

QSharedPointer<Stroke> InkCanvasQt::InkCanvasQt::createStroke(const QList<QPointF> &points, QList<float> const & pressures,
                                          qreal width, bool fitToCorve, bool ellipseShape, bool addPressure)
{
    QSharedPointer<DrawingAttributes> da(new MyDrawingAttribute(width, fitToCorve, ellipseShape));
    QSharedPointer<StylusPointCollection> stylusPoints(
                new StylusPointCollection);
    float p = 0.5f; /*pressure*/
    for (int i = 0; i < points.size(); ++i) {
        QPointF const & pt = points[i];
        if (!pressures.isEmpty())
            p = pressures[i];
        StylusPoint point(pt.x(), pt.y(), p);
        stylusPoints->Add(point);
    }
    if (pressures.isEmpty() && addPressure) {
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
    return QSharedPointer<Stroke>(new Stroke(stylusPoints, da));
}

QSharedPointer<Stroke> InkCanvasQt::createStroke(QPainterPath const & path, QColor color, qreal width)
{
    QSharedPointer<DrawingAttributes> da(new MyDrawingAttribute(width, true, true));
    da->SetColor(color);
    QSharedPointer<StylusPointCollection> stylusPoints(
                new StylusPointCollection);
    for (qreal t = 0; t < 1; t += 0.01) {
        QPointF pt = path.pointAtPercent(t);
        StylusPoint point(pt.x(), pt.y());
        stylusPoints->Add(point);
    }
    return QSharedPointer<Stroke>(new Stroke(stylusPoints, da));
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

QPainterPath const & InkCanvasQt::getStrokeGeometry(SharedPointer<Stroke> const & stroke, QRectF & bounds)
{
    stroke->GetGeometry();
    bounds = stroke->GetBounds();
    return *reinterpret_cast<QPainterPath*>(
                static_cast<StreamGeometry*>(stroke->GetGeometry())->path());
}

void InkCanvasQt::freeStroke(QSharedPointer<Stroke> &stroke)
{
    stroke.reset();
}

void InkCanvasQt::unload()
{
    QMetaType::unregisterType(qMetaTypeId<INKCANVAS_PREPEND_NAMESPACE(DrawingFlags)>());
    QMetaType::unregisterType(qMetaTypeId<INKCANVAS_PREPEND_NAMESPACE(StylusTip)>());
}

INKCANVAS_END_NAMESPACE
