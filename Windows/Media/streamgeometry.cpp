#include "Windows/Media/streamgeometry.h"
#include "qtstreamgeometrycontext.h"

#include <QPainter>

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
    path_.setFillRule(value == FillRule::EvenOdd ? Qt::OddEvenFill : Qt::WindingFill);
}

StreamGeometryContext &StreamGeometry::Open()
{
    if (context_ == nullptr)
        context_ = new QtStreamGeometryContext(this);
    return *context_;
}

void StreamGeometry::Close(QPainterPath &path)
{
    path_.swap(path);
}

QRectF StreamGeometry::Bounds()
{
    return path_.boundingRect();
}

void StreamGeometry::Draw(QPainter &painter)
{
    painter.drawPath(path_);
}

