#ifndef STREAMGEOMETRY_H
#define STREAMGEOMETRY_H

/// <summary>
///     FillRule -
/// </summary>
enum FillRule
{
    /// <summary>
    ///     EvenOdd -
    /// </summary>
    EvenOdd = 0,

    /// <summary>
    ///     Nonzero -
    /// </summary>
    Nonzero = 1,
};


#include "geometry.h"

class StreamGeometryContext;

class StreamGeometry : public Geometry
{
public:
    StreamGeometry();

    virtual ~StreamGeometry() override;

    void SetFillRule(FillRule value);

    StreamGeometryContext &Open();

private:
    StreamGeometryContext * context_;
};

#endif // STREAMGEOMETRY_H
