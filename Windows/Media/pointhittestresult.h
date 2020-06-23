#ifndef POINTHITTESTRESULT_H
#define POINTHITTESTRESULT_H

#include "hittestresult.h"

#include <QPointF>

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class UIElement;

/// <summary>
/// This class returns the point and QWidget hit during a hit test pass.
/// </summary>
class PointHitTestResult : public HitTestResult
{
private:
    QPointF _pointHit;

    /// <summary>
    /// This constructor takes a QWidget and point respresenting a hit.
    /// </summary>
public:
    PointHitTestResult();

    PointHitTestResult(UIElement * visualHit, QPointF const & pointHit);

    /// <summary>
    /// The point in local space of the hit QWidget.
    /// </summary>
    QPointF PointHit()
    {
        return _pointHit;
    }

    /// <summary>
    ///     Re-expose QWidget property strongly typed to 2D QWidget.
    /// </summary>
    UIElement* VisualHit();
};

INKCANVAS_END_NAMESPACE

#endif // POINTHITTESTRESULT_H
