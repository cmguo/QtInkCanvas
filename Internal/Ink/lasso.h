#ifndef LASSO_H
#define LASSO_H

#include "strokefindices.h"
#include "Windows/Ink/strokeintersection.h"
#include "Windows/rect.h"
#include "Collections/Generic/list.h"
#include "strokenode.h"

INKCANVAS_BEGIN_NAMESPACE

class StrokeNodeIterator;

// namespace MS.Internal.Ink

class Lasso
{
public:
    /// <summary>
    /// Default c-tor. Used in incremental hit-testing.
    /// </summary>
    Lasso()
    {
    }

    virtual ~Lasso()
    {
    }

    struct LassoCrossing;

public:
    /// <summary>
    /// Returns the bounds of the lasso
    /// </summary>
    Rect & Bounds()
    {
        return _bounds;
    }
    void SetBounds(Rect const & value)
    {
        _bounds = value;
    }

    /// <summary>
    /// Tells whether the lasso captures any area
    /// </summary>
    bool IsEmpty()
    {
        //System.Diagnostics.Debug.Assert(_points != null);
        // The value is based on the assumption that the lasso is normalized
        // i.e. it has no duplicate points or collinear sibling segments.
        return (_points.Count() < 3);
    }

    /// <summary>
    /// Returns the count of points in the lasso
    /// </summary>
    int PointCount()
    {
        //System.Diagnostics.Debug.Assert(_points != null);
        return _points.Count();
    }

    /// <summary>
    /// Index-based read-only accessor to lasso points
    /// </summary>
    /// <param name="index">index of the Point to return</param>
    /// <returns>a Point in the lasso</returns>
    Point operator[](int index)
    {
        //System.Diagnostics.Debug.Assert(_points != null);
        //System.Diagnostics.Debug.Assert((0 <= index) && (index < _points.Count()));

        return _points[index];
    }

    /// <summary>
    /// Extends the lasso by appending more points
    /// </summary>
    /// <param name="points">new points</param>
    void AddPoints(List<Point> const & points);

    /// <summary>
    /// Appends a Point to the lasso
    /// </summary>
    /// <param name="point">new lasso point</param>
    void AddPoint(Point const & point);

    /// <summary>
    /// This method implement the core algorithm to check whether a Point is within a polygon
    /// that are formed by the lasso points.
    /// </summary>
    /// <param name="point"></param>
    /// <returns>true if the Point is contained within the lasso; false otherwise </returns>
    bool Contains(Point const & point);

    Array<StrokeIntersection> HitTest(StrokeNodeIterator & iterator);

    /// <summary>
    /// Sort and merge the crossing list
    /// </summary>
    /// <param name="crossingList">The crossing list to sort/merge</param>
    static void SortAndMerge(List<LassoCrossing> & crossingList);


    /// <summary>
    /// Helper function to find out whether a Point is inside the lasso
    /// </summary>
    bool SegmentWithinLasso(StrokeNode & strokeNode, double fIndex);

    /// <summary>
    /// Helper function to find out the hit test result
    /// </summary>
    void ProduceHitTestResults(
                            List<LassoCrossing> & crossingList, List<StrokeIntersection> & strokeIntersections);

    /// <summary>
    /// This flag is set to true when a lasso Point has been modified or removed
    /// from the list, which will invalidate incremental lasso hitteting
    /// </summary>
    bool IsIncrementalLassoDirty();
    void SetIsIncrementalLassoDirty(bool value);

    /// <summary>
    /// Get a reference to the lasso points store
    /// </summary>
    List<Point> & PointsList();

    /// <summary>
    /// Filter out duplicate points (and maybe in the futuer colinear points).
    /// Return true if the Point should be filtered
    /// </summary>
protected:
    virtual bool Filter(Point const & point);

    /// <summary>
    /// Implemtnation of add point
    /// </summary>
    /// <param name="point"></param>
    virtual void AddPointImpl(Point const & point);

private:
    List<Point>             _points;
    Rect                    _bounds;
    bool                    _incrementalLassoDirty  = false;
    static constexpr double MinDistance             = 1.0;

public:
    /// <summary>
    /// Simple helper struct used to track where the lasso crosses a stroke
    /// we should consider making this a class if generics perf is bad for structs
    /// </summary>
    struct LassoCrossing
    {
        StrokeFIndices FIndices;
        StrokeNode StartNode;
        StrokeNode EndNode;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="newFIndices"></param>
        /// <param name="strokeNode"></param>
    public:
        LassoCrossing()
        {
        }

        LassoCrossing(StrokeFIndices newFIndices, StrokeNode const & strokeNode)
            : FIndices(newFIndices)
            , StartNode(strokeNode)
            , EndNode(strokeNode)
        {
            //System.Diagnostics.Debug.Assert(!newFIndices.IsEmpty);
            //System.Diagnostics.Debug.Assert(strokeNode.IsValid);
        }

#ifdef INKCANVAS_QT
        /// <summary>
        /// ToString
        /// </summary>
        QString ToString()
        {
            return FIndices.ToString();
        }
#endif
        /// <summary>
        /// Construct an empty LassoCrossing
        /// </summary>
        static LassoCrossing EmptyCrossing()
        {
            LassoCrossing crossing;
            crossing.FIndices = StrokeFIndices::Empty();
            return crossing;
        }

        /// <summary>
        /// Return true if this crossing is an empty one; false otherwise
        /// </summary>
        bool IsEmpty() const
        {
            return FIndices.IsEmpty();
        }

        friend bool operator<(LassoCrossing const & l, LassoCrossing const & r)
        {
            return l.CompareTo(r) < 0;
        }

        /// <summary>
        /// Implement the interface used for comparison
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        int CompareTo(LassoCrossing const & crossing) const;

        /// <summary>
        /// Merge two crossings into one.
        /// </summary>
        /// <param name="crossing"></param>
        /// <returns>Return true if these two crossings are actually overlapping and merged; false otherwise</returns>
        bool Merge(LassoCrossing const & crossing);
    };
};



/// <summary>
/// Implement a special lasso that considers only the first loop
/// </summary>
class SingleLoopLasso : public Lasso
{
public:
    /// <summary>
    /// Default constructor
    /// </summary>
    SingleLoopLasso() : Lasso(){}

    virtual ~SingleLoopLasso() {}

    /// <summary>
    /// Return true if the Point will be filtered out and should NOT be added to the list
    /// </summary>
protected:
    virtual bool Filter(Point const & point);

    virtual void AddPointImpl(Point const & point);

    /// <summary>
    /// If the line _points[Count -1]->point insersect with the existing lasso, return true
    /// and bIndex value is set to a doulbe value representing position of the intersection.
    /// </summary>
    bool GetIntersectionWithExistingLasso(Point const & point, double & bIndex);


    /// <summary>
    /// Finds the intersection between the segment [hitBegin, hitEnd] and the segment [orgBegin, orgEnd].
    /// </summary>
    static double FindIntersection(Point const & hitBegin, Point const & hitEnd,
                                   Point const & orgBegin, Point const & orgEnd);

    /// <summary>
    /// Clears double's computation fuzz around 0 and 1
    /// </summary>
    static double AdjustFIndex(double findex);

private:
    bool _hasLoop                           = false;
    Rect _prevBounds;
    static constexpr double NoIntersection  = StrokeFIndices::BeforeFirst;
};

INKCANVAS_END_NAMESPACE

#endif // LASSO_H
