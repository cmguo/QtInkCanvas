#include "qtstreamgeometrycontext.h"
#include "Windows/Media/streamgeometry.h"

QtStreamGeometryContext::QtStreamGeometryContext(StreamGeometry* geometry)
    : geometry_(geometry)
{
}

void QtStreamGeometryContext::BeginFigure(const QPointF &startPoint, bool isFilled, bool isClosed)
{
    if (isStarted_) {
        if (isClosed_)
            path_.closeSubpath();
    } else {
        isStarted_ = true;
    }
    isFilled_ = isFilled;
    isClosed_ = isClosed;
    path_.moveTo(startPoint);
}

void QtStreamGeometryContext::LineTo(const QPointF &point, bool isStroked, bool isSmoothJoin)
{
    path_.lineTo(point);
}

void QtStreamGeometryContext::QuadraticBezierTo(const QPointF &point1, const QPointF &point2, bool isStroked, bool isSmoothJoin)
{
    path_.quadTo(point1, point2);
}

void QtStreamGeometryContext::BezierTo(const QPointF &point1, const QPointF &point2, const QPointF &point3, bool isStroked, bool isSmoothJoin)
{
    path_.cubicTo(point1, point2, point3);
}

void QtStreamGeometryContext::PolyLineTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin)
{
    for (QPointF const & pt : points)
        path_.lineTo(pt);
}

void QtStreamGeometryContext::PolyQuadraticBezierTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.size(); i += 2) {
        path_.quadTo(points[i], points[i + 1]);
    }
}

void QtStreamGeometryContext::PolyBezierTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.size(); i += 3) {
        path_.cubicTo(points[i], points[i + 1], points[i + 2]);
    }
}

void QtStreamGeometryContext::ArcTo(const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin)
{
    //QRectF rect(QPointF(0, 0), size);
    //rect.moveCenter(point);
    path_.lineTo(point);
}

void QtStreamGeometryContext::DisposeCore()
{
    if (isStarted_) {
        if (isClosed_)
            path_.closeSubpath();
        isStarted_ = false;
    }
    geometry_->Close(path_);
    path_ = QPainterPath();
}

void QtStreamGeometryContext::SetClosedState(bool closed)
{
    isClosed_ = closed;
}
