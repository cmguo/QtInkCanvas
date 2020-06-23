#include "Internal/Ink/erasingstroke.h"
#include "Internal/Ink/strokenodeiterator.h"
#include "Internal/Ink/strokenode.h"
#include "Windows/Ink/strokeintersection.h"

#include <QDebug>

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
ErasingStroke::ErasingStroke(StylusShape &erasingShape, QVector<QPointF> const & path)
    : ErasingStroke(erasingShape)
{
    MoveTo(path);
}


/// <summary>
/// Generates stroke nodes along a given path.
/// Drops any previously genererated nodes.
/// </summary>
/// <param name="path"></param>
void ErasingStroke::MoveTo(QVector<QPointF>const & path)
{
    ////System.Diagnostics.Debug.Assert((path != null) && (IEnumerablePointHelper.GetCount(path) != 0));
    QVector<QPointF> points = path;

    //if (_erasingStrokeNodes == null)
    //{
    //    _erasingStrokeNodes = new List<StrokeNode>(points.Length);
    //}
    //else
    {
        _erasingStrokeNodes.clear();
    }


    _bounds = QRectF();
    _nodeIterator = _nodeIterator.GetIteratorForNextSegment(points.size() > 1 ? FilterPoints(points) : points);
    for (int i = 0; i < _nodeIterator.Count(); i++)
    {
        StrokeNode strokeNode = _nodeIterator[i];
        _bounds |= strokeNode.GetBoundsConnected();
        _erasingStrokeNodes.append(strokeNode);
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

    if (/*(_erasingStrokeNodes == null) || */(_erasingStrokeNodes.size() == 0))
    {
        return false;
    }

    QRectF inkSegmentBounds;
    for (int i = 0; i < iterator.Count(); i++)
    {
        StrokeNode inkStrokeNode = iterator[i];
        QRectF inkNodeBounds = inkStrokeNode.GetBounds();
        inkSegmentBounds |= inkNodeBounds;

        if (inkSegmentBounds.intersects(_bounds))
        {
            //

            for (StrokeNode& erasingStrokeNode : _erasingStrokeNodes)
            {
                if (inkSegmentBounds.intersects(erasingStrokeNode.GetBoundsConnected())
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
bool ErasingStroke::EraseTest(StrokeNodeIterator const & iterator, QVector<StrokeIntersection> & intersections)
{
    //System.Diagnostics.Debug.Assert(iterator != null);
    //System.Diagnostics.Debug.Assert(intersections != null);
    intersections.clear();

    QVector<StrokeFIndices> eraseAt;

    if (/*(_erasingStrokeNodes == null) || */(_erasingStrokeNodes.size() == 0))
    {
        return false;
    }

    QRectF inkSegmentBounds;
    for (int x = 0; x < iterator.Count(); x++)
    {
        StrokeNode inkStrokeNode = iterator[x];
        QRectF inkNodeBounds = inkStrokeNode.GetBounds();
        inkSegmentBounds |= inkNodeBounds;

        //qDebug() << "EraseTest: node" << x << inkNodeBounds << inkSegmentBounds;
        if (inkSegmentBounds.intersects(_bounds))
        {
            //

            int index = eraseAt.size();
            for (StrokeNode& erasingStrokeNode : _erasingStrokeNodes)
            {
                if (false == inkSegmentBounds.intersects(erasingStrokeNode.GetBoundsConnected()))
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
                for (int i = index; i < eraseAt.size(); i++)
                {
                    StrokeFIndices &lastFragment = eraseAt[i];
                    if (fragment.BeginFIndex() < lastFragment.EndFIndex())
                    {
                        // If the fragments overlap, merge them
                        if (fragment.EndFIndex() > lastFragment.BeginFIndex())
                        {
                            fragment = StrokeFIndices(
                                qMin(lastFragment.BeginFIndex(), fragment.BeginFIndex()),
                                qMax(lastFragment.EndFIndex(), fragment.EndFIndex()));

                            //qDebug() << "EraseTest: merge with" << i << lastFragment.ToString() << "=>" << fragment.ToString();
                            // If the fragment doesn't go beyond lastFragment, break
                            if ((fragment.EndFIndex() <= lastFragment.EndFIndex()) || ((i + 1) == eraseAt.size()))
                            {
                                inserted = true;
                                eraseAt[i] = fragment;
                                break;
                            }
                            else
                            {
                                eraseAt.removeAt(i);
                                i--;
                            }
                        }
                        // insert otherwise
                        else
                        {
                            //qDebug() << "EraseTest: insert" << i;
                            eraseAt.insert(i, fragment);
                            inserted = true;
                            break;
                        }
                    }
                }

                // If not merged nor inserted, add it to the end of the list
                if (false == inserted)
                {
                    //qDebug() << "EraseTest: append";
                    eraseAt.append(fragment);
                }
                // Break out if the entire ink segment is hit - {BeforeFirst, AfterLast}
                if (eraseAt[eraseAt.size() - 1].IsFull())
                {
                    //qDebug() << "EraseTest: entire ink segment is hit";
                    break;
                }
            }
            // Merge inter-segment overlapping fragments
            if ((index > 0) && (index < eraseAt.size()))
            {
                StrokeFIndices lastFragment = eraseAt[index - 1];
                if (DoubleUtil::AreClose(lastFragment.EndFIndex(), StrokeFIndices::AfterLast) )
                {
                    if (DoubleUtil::AreClose(eraseAt[index].BeginFIndex(), StrokeFIndices::BeforeFirst))
                    {
                        lastFragment.SetEndFIndex(eraseAt[index].EndFIndex());
                        eraseAt[index - 1] = lastFragment;
                        eraseAt.removeAt(index);
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
    if (eraseAt.size() != 0)
    {
        for (StrokeFIndices& segment : eraseAt)
        {
            intersections.append(StrokeIntersection(segment.BeginFIndex(), StrokeFIndices::AfterLast,
                                    StrokeFIndices::BeforeFirst, segment.EndFIndex()));
        }

    }
    return (eraseAt.size() != 0);
}


QVector<QPointF> ErasingStroke::FilterPoints(QVector<QPointF> const & path)
{
    //System.Diagnostics.Debug.Assert(path.Length > 1);
    QPointF back2, back1;
    int i;
    QList<QPointF> newPath;
    if (_nodeIterator.Count() == 0)
    {
        newPath.append(path[0]);
        newPath.append(path[1]);
        back2 = path[0];
        back1 = path[1];
        i = 2;
    }
    else
    {
        newPath.append(path[0]);
        back2 = _nodeIterator[_nodeIterator.Count() - 1].Position();
        back1 = path[0];
        i = 1;
    }

    while (i < path.size())
    {
        if (DoubleUtil::AreClose(back1, path[i]))
        {
            // Filter out duplicate points
            i++;
            continue;
        }

        QPointF begin = back2 - back1;
        QPointF end = path[i] - back1;
        //On a line defined by begin & end,  finds the findex of the point nearest to the origin (0,0).
        double findex = StrokeNodeOperations::GetProjectionFIndex(begin, end);

        if (DoubleUtil::IsBetweenZeroAndOne(findex))
        {
            QPointF v = (begin + (end - begin) * findex);
            if (LengthSquared(v) < CollinearTolerance)
            {
                // The point back1 can be considered as on the line from back2 to the toTest StrokeNode.
                // Modify the previous point.
                newPath[newPath.size() - 1] = path[i];
                back1 = path[i];
                i++;
#if POINTS_FILTER_TRACE
                _collinearPointsScreened ++;
#endif
                continue;
            }
        }

        // Add the surviving point into the list.
        newPath.append(path[i]);
        back2 = back1;
        back1 = path[i];
        i++;
    }
#if POINTS_FILTER_TRACE
    _totalPointsScreened += path.Length - newPath.Count;
#endif
    return newPath.toVector();
}

INKCANVAS_END_NAMESPACE
