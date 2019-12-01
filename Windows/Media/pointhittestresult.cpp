#include "Windows/Media/pointhittestresult.h"

#include "Windows/uielement.h"

PointHitTestResult::PointHitTestResult()
    : HitTestResult(nullptr)
{

}

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
