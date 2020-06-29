#include "Internal/Ink/lasso.h"
#include "Internal/Ink/strokenodeiterator.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Extends the lasso by appending more points
/// </summary>
/// <param name="points">new points</param>
void Lasso::AddPoints(List<Point> const & points)
{
    //System.Diagnostics.Debug.Assert(null != points);

    for (Point const & point : points)
    {
        AddPoint(point);
    }
}

/// <summary>
/// Appends a Point to the lasso
/// </summary>
/// <param name="point">new lasso point</param>
void Lasso::AddPoint(Point const & point)
{
    //System.Diagnostics.Debug.Assert(_points != null);
    if (!Filter(point))
    {
        // The Point is not filtered, add it to the lasso
        AddPointImpl(point);
    }
}

/// <summary>
/// This method implement the core algorithm to check whether a Point is within a polygon
/// that are formed by the lasso points.
/// </summary>
/// <param name="point"></param>
/// <returns>true if the Point is contained within the lasso; false otherwise </returns>
bool Lasso::Contains(Point const & point)
{
    //System.Diagnostics.Debug.Assert(_points != null);

    if (false == _bounds.Contains(point))
    {
        return false;
    }

    bool isHigher = false;
    int last = _points.Count();
    while (--last >= 0)
    {
        if (!DoubleUtil::AreClose(_points[last].Y(),point.Y()))
        {
            isHigher = (point.Y() < _points[last].Y());
            break;
        }
    }

    bool isInside = false;
    Point prevLassoPoint = _points[_points.Count() - 1];
    for (int i = 0; i < _points.Count(); i++)
    {
        Point lassoPoint = _points[i];
        if (DoubleUtil::AreClose(lassoPoint.Y(), point.Y()))
        {
            if (DoubleUtil::AreClose(lassoPoint.X(), point.X()))
            {
                isInside = true;
                break;
            }
            if ((0 != i) && DoubleUtil::AreClose(prevLassoPoint.Y(), point.Y()) &&
                DoubleUtil::GreaterThanOrClose(point.X(), Math::Min(prevLassoPoint.X(), lassoPoint.X())) &&
                DoubleUtil::LessThanOrClose(point.X(), Math::Max(prevLassoPoint.X(), lassoPoint.X())))
            {
                isInside = true;
                break;
            }
        }
        else if (isHigher != (point.Y() < lassoPoint.Y()))
        {
            isHigher = !isHigher;
            if (DoubleUtil::GreaterThanOrClose(point.X(), Math::Max(prevLassoPoint.X(), lassoPoint.X())))
            {
                // there certainly is an intersection on the left
                isInside = !isInside;
            }
            else if (DoubleUtil::GreaterThanOrClose(point.X(), Math::Min(prevLassoPoint.X(), lassoPoint.X())))
            {
                // The X of the Point lies within the x ranges for the segment.
                // Calculate the x value of the Point where the segment intersects with the line.
                Point lassoSegment = lassoPoint - prevLassoPoint;
                //System.Diagnostics.Debug.Assert(lassoSegment.Y() != 0);
                double x = prevLassoPoint.X() + (lassoSegment.X() / lassoSegment.Y()) * (point.Y() - prevLassoPoint.Y());
                if (DoubleUtil::GreaterThanOrClose(point.X(), x))
                {
                    isInside = !isInside;
                }
            }
        }
        prevLassoPoint = lassoPoint;
    }
    return isInside;
}

Array<StrokeIntersection> Lasso::HitTest(StrokeNodeIterator & iterator)
{
    //System.Diagnostics.Debug.Assert(_points != null);
    //System.Diagnostics.Debug.Assert(iterator != null);

    if (_points.Count() < 3)
    {
        //
        // it takes at least 3 points to create a lasso
        //
        return Array<StrokeIntersection>();
    }

    //
    // We're about to perform hit testing with a lasso.
    // To do so we need to iterate through each StrokeNode.
    // As we do, we calculate the bounding Rect between it
    // and the previous StrokeNode and store this in 'currentStrokeSegmentBounds'
    //
    // Next, we check to see if that StrokeNode pair's bounding box intersects
    // with the bounding box of the Lasso points.  If not, we continue iterating through
    // StrokeNode pairs.
    //
    // If it does, we do a more granular hit test by pairing points in the Lasso, getting
    // their bounding box and seeing if that bounding box intersects our current StrokeNode
    // pair
    //

    Point lastNodePosition;
    Point lassoLastPoint = _points[_points.Count() - 1];
    Rect currentStrokeSegmentBounds = Rect::Empty();

    // Initilize the current crossing to be an empty one
    LassoCrossing currentCrossing = LassoCrossing::EmptyCrossing();

    // Creat a list to hold all the crossings
    List<LassoCrossing> crossingList;
    for (int i = 0; i < iterator.Count(); i++)
    {
        StrokeNode strokeNode = iterator[i];
        Rect nodeBounds = strokeNode.GetBounds();
        currentStrokeSegmentBounds.Union(nodeBounds);

        // Skip the node if it's outside of the lasso's bounds
        if (currentStrokeSegmentBounds.IntersectsWith(_bounds) == true)
        {
            // currentStrokeSegmentBounds, made up of the bounding box of
            // this StrokeNode unioned with the last StrokeNode,
            // intersects the lasso bounding box.
            //
            // Now we need to iterate through the lasso points and find out where they cross
            //
            Point lastPoint = lassoLastPoint;
            for (Point point : _points)
            {
                //
                // calculate a segment of the lasso from the last point
                // to the current point
                //
                Rect lassoSegmentBounds(lastPoint, point);

                //
                // see if this lasso segment intersects with the current stroke segment
                //
                if (!currentStrokeSegmentBounds.IntersectsWith(lassoSegmentBounds))
                {
                    lastPoint = point;
                    continue;
                }

                //
                // the lasso segment DOES intersect with the current stroke segment
                // find out precisely where
                //
                StrokeFIndices strokeFIndices = strokeNode.CutTest(lastPoint, point);

                lastPoint = point;
                if (strokeFIndices.IsEmpty())
                {
                    // current lasso segment does not hit the stroke segment, continue with the next lasso point
                    continue;
                }

                // Create a potentially new crossing for the current hit testing result.
                LassoCrossing potentialNewCrossing(strokeFIndices, strokeNode);

                // Try to merge with the current crossing. If the merge is succussful (return true), the new crossing is actually
                // continueing the current crossing, so do not start a new crossing. Otherwise, start a new one and add the existing
                // one to the list.
                if (!currentCrossing.Merge(potentialNewCrossing))
                {
                    // start a new crossing and add the existing on to the list
                    crossingList.Add(currentCrossing);
                    currentCrossing = potentialNewCrossing;
                }
            }

        }

        // Continue with the next node
        currentStrokeSegmentBounds = nodeBounds;
        lastNodePosition = strokeNode.Position();
    }


    // Adding the last crossing to the list, if valid
    if (!currentCrossing.IsEmpty())
    {
        crossingList.Add(currentCrossing);
    }

    // Handle the special case of no intersection at all
    if (crossingList.Count() == 0)
    {
        // the stroke was either completely inside the lasso
        // or outside the lasso
        if (Contains(lastNodePosition))
        {
            Array<StrokeIntersection> strokeIntersections(1);
            strokeIntersections[0] = StrokeIntersection::Full();
            return strokeIntersections;
        }
        else
        {
            return Array<StrokeIntersection>();
        }
    }

    // It is still possible that the current crossing list is not sorted or overlapping.
    // Sort the list and merge the overlapping ones.
    SortAndMerge(crossingList);

    // Produce the hit test results and store them in a list
    List<StrokeIntersection> strokeIntersectionList ;
    ProduceHitTestResults(crossingList, strokeIntersectionList);

    return strokeIntersectionList.ToArray();
}

/// <summary>
/// Sort and merge the crossing list
/// </summary>
/// <param name="crossingList">The crossing list to sort/merge</param>
void Lasso::SortAndMerge(List<LassoCrossing> & crossingList)
{
    // Sort the crossings based on the BeginFIndex values
    std::sort(crossingList.begin(), crossingList.end());

    List<LassoCrossing> mergedList;
    LassoCrossing mcrossing = LassoCrossing::EmptyCrossing();
    for (LassoCrossing const & crossing : crossingList)
    {
        //System.Diagnostics.Debug.Assert(!crossing.IsEmpty && crossing.StartNode.IsValid && crossing.EndNode.IsValid);
        if (!mcrossing.Merge(crossing))
        {
            //System.Diagnostics.Debug.Assert(!mcrossing.IsEmpty && mcrossing.StartNode.IsValid && mcrossing.EndNode.IsValid);
            mergedList.Add(mcrossing);
            mcrossing = crossing;
        }
    }
    if (!mcrossing.IsEmpty())
    {
        //System.Diagnostics.Debug.Assert(!mcrossing.IsEmpty && mcrossing.StartNode.IsValid && mcrossing.EndNode.IsValid);
        mergedList.Add(mcrossing);
    }
    crossingList = mergedList;
}


/// <summary>
/// Helper function to find out whether a Point is inside the lasso
/// </summary>
bool Lasso::SegmentWithinLasso(StrokeNode & strokeNode, double fIndex)
{
    bool currentSegmentWithinLasso;
    if (DoubleUtil::AreClose(fIndex, StrokeFIndices::BeforeFirst))
    {
        // This should check against the very first stroke node
        currentSegmentWithinLasso = Contains(strokeNode.GetPointAt(0));
    }
    else if (DoubleUtil::AreClose(fIndex, StrokeFIndices::AfterLast))
    {
        // This should check against the last stroke node
        currentSegmentWithinLasso = Contains(strokeNode.Position());
    }
    else
    {
        currentSegmentWithinLasso = Contains(strokeNode.GetPointAt(fIndex));
    }

    return currentSegmentWithinLasso;
}

/// <summary>
/// Helper function to find out the hit test result
/// </summary>
void Lasso::ProduceHitTestResults(
                        List<LassoCrossing> & crossingList, List<StrokeIntersection> & strokeIntersections)
{
    bool previousSegmentInsideLasso = false;
    for (int x = 0; x <= crossingList.Count(); x++)
    {
        bool currentSegmentWithinLasso = false;
        bool canMerge = true;
        StrokeIntersection si;
        if (x == 0)
        {
            si.SetHitBegin(StrokeFIndices::BeforeFirst);
            si.SetInBegin(StrokeFIndices::BeforeFirst);
        }
        else
        {
            si.SetInBegin(crossingList[x - 1].FIndices.EndFIndex());
            si.SetHitBegin(crossingList[x - 1].FIndices.BeginFIndex());
            currentSegmentWithinLasso = SegmentWithinLasso(crossingList[x - 1].EndNode, si.InBegin());
        }

        if (x == crossingList.Count())
        {
            // NTRAID#WINOS-1132904-2005/04/27-XIAOTU: For a special case when the last intersection is something like (1.2, AL).
            // As a result the last InSegment should be empty.
            if (DoubleUtil::AreClose(si.InBegin(), StrokeFIndices::AfterLast))
            {
                si.SetInEnd(StrokeFIndices::BeforeFirst);
            }
            else
            {
                si.SetInEnd(StrokeFIndices::AfterLast);
            }
            si.SetHitEnd(StrokeFIndices::AfterLast);
        }
        else
        {
            si.SetInEnd(crossingList[x].FIndices.BeginFIndex());

            // NTRAID#WINOS-1132904-2005/04/27-XIAOTU: For a speical case when the first intersection is something like (BF, 0.67).
            // As a result the first InSegment should be empty
            if (DoubleUtil::AreClose(si.InEnd(), StrokeFIndices::BeforeFirst))
            {
                //System.Diagnostics.Debug.Assert(DoubleUtil::AreClose(si.InBegin, StrokeFIndices::BeforeFirst));
                si.SetInBegin(StrokeFIndices::AfterLast);
            }

            si.SetHitEnd(crossingList[x].FIndices.EndFIndex());
            currentSegmentWithinLasso = SegmentWithinLasso(crossingList[x].StartNode, si.InEnd());

            // NTRAID#WINOS-1141831-2005/04/27-XIAOTU: If both the start and end position of the current crossing is
            // outside the lasso, the crossing is a hit-only intersection, i.e., the in-segment is empty.
            if (!currentSegmentWithinLasso && !SegmentWithinLasso(crossingList[x].EndNode, si.HitEnd()))
            {
                currentSegmentWithinLasso = true;
                si.SetHitBegin(crossingList[x].FIndices.BeginFIndex());
                si.SetInBegin(StrokeFIndices::AfterLast);
                si.SetInEnd(StrokeFIndices::BeforeFirst);
                canMerge = false;
            }
        }

        if (currentSegmentWithinLasso)
        {
            if (x > 0 && previousSegmentInsideLasso && canMerge)
            {
                // we need to consolidate with the previous segment
                StrokeIntersection previousIntersection = strokeIntersections[strokeIntersections.Count() - 1];

                // For example: previousIntersection = [BF, AL, BF, 0.0027], si = [BF, 0.0027, 0.049, 0.063]
                if (previousIntersection.InSegment().IsEmpty())
                {
                    previousIntersection.SetInBegin(si.InBegin());
                }
                previousIntersection.SetInEnd(si.InEnd());
                previousIntersection.SetHitEnd(si.HitEnd());
                strokeIntersections[strokeIntersections.Count() - 1] = previousIntersection;
            }
            else
            {
                strokeIntersections.Add(si);
            }

            if (DoubleUtil::AreClose(si.HitEnd(), StrokeFIndices::AfterLast))
            {
                // The strokeIntersections already cover the end of the stroke. No need to continue.
                return;
            }
        }
        previousSegmentInsideLasso = currentSegmentWithinLasso;
    }
}

/// <summary>
/// This flag is set to true when a lasso Point has been modified or removed
/// from the list, which will invalidate incremental lasso hitteting
/// </summary>
bool Lasso::IsIncrementalLassoDirty()
{
    return _incrementalLassoDirty;
}
void Lasso::SetIsIncrementalLassoDirty(bool value)
{
    _incrementalLassoDirty = value;
}

/// <summary>
/// Get a reference to the lasso points store
/// </summary>
List<Point> & Lasso::PointsList()
{
    return _points;
}

/// <summary>
/// Filter out duplicate points (and maybe in the futuer colinear points).
/// Return true if the Point should be filtered
/// </summary>
bool Lasso::Filter(Point const & point)
{
    // First Point should not be filtered
    if (0 == _points.Count())
    {
        return false;
    }
    // ISSUE-2004/06/14-vsmirnov - If the new segment is collinear with the last one,
    // don't add the Point but modify the last Point instead.
    Point lastPoint = _points[_points.Count() - 1];
    Point vector = point - lastPoint;

    // The Point will be filtered out, i.e. not added to the list, if the distance to the previous Point is
    // within the tolerance
    return (Math::Abs(vector.X()) < MinDistance && Math::Abs(vector.Y()) < MinDistance);
}

/// <summary>
/// Implemtnation of add point
/// </summary>
/// <param name="point"></param>
void Lasso::AddPointImpl(Point const & point)
{
    _points.Add(point);
    _bounds.Union(point);
}


int Lasso::LassoCrossing::CompareTo(LassoCrossing const & crossing) const
{
    //System.Diagnostics.Debug.Assert(obj is LassoCrossing);
    if (crossing.IsEmpty() && IsEmpty())
    {
        return 0;
    }
    else if (crossing.IsEmpty())
    {
        return 1;
    }
    else if (IsEmpty())
    {
        return -1;
    }
    else
    {
        return FIndices.CompareTo(crossing.FIndices);
    }
}

bool Lasso::LassoCrossing::Merge(LassoCrossing const & crossing)
{
    if (crossing.IsEmpty())
    {
        return false;
    }

    if (FIndices.IsEmpty() && !crossing.IsEmpty())
    {
        FIndices = crossing.FIndices;
        StartNode = crossing.StartNode;
        EndNode = crossing.EndNode;
        return true;
    }

    if(DoubleUtil::GreaterThanOrClose(crossing.FIndices.EndFIndex(), FIndices.BeginFIndex()) &&
        DoubleUtil::GreaterThanOrClose(FIndices.EndFIndex(), crossing.FIndices.BeginFIndex()))
    {
        if (DoubleUtil::LessThan(crossing.FIndices.BeginFIndex(), FIndices.BeginFIndex()))
        {
            FIndices.SetBeginFIndex(crossing.FIndices.BeginFIndex());
            StartNode = crossing.StartNode;
        }

        if (DoubleUtil::GreaterThan(crossing.FIndices.EndFIndex(), FIndices.EndFIndex()))
        {
            FIndices.SetEndFIndex(crossing.FIndices.EndFIndex());
            EndNode = crossing.EndNode;
        }
        return true;
    }

    return false;
}

bool SingleLoopLasso::Filter(Point const & point)
{
    List<Point> points = PointsList();

    // First Point should not be filtered
    if (0 == points.Count())
    {
        // Just add the new Point to the lasso
        return false;
    }

    // Don't add this Point if the lasso already has a loop; or
    // if it's filtered by base class's filter.
    if (true == _hasLoop || true == Lasso::Filter(point))
    {
        // Don't add this Point to the lasso.
        return true;
    }

    double intersection = 0;

    // Now check whether the line lastPoint->point intersect with the
    // existing lasso.

    if (true == GetIntersectionWithExistingLasso(point, intersection))
    {
        //System.Diagnostics.Debug.Assert(intersection >= 0 && intersection <= points.Count() - 2);

        if (intersection == points.Count() - 2)
        {
            return true;
        }

        // Adding the new Point will form a loop
        int i = (int) intersection;

        if (!DoubleUtil::AreClose(i, intersection))
        {
            // Move points[i] to the intersection position
            Point intersectionPoint;
            intersectionPoint.setX(points[i].X() + (intersection - i) * (points[i + 1].X() - points[i].X()));
            intersectionPoint.setY(points[i].Y() + (intersection - i) * (points[i + 1].Y() - points[i].Y()));
            points[i] = intersectionPoint;
            SetIsIncrementalLassoDirty(true);
        }

        // Since the lasso has a self loop and the loop starts at points[i], points[0] to
        // points[i-1] should be removed
        if (i > 0)
        {
            points.RemoveRange(0, i /*count*/);   // Remove points[0] to points[i-1]
            SetIsIncrementalLassoDirty(true);
        }

        if (true == IsIncrementalLassoDirty())
        {
            // Update the bounds
            Rect bounds = Rect::Empty();
            for (int j = 0; j < points.Count(); j++)
            {
                bounds.Union(points[j]);
            }
            SetBounds(bounds);
        }

        // The lasso has a self_loop, any more points will be neglected.
        _hasLoop = true;

        // Don't add this Point to the lasso.
        return true;
    }

    // Just add the new Point to the lasso
    return false;
}

void SingleLoopLasso::AddPointImpl(Point const & point)
{
    _prevBounds = Bounds();
    Lasso::AddPointImpl(point);
}

/// <summary>
/// If the line _points[Count -1]->point insersect with the existing lasso, return true
/// and bIndex value is set to a doulbe value representing position of the intersection.
/// </summary>
bool SingleLoopLasso::GetIntersectionWithExistingLasso(Point const & point, double & bIndex)
{
    List<Point> points = PointsList();
    int count = points.Count();

    Rect newRect(points[count - 1], point);

    if (false == _prevBounds.IntersectsWith(newRect))
    {
        // The Point is not contained in the bound of the existing lasso, no intersection.
        return false;
    }

    for (int i = 0; i < count -2; i++)
    {
        Rect currRect(points[i], points[i+1]);
        if (!currRect.IntersectsWith(newRect))
        {
            continue;
        }

        double s = FindIntersection(points[count-1] - points[i],            /*hitBegin*/
                                            point - points[i],              /*hitEnd*/
                                            Point(0, 0),               /*orgBegin*/
                                            points[i+1] - points[i]         /*orgEnd*/);
        if (s >=0 && s <= 1)
        {
            // Intersection found, adjust the fIndex
            bIndex = i + s;
            return true;
        }
    }

    // No intersection
    return false;
}


/// <summary>
/// Finds the intersection between the segment [hitBegin, hitEnd] and the segment [orgBegin, orgEnd].
/// </summary>
double SingleLoopLasso::FindIntersection(Point const & hitBegin, Point const & hitEnd, Point const & orgBegin, Point const & orgEnd)
{
    //System.Diagnostics.Debug.Assert(hitEnd != hitBegin && orgBegin != orgEnd);

    //----------------------------------------------------------------------
    // Source: http://isc.faqs.org/faqs/graphics/algorithms-faq/
    // Subject 1.03: How do I find intersections of 2 2D line segments?
    //
    // Let A,B,C,D be 2-space position vectors.  Then the directed line
    // segments AB & CD are given by:
    //
    // AB=A+r(B-A), r in [0,1]
    // CD=C+s(D-C), s in [0,1]
    //
    // If AB & CD intersect, then
    //
    // A+r(B-A)=C+s(D-C), or  Ax+r(Bx-Ax)=Cx+s(Dx-Cx)
    // Ay+r(By-Ay)=Cy+s(Dy-Cy)  for some r,s in [0,1]
    //
    // Solving the above for r and s yields
    //
    //      (Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy)
    //  r = -----------------------------  (eqn 1)
    //      (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
    //
    //      (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
    //  s = -----------------------------  (eqn 2)
    //      (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
    //
    // Let P be the position vector of the intersection point, then
    //
    //  P=A+r(B-A) or Px=Ax+r(Bx-Ax) and Py=Ay+r(By-Ay)
    //
    // By examining the values of r & s, you can also determine some
    // other limiting conditions:
    //  If 0 <= r <= 1 && 0 <= s <= 1, intersection exists
    //  r < 0 or r > 1 or s < 0 or s > 1 line segments do not intersect
    //  If the denominator in eqn 1 is zero, AB & CD are parallel
    //  If the numerator in eqn 1 is also zero, AB & CD are collinear.
    //  If they are collinear, then the segments may be projected to the x-
    //  or y-axis, and overlap of the projected intervals checked.
    //
    // If the intersection Point of the 2 lines are needed (lines in this
    // context mean infinite lines) regardless whether the two line
    // segments intersect, then
    //  If r > 1, P is located on extension of AB
    //  If r < 0, P is located on extension of BA
    //  If s > 1, P is located on extension of CD
    //  If s < 0, P is located on extension of DC
    // Also note that the denominators of eqn 1 & 2 are identical.
    //
    // References:
    // [O'Rourke (C)] pp. 249-51
    // [Gems III] pp. 199-202 "Faster Line Segment Intersection,"
    //----------------------------------------------------------------------

    // Calculate the vectors.
    Vector AB = orgEnd - orgBegin;          // B - A
    Vector CA = orgBegin - hitBegin;        // A - C
    Vector CD = hitEnd - hitBegin;          // D - C
    double det = Vector::Determinant(AB, CD);

    if (DoubleUtil::IsZero(det))
    {
        // The segments are parallel. no intersection
        return NoIntersection;
    }

    double r = AdjustFIndex(Vector::Determinant(AB, CA) / det);

    if (r >= 0 && r <= 1)
    {
        // The line defined AB does cross the segment CD.
        double s = AdjustFIndex(Vector::Determinant(CD, CA) / det);
        if (s >= 0 && s <= 1)
        {
            // The crossing Point is on the segment AB as well.
            // Intersection found.
            return s;
        }
    }

    // No intersection found
    return NoIntersection;
}

/// <summary>
/// Clears double's computation fuzz around 0 and 1
/// </summary>
double SingleLoopLasso::AdjustFIndex(double findex)
{
    return DoubleUtil::IsZero(findex) ? 0 : (DoubleUtil::IsOne(findex) ? 1 : findex);
}

INKCANVAS_END_NAMESPACE
