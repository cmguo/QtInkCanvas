#include "Windows/Media/streamgeometry.h"

#ifdef INKCANVAS_QT
#include "Landing/Qt/qtstreamgeometrycontext.h"
#include <QPainter>
#endif

#ifdef INKCANVAS_ANDROID
#include "Landing/Android/androidstreamgeometrycontext.h"
#endif

#ifdef INKCANVAS_IOS
#include "Landing/Ios/iosstreamgeometrycontext.h"
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
    reinterpret_cast<QPainterPath*>(path_)->setFillRule(value == FillRule::EvenOdd ? Qt::OddEvenFill : Qt::WindingFill);
#else
    (void) value;
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
#ifdef INKCANVAS_IOS
    if (context_ == nullptr)
        context_ = new IosStreamGeometryContext(this);
#endif
    return *context_;
}

void StreamGeometry::Close(void * path)
{
    path_ = path;
}

Rect StreamGeometry::Bounds()
{
#ifdef INKCANVAS_QT
    return reinterpret_cast<QPainterPath*>(path_)->boundingRect();
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
