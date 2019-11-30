#include "lassohelper.h"
#include "drawingvisual.h"
#include "drawingcontext.h"
#include "finallyhelper.h"
#include "doubleutil.h"

const QColor LassoHelper::DotColor(Qt::yellow);     //FromArgb(1, 0.89f, 0.3607f, 0.1843f);
const QColor LassoHelper::DotCircumferenceColor(Qt::white);

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

            justAdded.append(point);
            _lasso.append(point);
            _boundingBox |= QRectF(point, point);

            _firstLassoPoint = point;
            _lastLassoPoint = point;
            _count++;
        }
        else
        {
            QPointF last2next = point - _lastLassoPoint;
            double distanceSquared = LengthSquared(last2next);

            // Avoid using Sqrt when the distance is equal to the step.
            if (DoubleUtil::AreClose(MinDistanceSquared, distanceSquared))
            {
                AddLassoPoint(point);
                justAdded.append(point);
                _lasso.append(point);
                _boundingBox |= QRectF(point, point);

                _lastLassoPoint = point;
                _count++;

            }
            else if (MinDistanceSquared < distanceSquared)
            {
                double step = sqrt(MinDistanceSquared / distanceSquared);
                QPointF last = _lastLassoPoint;
                for (double findex = step; findex < 1.0; findex += step)
                {
                    QPointF lassoPoint = last + (last2next * findex);
                    AddLassoPoint(lassoPoint);
                    justAdded.append(lassoPoint);
                    _lasso.append(lassoPoint);
                    _boundingBox |= QRectF(lassoPoint, lassoPoint);

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

    return justAdded.toVector();
}

///// <summary>
///// Draws a single lasso dot with the center at the given point.
///// </summary>
void LassoHelper::AddLassoPoint(QPointF const & lassoPoint)
{
    DrawingVisual* dv = new DrawingVisual;
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
    _containerVisual->Children().append(dv);
}

//#endregion

//#region ArePointsInLasso
/// <summary>Copy-pasted Platform's Lasso.Contains(...)</summary>
bool LassoHelper::ArePointsInLasso(QVector<QPointF> points, int percentIntersect)
{
    //System.Diagnostics.Debug.Assert(null != points);
    //System.Diagnostics.Debug.Assert((0 <= percentIntersect) && (100 >= percentIntersect));

    // Find out how many of the points need to be inside the lasso to satisfy the percentIntersect.
    int marginCount = (points.size() * percentIntersect) / 100;

    if ((0 == marginCount) || (50 <= ((points.size() * percentIntersect) % 100)))
    {
        marginCount++;
    }

    // Check if any QPointF on the stroke is within the lasso or not.
    // This is done by checking all segments on the left side of the point.
    // If the no of such segments is odd then the QPointF is within the lasso otherwise not.
    int countPointsInLasso = 0;

    for (QPointF point : points)
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
    if (false == _boundingBox.contains(point))
    {
        return false;
    }

    bool isHigher = false;
    int last = _lasso.size();

    while (--last >= 0)
    {
        if (false == DoubleUtil::AreClose(_lasso[last].y(), point.y()))
        {
            isHigher = point.y() < _lasso[last].y();
            break;
        }
    }

    bool isInside = false, isOnClosingSegment = false;
    QPointF prevLassoPoint = _lasso[_lasso.size() - 1];

    for (int i = 0; i < _lasso.size(); i++)
    {
        QPointF lassoPoint = _lasso[i];

        if (DoubleUtil::AreClose(lassoPoint.y(), point.y()))
        {
            if (DoubleUtil::AreClose(lassoPoint.x(), point.x()))
            {
                isInside = true;
                break;
            }

            if ((0 != i) && DoubleUtil::AreClose(prevLassoPoint.y(), point.y())
                && DoubleUtil::GreaterThanOrClose(point.x(), qMin(prevLassoPoint.x(), lassoPoint.x()))
                && DoubleUtil::LessThanOrClose(point.x(), qMax(prevLassoPoint.x(), lassoPoint.x())))
            {
                isInside = true;
                break;
            }
        }
        else if (isHigher != (point.y() < lassoPoint.y()))
        {
            isHigher = !isHigher;
            if (DoubleUtil::GreaterThanOrClose(point.x(), qMax(prevLassoPoint.x(), lassoPoint.x())))
            {
                // there certainly is an intersection on the left
                isInside = !isInside;

                // The closing segment is the only exclusive one. Special case it.
                if ((0 == i) && DoubleUtil::AreClose(point.x(), qMax(prevLassoPoint.x(), lassoPoint.x())))
                {
                    isOnClosingSegment = true;
                }
            }
            else if (DoubleUtil::GreaterThanOrClose(point.x(), qMin(prevLassoPoint.x(), lassoPoint.x())))
            {
                // The X of the QPointF lies within the x ranges for the segment.
                // Calculate the x value of the QPointF where the segment intersects with the line.
                QPointF lassoSegment = lassoPoint - prevLassoPoint;
                double x = prevLassoPoint.x() + (lassoSegment.x() / lassoSegment.y()) * (point.y() - prevLassoPoint.y());

                if (DoubleUtil::GreaterThanOrClose(point.x(), x))
                {
                    isInside = !isInside;
                    if ((0 == i) && DoubleUtil::AreClose(point.x(), x))
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

