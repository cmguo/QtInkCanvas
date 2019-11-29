#include "lassohelper.h"


const QColor LassoHelper::DotColor(Qt::yellow);     //FromArgb(1, 0.89f, 0.3607f, 0.1843f);
const QColor LassoHelper::DotCircumferenceColor(Qt::white);
/*
Visual* LassoHelper::GetVisual()
{
    EnsureVisual();
    return _containerVisual;
}

/// <summary>TBS</summary>
QVector<QPointF> LassoHelper::AddPoints(QList<QPointF> const & points)
{
    //if (null == points)
    //    throw new ArgumentNullException("points");

    // Lazy initialization.
    EnsureReady();

    QList<QPointF> justAdded;
    int count = points.size();
    for ( int i = 0; i < count ; i++ )
    {
        QPointF point = points[i];

        if (0 == _count)
        {
            AddLassoPoint(point);

            justAdded.Add(point);
            _lasso.Add(point);
            _boundingBox.Union(point);

            _firstLassoPoint = point;
            _lastLassoPoint = point;
            _count++;
        }
        else
        {
            Vector last2next = QPointF - _lastLassoPoint;
            double distanceSquared = last2next.LengthSquared;

            // Avoid using Sqrt when the distance is equal to the step.
            if (DoubleUtil.AreClose(MinDistanceSquared, distanceSquared))
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
                double step = Math.Sqrt(MinDistanceSquared / distanceSquared);
                QPointF last = _lastLassoPoint;
                for (double findex = step; findex < 1.0f; findex += step)
                {
                    QPointF lassoPoint = last + (last2next * findex);
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
    // Draw a line between the last QPointF and the first one.
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
void LassoHelper::AddLassoPoint(QPointF const & lassoPoint)
{
    DrawingVisual dv = new DrawingVisual();
    DrawingContext dc = null;
    try
    {
        dc = dv.RenderOpen();
        dc.DrawEllipse(_brush, _pen, lassoPoint, DotRadius, DotRadius);
    }
    finally
    {
        if (dc != null)
        {
            dc.Close();
        }
    }

    // Add the new visual to the container.
    _containerVisual.Children.Add(dv);
}

//#endregion

//#region ArePointsInLasso
/// <summary>Copy-pasted Platform's Lasso.Contains(...)</summary>
bool LassoHelper::ArePointsInLasso(QVector<QPointF> points, int percentIntersect)
{
    //System.Diagnostics.Debug.Assert(null != points);
    //System.Diagnostics.Debug.Assert((0 <= percentIntersect) && (100 >= percentIntersect));

    // Find out how many of the points need to be inside the lasso to satisfy the percentIntersect.
    int marginCount = (points.Length * percentIntersect) / 100;

    if ((0 == marginCount) || (50 <= ((points.Length * percentIntersect) % 100)))
    {
        marginCount++;
    }

    // Check if any QPointF on the stroke is within the lasso or not.
    // This is done by checking all segments on the left side of the point.
    // If the no of such segments is odd then the QPointF is within the lasso otherwise not.
    int countPointsInLasso = 0;

    foreach (Point QPointF in points)
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
bool LassoHelper::Contains(QPointF const & point)
{
    if (false == _boundingBox.Contains(point))
    {
        return false;
    }

    bool isHigher = false;
    int last = _lasso.Count;

    while (--last >= 0)
    {
        if (false == DoubleUtil.AreClose(_lasso[last].Y, point.Y))
        {
            isHigher = point.Y < _lasso[last].Y;
            break;
        }
    }

    bool isInside = false, isOnClosingSegment = false;
    QPointF prevLassoPoint = _lasso[_lasso.Count - 1];

    for (int i = 0; i < _lasso.Count; i++)
    {
        QPointF lassoPoint = _lasso[i];

        if (DoubleUtil.AreClose(lassoPoint.Y, point.Y))
        {
            if (DoubleUtil.AreClose(lassoPoint.X, point.X))
            {
                isInside = true;
                break;
            }

            if ((0 != i) && DoubleUtil.AreClose(prevLassoPoint.Y, point.Y)
                && DoubleUtil.GreaterThanOrClose(point.X, Math.Min(prevLassoPoint.X, lassoPoint.X))
                && DoubleUtil.LessThanOrClose(point.X, Math.Max(prevLassoPoint.X, lassoPoint.X)))
            {
                isInside = true;
                break;
            }
        }
        else if (isHigher != (point.Y < lassoPoint.Y))
        {
            isHigher = !isHigher;
            if (DoubleUtil.GreaterThanOrClose(point.X, Math.Max(prevLassoPoint.X, lassoPoint.X)))
            {
                // there certainly is an intersection on the left
                isInside = !isInside;

                // The closing segment is the only exclusive one. Special case it.
                if ((0 == i) && DoubleUtil.AreClose(point.X, Math.Max(prevLassoPoint.X, lassoPoint.X)))
                {
                    isOnClosingSegment = true;
                }
            }
            else if (DoubleUtil.GreaterThanOrClose(point.X, Math.Min(prevLassoPoint.X, lassoPoint.X)))
            {
                // The X of the QPointF lies within the x ranges for the segment.
                // Calculate the x value of the QPointF where the segment intersects with the line.
                Vector lassoSegment = lassoPoint - prevLassoPoint;
                double x = prevLassoPoint.X + (lassoSegment.X / lassoSegment.Y) * (point.Y - prevLassoPoint.Y);

                if (DoubleUtil.GreaterThanOrClose(point.X, x))
                {
                    isInside = !isInside;
                    if ((0 == i) && DoubleUtil.AreClose(point.X, x))
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
    if (null == _containerVisual)
    {
        _containerVisual = new DrawingVisual();
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

        _brush = new SolidColorBrush(DotColor);
        _brush.Freeze();

        //_linePen = new Pen(new SolidColorBrush(Colors.DarkGray), ConnectLineThickness);
        //_linePen.Brush.Opacity = ConnectLineOpacity;
        //_linePen.LineJoin = PenLineJoin.Round;

        _pen = new Pen(new SolidColorBrush(DotCircumferenceColor), DotCircumferenceThickness);
        _pen.LineJoin = PenLineJoin.Round;
        _pen.Freeze();

        _lasso = new List<Point>(100);
        _boundingBox = Rect.Empty;

        _count = 0;
    }
}
*/
