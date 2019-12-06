#ifndef STROKENODEDATA_H
#define STROKENODEDATA_H

#include "Internal/doubleutil.h"
#include "debug.h"

#include <QPointF>

// namespace MS.Internal.Ink

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
    StrokeNodeData(QPointF const & position)
    {
        _position = position;
        _pressure = 1;
    }

    /// <summary>
    /// Constructor for nodes with pressure data
    /// </summary>
    /// <param name="position">position of the node</param>
    /// <param name="pressure">pressure scaling factor at the node</param>
    StrokeNodeData(QPointF const & position, double pressure)
    {
        Debug::Assert(DoubleUtil::GreaterThan(pressure, 0));

        _position = position;
        _pressure = pressure;
    }

    /// <summary> Tells whether the structre was properly initialized </summary>
    bool IsEmpty() const
    {
        Debug::Assert(DoubleUtil::AreClose(0, s_empty._pressure));
        return DoubleUtil::AreClose(_pressure, s_empty._pressure);
    }

    /// <summary> Position of the node </summary>
    QPointF const & Position() const
    {
        return _position;
    }

    /// <summary> Pressure scaling factor at the node </summary>
    double PressureFactor() const { return _pressure; }

private:
    QPointF   _position;
    double _pressure = 0.0;
};

#endif // STROKENODEDATA_H
