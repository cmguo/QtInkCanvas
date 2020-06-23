#ifndef STREAMGEOMETRYCONTEXT_H
#define STREAMGEOMETRYCONTEXT_H

#include "geometry.h"

#include <QPointF>
#include <QList>

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class StreamGeometryContext
{
public:
    /// <summary>
    /// This constructor exists to prevent external derivation
    /// </summary>
    StreamGeometryContext();


    virtual ~StreamGeometryContext()
    {
#if ! PBTCOMPILER
        //VerifyAccess();
#endif
        DisposeCore();
        //GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Closes the StreamContext and flushes the content.
    /// Afterwards the StreamContext can not be used anymore.
    /// This call does not require all Push calls to have been Popped.
    /// </summary>
    /// <exception cref="ObjectDisposedException">
    /// This call is illegal if this object has already been closed or disposed.
    /// </exception>
    virtual void Close()
    {
        DisposeCore();
    }


    /// <summary>
    /// BeginFigure - Start a new figure.
    /// </summary>
    virtual void BeginFigure(QPointF const & startPoint, bool isFilled, bool isClosed) = 0;

    /// <summary>
    /// LineTo - append a LineTo to the current figure.
    /// </summary>
    virtual void LineTo(QPointF const & point, bool isStroked, bool isSmoothJoin) = 0;

    /// <summary>
    /// QuadraticBezierTo - append a QuadraticBezierTo to the current figure.
    /// </summary>
    virtual void QuadraticBezierTo(QPointF const & point1, QPointF const & point2, bool isStroked, bool isSmoothJoin) = 0;

    /// <summary>
    /// BezierTo - apply a BezierTo to the current figure.
    /// </summary>
    virtual void BezierTo(QPointF const & point1, QPointF const & point2, QPointF const & point3, bool isStroked, bool isSmoothJoin) = 0;

    /// <summary>
    /// PolyLineTo - append a PolyLineTo to the current figure.
    /// </summary>
    virtual void PolyLineTo(QList<QPointF> const & points, bool isStroked, bool isSmoothJoin) = 0;

    /// <summary>
    /// PolyQuadraticBezierTo - append a PolyQuadraticBezierTo to the current figure.
    /// </summary>
    virtual void PolyQuadraticBezierTo(QList<QPointF> const & points, bool isStroked, bool isSmoothJoin) = 0;

    /// <summary>
    /// PolyBezierTo - append a PolyBezierTo to the current figure.
    /// </summary>
    virtual void PolyBezierTo(QList<QPointF> const & points, bool isStroked, bool isSmoothJoin) = 0;

    /// <summary>
    /// ArcTo - append an ArcTo to the current figure.
    /// </summary>

    // Special case this one. Bringing in sweep direction requires code-gen changes.
    //
#if !PBTCOMPILER
    virtual void ArcTo(QPointF const & point, QSizeF const & size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin) = 0;
#else
    virtual void ArcTo(QPointF const & point, QSizeF const & size, double rotationAngle, bool isLargeArc, bool sweepDirection, bool isStroked, bool isSmoothJoin) = 0;
#endif


    /// <summary>
    /// This is the same as the Close call:
    /// Closes the Context and flushes the content.
    /// Afterwards the Context can not be used anymore.
    /// This call does not require all Push calls to have been Popped.
    /// </summary>
    /// <exception cref="ObjectDisposedException">
    /// This call is illegal if this object has already been closed or disposed.
    /// </exception>
    virtual void DisposeCore() {}

    /// <summary>
    /// SetClosedState - Sets the current closed state of the figure.
    /// </summary>
    virtual void SetClosedState(bool closed) = 0;
};

INKCANVAS_END_NAMESPACE

#endif // STREAMGEOMETRYCONTEXT_H
