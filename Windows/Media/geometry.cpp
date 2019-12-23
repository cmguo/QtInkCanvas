#include "Windows/Media/geometry.h"

#include <QPainter>

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

bool Geometry::tryTakeOwn(void *owner)
{
    if (owner_ == nullptr) {
        owner_ = owner;
        return true;
    }
    return owner_ == owner;
}

PathGeometry::PathGeometry()
{
}

Geometry * PathGeometry::GetOutlinedPathGeometry()
{
    return nullptr;
}

void PathGeometry::Add(QPainterPath& path)
{
    path_.addPath(path);
}

PathGeometry* PathGeometry::Combine(Geometry * geometry)
{
    return this;
}

QRectF PathGeometry::Bounds()
{
    return path_.boundingRect();
}

void PathGeometry::Draw(QPainter &painter)
{
    painter.drawPath(path_);
}

GeometryGroup::~GeometryGroup()
{
    for (Geometry* g : children_)
        delete g;
    children_.clear();
}

QList<Geometry*>& GeometryGroup::Children()
{
    return children_;
}

QRectF GeometryGroup::Bounds()
{
    QRectF bounds;
    for (Geometry* g : children_)
        bounds |= g->Bounds();
    return bounds;
}

void GeometryGroup::Draw(QPainter &painter)
{
    for (Geometry* g : children_)
        g->Draw(painter);
}

LineGeometry::LineGeometry(QPointF point0, QPointF point1)
    : line_(point0, point1)
{
}

QRectF LineGeometry::Bounds()
{
    return QRectF(line_.p1(), line_.p2());
}

void LineGeometry::Draw(QPainter &painter)
{
    painter.drawLine(line_);
}

RectangleGeometry::RectangleGeometry(QRectF rectangle)
    : RectangleGeometry(rectangle, 0, 0)
{
}
RectangleGeometry::RectangleGeometry(QRectF rectangle, double radiusX, double radiusY)
    : rectangle_(rectangle)
    , radius_(radiusX, radiusY)
{
}

QRectF RectangleGeometry::Bounds()
{
    return rectangle_;
}

void RectangleGeometry::Draw(QPainter &painter)
{
    if (radius_.isEmpty())
        painter.drawRect(rectangle_);
    else
        painter.drawRoundedRect(rectangle_, radius_.width(), radius_.height());
}

EllipseGeometry::EllipseGeometry(QPointF center, double radiusX, double radiusY)
    : rectangle_(0, 0, radiusX, radiusY)
{
    rectangle_.moveCenter(center);
}

QRectF EllipseGeometry::Bounds()
{
    return rectangle_;
}

void EllipseGeometry::Draw(QPainter &painter)
{
    painter.drawEllipse(rectangle_);
}
