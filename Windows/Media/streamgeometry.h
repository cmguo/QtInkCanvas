#ifndef STREAMGEOMETRY_H
#define STREAMGEOMETRY_H

#include "InkCanvas_global.h"
#include "geometry.h"

#ifndef INKCANVAS_CORE
#include <QPainterPath>
#endif

INKCANVAS_BEGIN_NAMESPACE

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


class StreamGeometryContext;

// namespace System.Windows.Media

class StreamGeometry : public Geometry
{
public:
    StreamGeometry();

    virtual ~StreamGeometry() override;

    void SetFillRule(FillRule value);

    StreamGeometryContext &Open();

#ifndef INKCANVAS_CORE
    void Close(QPainterPath & path);

    QPainterPath path() { return path_; }
#endif

    virtual Rect Bounds() override;

#ifndef INKCANVAS_CORE
    virtual void Draw(QPainter& painter) override;
#endif

private:
    StreamGeometryContext * context_ = nullptr;
#ifndef INKCANVAS_CORE
    QPainterPath path_;
#endif
};

INKCANVAS_END_NAMESPACE

#endif // STREAMGEOMETRY_H
