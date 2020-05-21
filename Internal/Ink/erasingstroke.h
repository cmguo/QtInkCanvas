#ifndef ERASINGSTROKE_H
#define ERASINGSTROKE_H

#include "Internal/Ink/strokenodeiterator.h"
#include "Internal/Ink/strokenode.h"

class StylusShape;

// namespace MS.Internal.Ink

/// <summary>
/// This class represents a contour of an erasing stroke, and provides
/// internal API for static and incremental stroke_contour vs stroke_contour
/// hit-testing.
/// </summary>
class ErasingStroke
{
public:


    /// <summary>
    /// Constructor for incremental erasing
    /// </summary>
    /// <param name="erasingShape">The shape of the eraser's tip</param>
    ErasingStroke(StylusShape& erasingShape, const QPolygonF &clipShape);

    /// <summary>
    /// Constructor for static (atomic) erasing
    /// </summary>
    /// <param name="erasingShape">The shape of the eraser's tip</param>
    /// <param name="path">the spine of the erasing stroke</param>
    ErasingStroke(StylusShape &erasingShape, QVector<QPointF> const & path, const QPolygonF &clipShape);

    /// <summary>
    /// Generates stroke nodes along a given path.
    /// Drops any previously genererated nodes.
    /// </summary>
    /// <param name="path"></param>
    void MoveTo(QVector<QPointF>const & path);

    /// <summary>
    /// Returns the bounds of the eraser's last move.
    /// </summary>
    /// <value></value>
    QRectF Bounds() { return _bounds; }

    /// <summary>
    /// Hit-testing for stroke erase scenario.
    /// </summary>
    /// <param name="iterator">the stroke nodes to iterate</param>
    /// <returns>true if the strokes intersect, false otherwise</returns>
    bool HitTest(StrokeNodeIterator const & iterator);

    /// <summary>
    /// Hit-testing for point erase.
    /// </summary>
    /// <param name="iterator"></param>
    /// <param name="intersections"></param>
    /// <returns></returns>
    bool EraseTest(StrokeNodeIterator const & iterator, QList<StrokeIntersection>& intersections);


private:
    QVector<QPointF> FilterPoints(QVector<QPointF> const & path);

private:
    StrokeNodeIterator      _nodeIterator;
    QList<StrokeNode>       _erasingStrokeNodes;
    QRectF                  _bounds;
    std::unique_ptr<ErasingStroke> _clipStroke;

#if POINTS_FILTER_TRACE
    int                     _totalPointsAdded = 0;
    int                     _totalPointsScreened = 0;
    int                     _collinearPointsScreened = 0;
#endif

    // The collinear tolerance used in points filtering algorithm. The valie
    // should be further tuned considering trade-off of performance and accuracy.
    // In general, the larger the value, more points are filtered but less accurate.
    // For a value of 0.5, typically 70% - 80% percent of the points are filtered out.
private:
    static constexpr double CollinearTolerance = 0.1;

};

#endif // ERASINGSTROKE_H
