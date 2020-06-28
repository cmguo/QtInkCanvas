#include "Internal/Ink/lassohelper.h"
#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/drawingcontext.h"
#include "Internal/finallyhelper.h"
#include "Internal/doubleutil.h"

INKCANVAS_BEGIN_NAMESPACE

const QColor LassoHelper::DotColor(Qt::yellow);     //FromArgb(1, 0.89f, 0.3607f, 0.1843f);
const QColor LassoHelper::DotCircumferenceColor(Qt::white);

LassoHelper::~LassoHelper()
{
    if (_containerVisual)
        delete _containerVisual;
}

Visual* LassoHelper::GetVisual()
{
    EnsureVisual();
    return _containerVisual;
}

/// <summary>TBS</summary>
Array<Point> LassoHelper::AddPoints(List<Point> const & points)
{
    //if (null == points)
    //    throw new ArgumentNullException("points");

    // Lazy initialization.
    EnsureReady();

    List<Point> justAdded;
    int count = points.Count();
    for ( int i = 0; i < count ; i++ )
    {
        Point point = points[i];

        if (0 == _count)
        {
            AddLassoPoint(point);

            justAdded.Add(point);
            _lasso.Add(point);
            _boundingBox.Union(_boundingBox, point);

            _firstLassoPoint = point;
            _lastLassoPoint = point;
            _count++;
        }
        else
        {
            Vector last2next = point - _lastLassoPoint;
            double distanceSquared = last2next.LengthSquared();

            // Avoid using Sqrt when the distance is equal to the step.
            if (DoubleUtil::AreClose(MinDistanceSquared, distanceSquared))
            {
                AddLassoPoint(point);
                justAdded.Add(point);
                _lasso.Add(point);
                _boundingBox.Union(point);

                _lastLassoPoint = point;
                _count++;

            }
            else if (MinDistanceSquared < distanceSquared)
            {
                double step = sqrt(MinDistanceSquared / distanceSquared);
                Point last = _lastLassoPoint;
                for (double findex = step; findex < 1.0; findex += step)
                {
                    Point lassoPoint = last + (last2next * findex);
                    AddLassoPoint(lassoPoint);
                    justAdded.Add(lassoPoint);
                    _lasso.Add(lassoPoint);
                    _boundingBox.Union(lassoPoint);

                    _lastLassoPoint = lassoPoint;
                    _count++;

                }
            }
        }
    }

    // still working on perf here.
    // Draw a line between the last Point and the first one.
    //if (_count > 1)
    //{
    //    DrawingContext dc = _containerVisual.RenderOpen();
    //    dc.DrawLine(_linePen, _firstLassoPoint, _lastLassoPoint);
    //    dc.Close();
    //}

    return justAdded.ToArray();
}

///// <summary>
///// Draws a single lasso dot with the center at the given point.
///// </summary>
void LassoHelper::AddLassoPoint(Point const & lassoPoint)
{
    DrawingVisual* dv = new DrawingVisual;
    dv->setObjectName("LassoHelper::DrawingVisual");
    std::unique_ptr<DrawingContext> dc;
    //try
    {
        FinallyHelper final([&dc](){
            if (dc != nullptr)
            {
                dc->Close();
            }
        });
        dc.reset(dv->RenderOpen());
        dc->DrawEllipse(_brush, _pen, lassoPoint, DotRadius, DotRadius);
    }
    //finally
    //{
    //    if (dc != nullptr)
    //    {
    //        dc.Close();
    //    }
    //}

    // Add the new visual to the container.
    _containerVisual->Children().Add(dv);
}

//#endregion

//#region ArePointsInLasso
/// <summary>Copy-pasted Platform's Lasso.Contains(...)</summary>
bool LassoHelper::ArePointsInLasso(Array<Point> const & points, int percentIntersect)
{
    //System.Diagnostics.Debug.Assert(null != points);
    //System.Diagnostics.Debug.Assert((0 <= percentIntersect) && (100 >= percentIntersect));

    // Find out how many of the points need to be inside the lasso to satisfy the percentIntersect.
    int marginCount = (points.Length() * percentIntersect) / 100;

    if ((0 == marginCount) || (50 <= ((points.Length() * percentIntersect) % 100)))
    {
        marginCount++;
    }

    // Check if any Point on the stroke is within the lasso or not.
    // This is done by checking all segments on the left side of the point.
    // If the no of such segments is odd then the Point is within the lasso otherwise not.
    int countPointsInLasso = 0;

    for (Point point : points)
    {
        if (true == Contains(point))
        {
            countPointsInLasso++;
            if (countPointsInLasso == marginCount)
                break;
        }
    }

    return (countPointsInLasso == marginCount);
}

/// <summary>TBS</summary>
bool LassoHelper::Contains(Point const & point)
{
    if (false == _boundingBox.Contains(point))
    {
        return false;
    }

    bool isHigher = false;
    int last = _lasso.Count();

    while (--last >= 0)
    {
        if (false == DoubleUtil::AreClose(_lasso[last].Y(), point.Y()))
        {
            isHigher = point.Y() < _lasso[last].Y();
            break;
        }
    }

    bool isInside = false, isOnClosingSegment = false;
    Point prevLassoPoint = _lasso[_lasso.Count() - 1];

    for (int i = 0; i < _lasso.Count(); i++)
    {
        Point lassoPoint = _lasso[i];

        if (DoubleUtil::AreClose(lassoPoint.Y(), point.Y()))
        {
            if (DoubleUtil::AreClose(lassoPoint.X(), point.X()))
            {
                isInside = true;
                break;
            }

            if ((0 != i) && DoubleUtil::AreClose(prevLassoPoint.Y(), point.Y())
                && DoubleUtil::GreaterThanOrClose(point.X(), qMin(prevLassoPoint.X(), lassoPoint.X()))
                && DoubleUtil::LessThanOrClose(point.X(), qMax(prevLassoPoint.X(), lassoPoint.X())))
            {
                isInside = true;
                break;
            }
        }
        else if (isHigher != (point.Y() < lassoPoint.Y()))
        {
            isHigher = !isHigher;
            if (DoubleUtil::GreaterThanOrClose(point.X(), qMax(prevLassoPoint.X(), lassoPoint.X())))
            {
                // there certainly is an intersection on the left
                isInside = !isInside;

                // The closing segment is the only exclusive one. Special case it.
                if ((0 == i) && DoubleUtil::AreClose(point.X(), qMax(prevLassoPoint.X(), lassoPoint.X())))
                {
                    isOnClosingSegment = true;
                }
            }
            else if (DoubleUtil::GreaterThanOrClose(point.X(), qMin(prevLassoPoint.X(), lassoPoint.X())))
            {
                // The X of the Point lies within the x ranges for the segment.
                // Calculate the x value of the Point where the segment intersects with the line.
                Point lassoSegment = lassoPoint - prevLassoPoint;
                double x = prevLassoPoint.X() + (lassoSegment.X() / lassoSegment.Y()) * (point.Y() - prevLassoPoint.Y());

                if (DoubleUtil::GreaterThanOrClose(point.X(), x))
                {
                    isInside = !isInside;
                    if ((0 == i) && DoubleUtil::AreClose(point.X(), x))
                    {
                        isOnClosingSegment = true;
                    }
                }
            }
        }

        prevLassoPoint = lassoPoint;
    }

    return isInside ? !isOnClosingSegment : false;
}
//#endregion

//#region Implementation helpers
/// <summary> Creates the container visual when needed.</summary>
void LassoHelper::EnsureVisual()
{
    if (nullptr == _containerVisual)
    {
        _containerVisual = new DrawingVisual();
        _containerVisual->setObjectName("LassoHelper::ContainerVisual");
    }
}

/// <summary>
/// Creates and initializes objects required for drawing
/// </summary>
void LassoHelper::EnsureReady()
{
    if (false == _isActivated)
    {
        _isActivated = true;

        EnsureVisual();

        _brush = QBrush(DotColor);
        //_brush.Freeze();

        //_linePen = new Pen(new SolidColorBrush(Colors.DarkGray), ConnectLineThickness);
        //_linePen.Brush.Opacity = ConnectLineOpacity;
        //_linePen.LineJoin = PenLineJoin.Round;

        _pen = QPen(DotCircumferenceColor, DotCircumferenceThickness);
        _pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
        //_pen.Freeze();

        _lasso.reserve(100);
        //_boundingBox = Rect.Empty;

        _count = 0;
    }
}

INKCANVAS_END_NAMESPACE
