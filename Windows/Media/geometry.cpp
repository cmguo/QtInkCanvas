#include "Windows/Media/geometry.h"

#ifdef INKCANVAS_QT_DRAW
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

#ifdef INKCANVAS_QT

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

PathGeometry* PathGeometry::Combine(Geometry *)
{
    return this;
}

Rect PathGeometry::Bounds()
{
    return path_.boundingRect();
}

#ifdef INKCANVAS_QT_DRAW
void PathGeometry::Draw(QPainter &painter)
{
    painter.drawPath(path_);
}
#endif

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
    Rect bounds = Rect::Empty();
    for (Geometry* g : children_)
        bounds.Union(g->Bounds());
    return bounds;
}

#ifdef INKCANVAS_QT_DRAW
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

#ifdef INKCANVAS_QT_DRAW
void LineGeometry::Draw(QPainter &painter)
{
    painter.drawLine(start_, end_);
}
#endif

RectangleGeometry::RectangleGeometry(Rect const & rectangle)
    : RectangleGeometry(rectangle, 0, 0)
{
}
RectangleGeometry::RectangleGeometry(Rect const & rectangle, double radiusX, double radiusY)
    : rectangle_(rectangle)
    , radius_(radiusX, radiusY)
{
}

Rect RectangleGeometry::Bounds()
{
    return rectangle_;
}

#ifdef INKCANVAS_QT_DRAW
void RectangleGeometry::Draw(QPainter &painter)
{
    if (radius_.IsEmpty())
        painter.drawRect(rectangle_);
    else
        painter.drawRoundedRect(rectangle_, radius_.Width(), radius_.Height());
}
#endif

EllipseGeometry::EllipseGeometry(Point const & center, double radiusX, double radiusY)
    : rectangle_(0, 0, radiusX, radiusY)
{
    rectangle_.Offset(center - Vector(rectangle_.GetSize()) / 2);
}

Rect EllipseGeometry::Bounds()
{
    return rectangle_;
}

#ifdef INKCANVAS_QT_DRAW
void EllipseGeometry::Draw(QPainter &painter)
{
    painter.drawEllipse(rectangle_);
}
#endif

INKCANVAS_END_NAMESPACE
