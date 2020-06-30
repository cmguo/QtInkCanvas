#include "Windows/Media/streamgeometry.h"

#ifdef INKCANVAS_QT
#include "Landing/Qt/qtstreamgeometrycontext.h"
#include <QPainter>
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
#ifndef INKCANVAS_CORE
    path_.setFillRule(value == FillRule::EvenOdd ? Qt::OddEvenFill : Qt::WindingFill);
#endif
}

StreamGeometryContext &StreamGeometry::Open()
{
#ifndef INKCANVAS_CORE
    if (context_ == nullptr)
        context_ = new QtStreamGeometryContext(this);
#endif
    return *context_;
}

#ifndef INKCANVAS_CORE

void StreamGeometry::Close(QPainterPath &path)
{
    path_.swap(path);
}

#endif

Rect StreamGeometry::Bounds()
{
#ifndef INKCANVAS_CORE
    return path_.boundingRect();
#else
    return Rect();
#endif
}

#ifndef INKCANVAS_CORE

void StreamGeometry::Draw(QPainter &painter)
{
    painter.drawPath(path_);
}

#endif

INKCANVAS_END_NAMESPACE
