#include "Windows/Media/streamgeometry.h"

#ifdef INKCANVAS_QT
#include "Landing/Qt/qtstreamgeometrycontext.h"
#include <QPainter>
#endif

#ifdef INKCANVAS_ANDROID
#include "Landing/Jni/androidstreamgeometrycontext.h"
#endif

INKCANVAS_BEGIN_NAMESPACE

StreamGeometry::StreamGeometry()
    : context_(nullptr)
{
}

StreamGeometry::~StreamGeometry()
{
    if (context_)
        delete context_;
}

void StreamGeometry::SetFillRule(FillRule value)
{
#ifdef INKCANVAS_QT
    path_.setFillRule(value == FillRule::EvenOdd ? Qt::OddEvenFill : Qt::WindingFill);
#endif
}

StreamGeometryContext &StreamGeometry::Open()
{
#ifdef INKCANVAS_QT
    if (context_ == nullptr)
        context_ = new QtStreamGeometryContext(this);
#endif
#ifdef INKCANVAS_ANDROID
    if (context_ == nullptr)
        context_ = new AndroidStreamGeometryContext(this);
#endif
    return *context_;
}

#ifdef INKCANVAS_QT

void StreamGeometry::Close(QPainterPath &path)
{
    path_.swap(path);
}

#endif

#ifdef INKCANVAS_ANDROID

void StreamGeometry::Close(void * path)
{
    path_ = path;
}

#endif

Rect StreamGeometry::Bounds()
{
#ifdef INKCANVAS_QT
    return path_.boundingRect();
#else
    return Rect();
#endif
}

#ifdef INKCANVAS_QT_DRAW
void StreamGeometry::Draw(QPainter &painter)
{
    painter.drawPath(path_);
}
#endif

INKCANVAS_END_NAMESPACE
