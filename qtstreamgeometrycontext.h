#ifndef QTSTREAMGEOMETRYCONTEXT_H
#define QTSTREAMGEOMETRYCONTEXT_H

#include "streamgeometrycontext.h"

class QtStreamGeometryContext : public StreamGeometryContext
{
public:
    QtStreamGeometryContext();


    // StreamGeometryContext interface
public:
    virtual void Close() override;
    virtual void BeginFigure(const QPointF &startPoint, bool isFilled, bool isClosed) override;
    virtual void LineTo(const QPointF &point, bool isStroked, bool isSmoothJoin) override;
    virtual void QuadraticBezierTo(const QPointF &point1, const QPointF &point2, bool isStroked, bool isSmoothJoin) override;
    virtual void BezierTo(const QPointF &point1, const QPointF &point2, const QPointF &point3, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyLineTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyQuadraticBezierTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyBezierTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void ArcTo(const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin) override;
    virtual void DisposeCore() override;
    virtual void SetClosedState(bool closed) override;
};

#endif // QTSTREAMGEOMETRYCONTEXT_H