#ifndef CONTOURSEGMENT_H
#define CONTOURSEGMENT_H

#include "Internal/doubleutil.h"
#include "Windows/point.h"

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
    ContourSegment(Point const &begin, Point const &end)
    {
        _begin = begin;
        _vector = DoubleUtil::AreClose(begin, end) ? Vector(0, 0) : (end - begin);
        _radius = Point(0, 0);
    }

    /// <summary>
    /// Constructor for arcs
    /// </summary>
    /// <param name="begin">arc's begin point</param>
    /// <param name="end">arc's end point</param>
    /// <param name="center">arc's center</param>
    ContourSegment(Point const &begin, Point const &end, Point const &center)
    {
        _begin = begin;
        _vector = end - begin;
        _radius = center - begin;
    }

    /// <summary> Tells whether the segment is arc or straight </summary>
    bool IsArc() const { return (_radius.X() != 0) || (_radius.Y() != 0); }

    /// <summary> Returns the begin Point &of the segment </summary>
    Point const & Begin() const { return _begin; }

    /// <summary> Returns the end Point &of the segment </summary>
    Point End() const { return _begin + _vector; }

    /// <summary> Returns the Point from Begin to End </summary>
    Vector const & GetVector() const { return _vector; }

    /// <summary> Returns the Point from Begin to the center of the circle
    /// (zero Point for linear segments </summary>
    Vector Radius() const { return _radius; }

private:
    Point  _begin;
    Vector  _vector;
    Vector  _radius;
};

INKCANVAS_END_NAMESPACE

#endif // CONTOURSEGMENT_H
