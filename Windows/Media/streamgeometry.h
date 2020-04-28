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

#include <QPainterPath>

class StreamGeometryContext;

// namespace System.Windows.Media

class StreamGeometry : public Geometry
{
public:
    StreamGeometry();

    virtual ~StreamGeometry() override;

    void SetFillRule(FillRule value);

    StreamGeometryContext &Open();

    void Close(QPainterPath & path);

    QPainterPath path() { return path_; }

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    StreamGeometryContext * context_ = nullptr;
    QPainterPath path_;
};

#endif // STREAMGEOMETRY_H
