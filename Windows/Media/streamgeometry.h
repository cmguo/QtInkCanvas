#ifndef STREAMGEOMETRY_H
#define STREAMGEOMETRY_H

#include "InkCanvas_global.h"
#include "Windows/Media/geometry.h"

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

#ifdef INKCANVAS_QT
    void Close(QPainterPath & path);

    QPainterPath path() { return path_; }
#endif

#ifdef INKCANVAS_ANDROID
    void Close(void * path);

    void * path() { return path_; }
#endif

    virtual Rect Bounds() override;

#ifdef INKCANVAS_QT_DRAW
    virtual void Draw(QPainter& painter) override;
#endif

private:
    StreamGeometryContext * context_ = nullptr;
#ifdef INKCANVAS_QT
    QPainterPath path_;
#endif
#ifdef INKCANVAS_ANDROID
    void * path_ = nullptr;
#endif
};

INKCANVAS_END_NAMESPACE

#endif // STREAMGEOMETRY_H
