#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QRectF>

enum SweepDirection
{
    /// <summary>
    ///     Counterclockwise -
    /// </summary>
    Counterclockwise = 0,

    /// <summary>
    ///     Clockwise -
    /// </summary>
    Clockwise = 1,
};

class Geometry
{
public:
    Geometry();

    virtual ~Geometry();
};

class PathGeometry : public Geometry
{
public:
    Geometry * GetOutlinedPathGeometry();
};

class RectangleGeometry : public Geometry
{
public:
    RectangleGeometry(QRectF);
};

class GeometryGroup : public Geometry
{

};

#endif // GEOMETRY_H
