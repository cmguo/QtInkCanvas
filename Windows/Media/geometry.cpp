#include "Windows/Media/geometry.h"

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

QList<Geometry*>& GeometryGroup::Children()
{
    return children_;
}

LineGeometry::LineGeometry(QPointF point0, QPointF point1)
{
}

RectangleGeometry::RectangleGeometry(QRectF rectangle)
{
}
RectangleGeometry::RectangleGeometry(QRectF rectangle, double radiusX, double radiusY)
{
}

EllipseGeometry::EllipseGeometry(QPointF center, double radiusX, double radiusY)
{
}
