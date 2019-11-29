#include "pointhittestresult.h"

#include "uielement.h"

PointHitTestResult::PointHitTestResult(UIElement * visualHit, QPointF const & pointHit)
    : HitTestResult(visualHit)
{
    _pointHit = pointHit;
}


/// <summary>
///     Re-expose QWidget property strongly typed to 2D QWidget.
/// </summary>
UIElement* PointHitTestResult::VisualHit()
{
    return (UIElement*) HitTestResult::VisualHit();
}
