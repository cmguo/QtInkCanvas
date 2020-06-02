#include "Windows/Media/geometry.h"

#ifndef INKCANVAS_CORE
#include <QPainter>
#include <QDebug>
#endif

INKCANVAS_BEGIN_NAMESPACE

//static int count = 0;

Geometry::Geometry()
{
    //qDebug() << "Geometry ++ " << ++count;
}

Geometry::~Geometry()
{
    //qDebug() << "Geometry -- " << --count;
}

bool Geometry::tryTakeOwn(void *owner)
{
    if (owner_ == nullptr) {
        owner_ = owner;
        return true;
    }
    return owner_ == owner;
}

bool Geometry::releaseOwn(void *owner)
{
    if (owner_ == owner) {
        owner_ = nullptr;
        return true;
    }
    return owner_ == nullptr;
}

#ifndef INKCANVAS_CORE

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

Rect PathGeometry::Bounds()
{
    return path_.boundingRect();
}

void PathGeometry::Draw(Painter &painter)
{
    painter.drawPath(path_);
}

#endif

GeometryGroup::~GeometryGroup()
{
    for (Geometry* g : children_)
        delete g;
    children_.Clear();
}

List<Geometry*>& GeometryGroup::Children()
{
    return children_;
}

Rect GeometryGroup::Bounds()
{
    Rect bounds;
    for (Geometry* g : children_)
        bounds.Union(g->Bounds());
    return bounds;
}

#ifndef INKCANVAS_CORE
void GeometryGroup::Draw(QPainter &painter)
{
    for (Geometry* g : children_)
        g->Draw(painter);
}
#endif

LineGeometry::LineGeometry(Point const & point0, Point const & point1)
    : start_(point0)
    , end_(point1)
{
}

Rect LineGeometry::Bounds()
{
    return Rect(start_, end_);
}

#ifndef INKCANVAS_CORE
void LineGeometry::Draw(Painter &painter)
{
    painter.drawLine(start_, end_);
}
#endif

RectangleGeometry::RectangleGeometry(Rect rectangle)
    : RectangleGeometry(rectangle, 0, 0)
{
}
RectangleGeometry::RectangleGeometry(Rect rectangle, double radiusX, double radiusY)
    : rectangle_(rectangle)
    , radius_(radiusX, radiusY)
{
}

Rect RectangleGeometry::Bounds()
{
    return rectangle_;
}

#ifndef INKCANVAS_CORE
void RectangleGeometry::Draw(QPainter &painter)
{
    if (radius_.Empty())
        painter.drawRect(rectangle_);
    else
        painter.drawRoundedRect(rectangle_, radius_.Width(), radius_.Height());
}
#endif

EllipseGeometry::EllipseGeometry(Point center, double radiusX, double radiusY)
    : rectangle_(0, 0, radiusX, radiusY)
{
#ifndef INKCANVAS_CORE
    rectangle_.moveCenter(center);
#endif
}

Rect EllipseGeometry::Bounds()
{
    return rectangle_;
}

#ifndef INKCANVAS_CORE
void EllipseGeometry::Draw(QPainter &painter)
{
    painter.drawEllipse(rectangle_);
}
#endif

INKCANVAS_END_NAMESPACE
