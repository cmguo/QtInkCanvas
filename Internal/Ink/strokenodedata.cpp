#include "Internal/Ink/strokenodedata.h"
#include "Internal/debug.h"

StrokeNodeData StrokeNodeData::s_empty;

/// <summary>
/// Constructor for nodes of a pressure insensitive stroke
/// </summary>
/// <param name="position">position of the node</param>
StrokeNodeData::StrokeNodeData(QPointF const & position)
{
    _position = position;
    _pressure = 1;
}

/// <summary>
/// Constructor for nodes with pressure data
/// </summary>
/// <param name="position">position of the node</param>
/// <param name="pressure">pressure scaling factor at the node</param>
StrokeNodeData::StrokeNodeData(QPointF const & position, double pressure)
{
    Debug::Assert(DoubleUtil::GreaterThan(pressure, 0));

    _position = position;
    _pressure = pressure;
}

/// <summary> Tells whether the structre was properly initialized </summary>
bool StrokeNodeData::IsEmpty() const
{
    Debug::Assert(DoubleUtil::AreClose(0, s_empty._pressure));
    return DoubleUtil::AreClose(_pressure, s_empty._pressure);
}
