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
    virtual void BeginFigure(const QPointF &startPoint, bool isFilled, bool isClosed) override;
    virtual void LineTo(const QPointF &point, bool isStroked, bool isSmoothJoin) override;
    virtual void QuadraticBezierTo(const QPointF &point1, const QPointF &point2, bool isStroked, bool isSmoothJoin) override;
    virtual void BezierTo(const QPointF &point1, const QPointF &point2, const QPointF &point3, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyLineTo(const List<QPointF> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyQuadraticBezierTo(const List<QPointF> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyBezierTo(const List<QPointF> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void ArcTo(const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin) override;
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
