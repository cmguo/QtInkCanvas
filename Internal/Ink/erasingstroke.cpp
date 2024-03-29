#include "Internal/Ink/erasingstroke.h"
#include "Internal/Ink/strokenodeiterator.h"
#include "Internal/Ink/strokenode.h"
#include "Windows/Ink/strokeintersection.h"


INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Constructor for incremental erasing
/// </summary>
/// <param name="erasingShape">The shape of the eraser's tip</param>
ErasingStroke::ErasingStroke(StylusShape& erasingShape)
    : _nodeIterator(erasingShape)
{
    ////System.Diagnostics.Debug.Assert(erasingShape != null);
    //_nodeIterator = new StrokeNodeIterator(erasingShape);
}

/// <summary>
/// Constructor for static (atomic) erasing
/// </summary>
/// <param name="erasingShape">The shape of the eraser's tip</param>
/// <param name="path">the spine of the erasing stroke</param>
ErasingStroke::ErasingStroke(StylusShape &erasingShape, List<Point> const & path)
    : ErasingStroke(erasingShape)
{
    MoveTo(path);
}


/// <summary>
/// Generates stroke nodes along a given path.
/// Drops any previously genererated nodes.
/// </summary>
/// <param name="path"></param>
void ErasingStroke::MoveTo(List<Point>const & path)
{
    ////System.Diagnostics.Debug.Assert((path != null) && (IEnumerablePointHelper.GetCount(path) != 0));
    Array<Point> points = path.ToArray();

    //if (_erasingStrokeNodes == null)
    //{
    //    _erasingStrokeNodes = new List<StrokeNode>(points.Length);
    //}
    //else
    {
        _erasingStrokeNodes.Clear();
    }


    _bounds = Rect();
    _nodeIterator = _nodeIterator.GetIteratorForNextSegment(points.Length() > 1 ? FilterPoints(points) : points);
    for (int i = 0; i < _nodeIterator.Count(); i++)
    {
        StrokeNode strokeNode = _nodeIterator[i];
        _bounds.Union(strokeNode.GetBoundsConnected());
        _erasingStrokeNodes.Add(strokeNode);
    }
#if POINTS_FILTER_TRACE
    _totalPointsAdded += path.Length;
    //System.Diagnostics.Debug.WriteLine(String.Format("Total Points added: {0} screened: {1} collinear screened: {2}", _totalPointsAdded, _totalPointsScreened, _collinearPointsScreened));
#endif

}

/// <summary>
/// Hit-testing for stroke erase scenario.
/// </summary>
/// <param name="iterator">the stroke nodes to iterate</param>
/// <returns>true if the strokes intersect, false otherwise</returns>
bool ErasingStroke::HitTest(StrokeNodeIterator const & iterator)
{
    ////System.Diagnostics.Debug.Assert(iterator != null);

    if (/*(_erasingStrokeNodes == null) || */(_erasingStrokeNodes.Count() == 0))
    {
        return false;
    }

    Rect inkSegmentBounds = Rect::Empty();
    for (int i = 0; i < iterator.Count(); i++)
    {
        StrokeNode inkStrokeNode = iterator[i];
        Rect inkNodeBounds = inkStrokeNode.GetBounds();
        inkSegmentBounds.Union(inkNodeBounds);

        if (inkSegmentBounds.IntersectsWith(_bounds))
        {
            //

            for (StrokeNode& erasingStrokeNode : _erasingStrokeNodes)
            {
                if (inkSegmentBounds.IntersectsWith(erasingStrokeNode.GetBoundsConnected())
                    && erasingStrokeNode.HitTest(inkStrokeNode))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/// <summary>
/// Hit-testing for point erase.
/// </summary>
/// <param name="iterator"></param>
/// <param name="intersections"></param>
/// <returns></returns>
bool ErasingStroke::EraseTest(StrokeNodeIterator const & iterator, List<StrokeIntersection> & intersections)
{
    //System.Diagnostics.Debug.Assert(iterator != null);
    //System.Diagnostics.Debug.Assert(intersections != null);
    intersections.Clear();

    List<StrokeFIndices> eraseAt;

    if (/*(_erasingStrokeNodes == null) || */(_erasingStrokeNodes.Count() == 0))
    {
        return false;
    }

    Rect inkSegmentBounds = Rect::Empty();
    for (int x = 0; x < iterator.Count(); x++)
    {
        StrokeNode inkStrokeNode = iterator[x];
        Rect inkNodeBounds = inkStrokeNode.GetBounds();
        inkSegmentBounds.Union(inkNodeBounds);

        //qDebug() << "EraseTest: node" << x << inkNodeBounds << inkSegmentBounds;
        if (inkSegmentBounds.IntersectsWith(_bounds))
        {
            //

            int index = eraseAt.Count();
            for (StrokeNode& erasingStrokeNode : _erasingStrokeNodes)
            {
                if (false == inkSegmentBounds.IntersectsWith(erasingStrokeNode.GetBoundsConnected()))
                {
                    continue;
                }

                //qDebug() << "EraseTest: intersects" << erasingStrokeNode.GetBoundsConnected();
                StrokeFIndices fragment = inkStrokeNode.CutTest(erasingStrokeNode);
                //qDebug() << "EraseTest: fragment" << fragment.ToString();
                if (fragment.IsEmpty())
                {
                    continue;
                }

                // Merge it with the other results for this ink segment
                bool inserted = false;
                for (int i = index; i < eraseAt.Count(); i++)
                {
                    StrokeFIndices &lastFragment = eraseAt[i];
                    if (fragment.BeginFIndex() < lastFragment.EndFIndex())
                    {
                        // If the fragments overlap, merge them
                        if (fragment.EndFIndex() > lastFragment.BeginFIndex())
                        {
                            fragment = StrokeFIndices(
                                Math::Min(lastFragment.BeginFIndex(), fragment.BeginFIndex()),
                                Math::Max(lastFragment.EndFIndex(), fragment.EndFIndex()));

                            //qDebug() << "EraseTest: merge with" << i << lastFragment.ToString() << "=>" << fragment.ToString();
                            // If the fragment doesn't go beyond lastFragment, break
                            if ((fragment.EndFIndex() <= lastFragment.EndFIndex()) || ((i + 1) == eraseAt.Count()))
                            {
                                inserted = true;
                                eraseAt[i] = fragment;
                                break;
                            }
                            else
                            {
                                eraseAt.RemoveAt(i);
                                i--;
                            }
                        }
                        // insert otherwise
                        else
                        {
                            //qDebug() << "EraseTest: insert" << i;
                            eraseAt.Insert(i, fragment);
                            inserted = true;
                            break;
                        }
                    }
                }

                // If not merged nor inserted, add it to the end of the list
                if (false == inserted)
                {
                    //qDebug() << "EraseTest: append";
                    eraseAt.Add(fragment);
                }
                // Break out if the entire ink segment is hit - {BeforeFirst, AfterLast}
                if (eraseAt[eraseAt.Count() - 1].IsFull())
                {
                    //qDebug() << "EraseTest: entire ink segment is hit";
                    break;
                }
            }
            // Merge inter-segment overlapping fragments
            if ((index > 0) && (index < eraseAt.Count()))
            {
                StrokeFIndices lastFragment = eraseAt[index - 1];
                if (DoubleUtil::AreClose(lastFragment.EndFIndex(), StrokeFIndices::AfterLast) )
                {
                    if (DoubleUtil::AreClose(eraseAt[index].BeginFIndex(), StrokeFIndices::BeforeFirst))
                    {
                        lastFragment.SetEndFIndex(eraseAt[index].EndFIndex());
                        eraseAt[index - 1] = lastFragment;
                        eraseAt.RemoveAt(index);
                    }
                    else
                    {
                        lastFragment.SetEndFIndex(inkStrokeNode.Index());
                        eraseAt[index - 1] = lastFragment;
                    }
                }
            }
        }
        // Start next ink segment
        inkSegmentBounds = inkNodeBounds;
    }
    if (eraseAt.Count() != 0)
    {
        for (StrokeFIndices& segment : eraseAt)
        {
            intersections.Add(StrokeIntersection(segment.BeginFIndex(), StrokeFIndices::AfterLast,
                                    StrokeFIndices::BeforeFirst, segment.EndFIndex()));
        }

    }
    return (eraseAt.Count() != 0);
}


Array<Point> ErasingStroke::FilterPoints(Array<Point> const & path)
{
    //System.Diagnostics.Debug.Assert(path.Length > 1);
    Point back2, back1;
    int i;
    List<Point> newPath;
    if (_nodeIterator.Count() == 0)
    {
        newPath.Add(path[0]);
        newPath.Add(path[1]);
        back2 = path[0];
        back1 = path[1];
        i = 2;
    }
    else
    {
        newPath.Add(path[0]);
        back2 = _nodeIterator[_nodeIterator.Count() - 1].Position();
        back1 = path[0];
        i = 1;
    }

    while (i < path.Length())
    {
        if (DoubleUtil::AreClose(back1, path[i]))
        {
            // Filter out duplicate points
            i++;
            continue;
        }

        Point begin = back2 - back1;
        Point end = path[i] - back1;
        //On a line defined by begin & end,  finds the findex of the point nearest to the origin (0,0).
        double findex = StrokeNodeOperations::GetProjectionFIndex(begin, end);

        if (DoubleUtil::IsBetweenZeroAndOne(findex))
        {
            Vector v = (begin + (end - begin) * findex);
            if (v.LengthSquared() < CollinearTolerance)
            {
                // The point back1 can be considered as on the line from back2 to the toTest StrokeNode.
                // Modify the previous point.
                newPath[newPath.Count() - 1] = path[i];
                back1 = path[i];
                i++;
#if POINTS_FILTER_TRACE
                _collinearPointsScreened ++;
#endif
                continue;
            }
        }

        // Add the surviving point into the list.
        newPath.Add(path[i]);
        back2 = back1;
        back1 = path[i];
        i++;
    }
#if POINTS_FILTER_TRACE
    _totalPointsScreened += path.Length - newPath.Count;
#endif
    return newPath.ToArray();
}

INKCANVAS_END_NAMESPACE
