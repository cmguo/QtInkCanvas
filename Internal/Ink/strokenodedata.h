#ifndef STROKENODEDATA_H
#define STROKENODEDATA_H

#include "Internal/doubleutil.h"
#include "Windows/point.h"

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// This structure represents a node on a stroke spine.
/// </summary>
class StrokeNodeData
{
private:
    static StrokeNodeData s_empty;

public:
    /// <summary> Returns static object representing an unitialized node </summary>
    static StrokeNodeData Empty() { return s_empty; }

    StrokeNodeData()
    {
    }

    /// <summary>
    /// Constructor for nodes of a pressure insensitive stroke
    /// </summary>
    /// <param name="position">position of the node</param>
    StrokeNodeData(Point const & position);

    /// <summary>
    /// Constructor for nodes with pressure data
    /// </summary>
    /// <param name="position">position of the node</param>
    /// <param name="pressure">pressure scaling factor at the node</param>
    StrokeNodeData(Point const & position, float pressure);

    /// <summary> Tells whether the structre was properly initialized </summary>
    bool IsEmpty() const;

    /// <summary> Position of the node </summary>
    Point const & Position() const
    {
        return _position;
    }

    /// <summary> Pressure scaling factor at the node </summary>
    float PressureFactor() const { return _pressure; }

private:
    Point   _position;
    float _pressure = 0.0;
};

INKCANVAS_END_NAMESPACE

#endif // STROKENODEDATA_H
