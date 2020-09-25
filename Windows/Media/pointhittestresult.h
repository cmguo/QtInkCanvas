#ifndef WINDOWS_MEDIA_POINTHITTESTRESULT_H
#define WINDOWS_MEDIA_POINTHITTESTRESULT_H

#include "Windows/Media/hittestresult.h"
#include "Windows/point.h"

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class UIElement;

/// <summary>
/// This class returns the point and QWidget hit during a hit test pass.
/// </summary>
class PointHitTestResult : public HitTestResult
{
private:
    Point _pointHit;

    /// <summary>
    /// This constructor takes a QWidget and point respresenting a hit.
    /// </summary>
public:
    PointHitTestResult();

    PointHitTestResult(UIElement * visualHit, Point const & pointHit);

    /// <summary>
    /// The point in local space of the hit QWidget.
    /// </summary>
    Point PointHit()
    {
        return _pointHit;
    }

    /// <summary>
    ///     Re-expose QWidget property strongly typed to 2D QWidget.
    /// </summary>
    UIElement* VisualHit();
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_MEDIA_POINTHITTESTRESULT_H
