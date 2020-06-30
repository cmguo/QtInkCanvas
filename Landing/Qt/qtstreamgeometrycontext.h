#ifndef QTSTREAMGEOMETRYCONTEXT_H
#define QTSTREAMGEOMETRYCONTEXT_H

#include "Windows/Media/streamgeometrycontext.h"

#include <QPainterPath>

INKCANVAS_BEGIN_NAMESPACE

class StreamGeometry;

class QtStreamGeometryContext : public StreamGeometryContext
{
public:
    QtStreamGeometryContext(StreamGeometry* geometry);


    // StreamGeometryContext interface
public:
    virtual void BeginFigure(const Point &startPoint, bool isFilled, bool isClosed) override;
    virtual void LineTo(const Point &point, bool isStroked, bool isSmoothJoin) override;
    virtual void QuadraticBezierTo(const Point &point1, const Point &point2, bool isStroked, bool isSmoothJoin) override;
    virtual void BezierTo(const Point &point1, const Point &point2, const Point &point3, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyLineTo(const List<Point> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyQuadraticBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void ArcTo(const Point &point, const Size &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin) override;
    virtual void DisposeCore() override;
    virtual void SetClosedState(bool closed) override;

private:
    StreamGeometry* geometry_;
    QPainterPath path_;
    bool isStarted_ = false;
    bool isClosed_ = false;
    bool isFilled_ = false;
};

INKCANVAS_END_NAMESPACE

#endif // QTSTREAMGEOMETRYCONTEXT_H
