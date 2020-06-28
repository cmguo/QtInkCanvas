#ifndef LASSOHELPER_H
#define LASSOHELPER_H

#include "InkCanvas_global.h"
#include "Windows/rect.h"
#include "Collections/Generic/list.h"

#include <QColor>
#include <QBrush>
#include <QPen>

INKCANVAS_BEGIN_NAMESPACE

class DrawingVisual;
class Visual;

// namespace MS.Internal.Ink

/// <summary>
/// An internal helper class to draw lasso as a sequence of dots
/// closed with a rubber line. LassoSelectionBehavior creates an object of
/// this class to render a single lasso, so, for simplicity,
/// LassoHelper objects are indended for one-time use only.
/// </summary>
class LassoHelper
{
    //#region Fields

    // Visuals, geometry builders and drawing stuff
    DrawingVisual*       _containerVisual = nullptr;
    QBrush               _brush;
    QPen                 _pen;
    //Pen                 _linePen = null;

    //
    bool                _isActivated = false;
    Point               _firstLassoPoint;
    Point               _lastLassoPoint;
    int                 _count = 0;

    // Entire lasso. Collected to hit test InkCanvas' subelements after stylus up.
    List<Point>         _lasso;
    Rect                _boundingBox;

    // NTRAID#T2-00000-2003/07/14-vsmirnov - some of these are probably not in sync
    // with the spec (which is not available at this moment), and also might
    // need to be different for the high contrast mode.
public:
    static constexpr double  MinDistanceSquared     = 49.0;
    static constexpr double  DotRadius                     = 2.5;
    static constexpr double  DotCircumferenceThickness     = 0.5;
    static constexpr double  ConnectLineThickness          = 0.75;
    static constexpr double  ConnectLineOpacity            = 0.75;
    static const QColor DotColor;     //FromArgb(1, 0.89f, 0.3607f, 0.1843f);
    static const QColor DotCircumferenceColor;

    //#endregion

    //#region API
    /// <summary>
    /// Read-only access to the container visual for dynamic drawing a lasso
    /// </summary>
public:
    ~LassoHelper();

    Visual* GetVisual();

    /// <summary>TBS</summary>
    Array<Point> AddPoints(List<Point> const & points);

    //#region ArePointsInLasso
    /// <summary>Copy-pasted Platform's Lasso.Contains(...)</summary>
    bool ArePointsInLasso(Array<Point> const & points, int percentIntersect);

private:
    ///// <summary>
    ///// Draws a single lasso dot with the center at the given point.
    ///// </summary>
    void AddLassoPoint(Point const & lassoPoint);

    //#endregion

    /// <summary>TBS</summary>
    bool Contains(Point const & point);
    //#endregion

    //#region Implementation helpers
    /// <summary> Creates the container visual when needed.</summary>
    void EnsureVisual();

    /// <summary>
    /// Creates and initializes objects required for drawing
    /// </summary>
    void EnsureReady();

    //#endregion
};

INKCANVAS_END_NAMESPACE

#endif // LASSOHELPER_H
