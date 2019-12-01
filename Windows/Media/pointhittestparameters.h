#ifndef POINTHITTESTPARAMETERS_H
#define POINTHITTESTPARAMETERS_H

#include "hittestparameters.h"

#include <QPointF>

// namespace System.Windows.Media

/// <summary>
/// This is the class for specifying parameters hit testing with a point.
/// </summary>
class PointHitTestParameters : public HitTestParameters
{
public:
    /// <summary>
    /// The constructor takes the point to hit test with.
    /// </summary>
    PointHitTestParameters(QPointF const & point) : HitTestParameters()
    {
        _hitPoint = point;
    }

    /// <summary>
    /// The point to hit test against.
    /// </summary>
    QPointF & HitPoint()
    {
        return _hitPoint;
    }

    void SetHitPoint(QPointF const & hitPoint)
    {
        _hitPoint = hitPoint;
    }

private:
    QPointF _hitPoint;
};

#endif // POINTHITTESTPARAMETERS_H
