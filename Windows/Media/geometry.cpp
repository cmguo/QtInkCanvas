#include "Windows/Media/geometry.h"

#include <QPainter>

Geometry::Geometry()
{

}

Geometry::~Geometry()
{

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

void PathGeometry::Draw(QPainter &painter)
{
    painter.drawPath(path_);
}

QList<Geometry*>& GeometryGroup::Children()
{
    return children_;
}

void GeometryGroup::Draw(QPainter &painter)
{
    for (Geometry* g : children_)
        g->Draw(painter);
}

LineGeometry::LineGeometry(QPointF point0, QPointF point1)
{
}

void LineGeometry::Draw(QPainter &painter)
{
    painter.drawLine(line_);
}

RectangleGeometry::RectangleGeometry(QRectF rectangle)
{
}
RectangleGeometry::RectangleGeometry(QRectF rectangle, double radiusX, double radiusY)
{
}

void RectangleGeometry::Draw(QPainter &painter)
{
    painter.drawRect(rectangle_);
}

EllipseGeometry::EllipseGeometry(QPointF center, double radiusX, double radiusY)
{
}

void EllipseGeometry::Draw(QPainter &painter)
{
    painter.drawEllipse(rectangle_);
}
