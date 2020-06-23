#ifndef LASSO_H
#define LASSO_H

#include "strokefindices.h"
#include "Windows/Ink/strokeintersection.h"
#include "strokenode.h"

#include <QRectF>
#include <QPointF>
#include <QVector>
#include <QList>

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
    QRectF & Bounds()
    {
        return _bounds;
    }
    void SetBounds(QRectF const & value)
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
        return (_points.size() < 3);
    }

    /// <summary>
    /// Returns the count of points in the lasso
    /// </summary>
    int PointCount()
    {
        //System.Diagnostics.Debug.Assert(_points != null);
        return _points.size();
    }

    /// <summary>
    /// Index-based read-only accessor to lasso points
    /// </summary>
    /// <param name="index">index of the QPointF to return</param>
    /// <returns>a QPointF in the lasso</returns>
    QPointF operator[](int index)
    {
        //System.Diagnostics.Debug.Assert(_points != null);
        //System.Diagnostics.Debug.Assert((0 <= index) && (index < _points.size()));

        return _points[index];
    }

    /// <summary>
    /// Extends the lasso by appending more points
    /// </summary>
    /// <param name="points">new points</param>
    void AddPoints(QVector<QPointF> const & points);

    /// <summary>
    /// Appends a QPointF to the lasso
    /// </summary>
    /// <param name="point">new lasso point</param>
    void AddPoint(QPointF const & point);

    /// <summary>
    /// This method implement the core algorithm to check whether a QPointF is within a polygon
    /// that are formed by the lasso points.
    /// </summary>
    /// <param name="point"></param>
    /// <returns>true if the QPointF is contained within the lasso; false otherwise </returns>
    bool Contains(QPointF const & point);

    QVector<StrokeIntersection> HitTest(StrokeNodeIterator & iterator);

    /// <summary>
    /// Sort and merge the crossing list
    /// </summary>
    /// <param name="crossingList">The crossing list to sort/merge</param>
    static void SortAndMerge(QList<LassoCrossing> & crossingList);


    /// <summary>
    /// Helper function to find out whether a QPointF is inside the lasso
    /// </summary>
    bool SegmentWithinLasso(StrokeNode & strokeNode, double fIndex);

    /// <summary>
    /// Helper function to find out the hit test result
    /// </summary>
    void ProduceHitTestResults(
                            QList<LassoCrossing> & crossingList, QList<StrokeIntersection> & strokeIntersections);

    /// <summary>
    /// This flag is set to true when a lasso QPointF has been modified or removed
    /// from the list, which will invalidate incremental lasso hitteting
    /// </summary>
    bool IsIncrementalLassoDirty();
    void SetIsIncrementalLassoDirty(bool value);

    /// <summary>
    /// Get a reference to the lasso points store
    /// </summary>
    QList<QPointF> & PointsList();

    /// <summary>
    /// Filter out duplicate points (and maybe in the futuer colinear points).
    /// Return true if the QPointF should be filtered
    /// </summary>
protected:
    virtual bool Filter(QPointF const & point);

    /// <summary>
    /// Implemtnation of add point
    /// </summary>
    /// <param name="point"></param>
    virtual void AddPointImpl(QPointF const & point);

private:
    QList<QPointF>          _points;
    QRectF                  _bounds;
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

        /// <summary>
        /// ToString
        /// </summary>
        QString ToString()
        {
            return FIndices.ToString();
        }

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
    /// Return true if the QPointF will be filtered out and should NOT be added to the list
    /// </summary>
protected:
    virtual bool Filter(QPointF const & point);

    virtual void AddPointImpl(QPointF const & point);

    /// <summary>
    /// If the line _points[Count -1]->point insersect with the existing lasso, return true
    /// and bIndex value is set to a doulbe value representing position of the intersection.
    /// </summary>
    bool GetIntersectionWithExistingLasso(QPointF const & point, double & bIndex);


    /// <summary>
    /// Finds the intersection between the segment [hitBegin, hitEnd] and the segment [orgBegin, orgEnd].
    /// </summary>
    static double FindIntersection(QPointF const & hitBegin, QPointF const & hitEnd,
                                   QPointF const & orgBegin, QPointF const & orgEnd);

    /// <summary>
    /// Clears double's computation fuzz around 0 and 1
    /// </summary>
    static double AdjustFIndex(double findex);

private:
    bool _hasLoop                           = false;
    QRectF _prevBounds;
    static constexpr double NoIntersection  = StrokeFIndices::BeforeFirst;
};

INKCANVAS_END_NAMESPACE

#endif // LASSO_H
