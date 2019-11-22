#ifndef GEOMETRY_H
#define GEOMETRY_H

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

#endif // GEOMETRY_H
