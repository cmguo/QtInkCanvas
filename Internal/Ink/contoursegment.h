#ifndef CONTOURSEGMENT_H
#define CONTOURSEGMENT_H

#include "Internal/doubleutil.h"

#include <QPointF>

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class ContourSegment
{
public:
    /// <summary>
    /// Constructor for linear segments
    /// </summary>
    /// <param name="begin">segment's begin point</param>
    /// <param name="end">segment's end point</param>
    ContourSegment(QPointF const &begin, QPointF const &end)
    {
        _begin = begin;
        _vector = DoubleUtil::AreClose(begin, end) ? QPointF(0, 0) : (end - begin);
        _radius = QPointF(0, 0);
    }

    /// <summary>
    /// Constructor for arcs
    /// </summary>
    /// <param name="begin">arc's begin point</param>
    /// <param name="end">arc's end point</param>
    /// <param name="center">arc's center</param>
    ContourSegment(QPointF const &begin, QPointF const &end, QPointF const &center)
    {
        _begin = begin;
        _vector = end - begin;
        _radius = center - begin;
    }

    /// <summary> Tells whether the segment is arc or straight </summary>
    bool IsArc() const { return (_radius.x() != 0) || (_radius.y() != 0); }

    /// <summary> Returns the begin QPointF &of the segment </summary>
    QPointF const & Begin() const { return _begin; }

    /// <summary> Returns the end QPointF &of the segment </summary>
    QPointF End() const { return _begin + _vector; }

    /// <summary> Returns the QPointF from Begin to End </summary>
    QPointF const & Vector() const { return _vector; }

    /// <summary> Returns the QPointF from Begin to the center of the circle
    /// (zero QPointF for linear segments </summary>
    QPointF Radius() const { return _radius; }

private:
    QPointF  _begin;
    QPointF  _vector;
    QPointF  _radius;
};

INKCANVAS_END_NAMESPACE

#endif // CONTOURSEGMENT_H
