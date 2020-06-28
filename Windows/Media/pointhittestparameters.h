#ifndef POINTHITTESTPARAMETERS_H
#define POINTHITTESTPARAMETERS_H

#include "Windows/Media/hittestparameters.h"
#include "Windows/point.h"

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// This is the class for specifying parameters hit testing with a point.
/// </summary>
class PointHitTestParameters : public HitTestParameters
{
public:
    /// <summary>
    /// The constructor takes the point to hit test with.
    /// </summary>
    PointHitTestParameters(Point const & point) : HitTestParameters()
    {
        _hitPoint = point;
    }

    /// <summary>
    /// The point to hit test against.
    /// </summary>
    Point & HitPoint()
    {
        return _hitPoint;
    }

    void SetHitPoint(Point const & hitPoint)
    {
        _hitPoint = hitPoint;
    }

private:
    Point _hitPoint;
};

INKCANVAS_END_NAMESPACE

#endif // POINTHITTESTPARAMETERS_H
