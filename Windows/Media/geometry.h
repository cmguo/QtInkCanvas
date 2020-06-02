#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "InkCanvas_global.h"

#include "Windows/point.h"
#include "Windows/rect.h"
#include "Collections/Generic/list.h"

#ifndef INKCANVAS_CORE
#include <QPainterPath>
class QPainter;
#endif

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

enum SweepDirection
{
    /// <summary>
    ///     Counterclockwise -
    /// </summary>
    Counterclockwise = 0,

    /// <summary>
    ///     Clockwise -
    /// </summary>
    Clockwise = 1,
};


class Geometry
{
public:
    Geometry();

    virtual ~Geometry();

    virtual Rect Bounds() = 0;

#ifndef INKCANVAS_CORE
    virtual void Draw(QPainter& painter) = 0;
#endif

    bool tryTakeOwn(void * owner);

    bool releaseOwn(void * owner);

private:
    void * owner_ = nullptr;
};

#ifndef INKCANVAS_CORE

class PathGeometry : public Geometry
{
public:
    PathGeometry();

    Geometry * GetOutlinedPathGeometry();

    void Add(QPainterPath& path);

    PathGeometry* Combine(Geometry * geometry);

    virtual Rect Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    QPainterPath path_;
};

#endif

class GeometryGroup : public Geometry
{
public:
    virtual ~GeometryGroup() override;

    List<Geometry*>& Children();

    virtual Rect Bounds() override;

#ifndef INKCANVAS_CORE
    virtual void Draw(QPainter& painter) override;
#endif

private:
    List<Geometry*> children_;
};

class LineGeometry : public Geometry
{
public:
    LineGeometry(Point const & point0, Point const & point1);

    virtual Rect Bounds() override;

#ifndef INKCANVAS_CORE
    virtual void Draw(QPainter& painter) override;
#endif

private:
    Point start_;
    Point end_;
};

class RectangleGeometry : public Geometry
{
public:
    RectangleGeometry(Rect rectangle);
    RectangleGeometry(Rect rectangle, double radiusX, double radiusY);

    virtual Rect Bounds() override;

#ifndef INKCANVAS_CORE
    virtual void Draw(QPainter& painter) override;
#endif

private:
    Rect rectangle_;
    Size radius_;
};

class EllipseGeometry : public Geometry
{
public:
    EllipseGeometry(Point center, double radiusX, double radiusY);

    virtual Rect Bounds() override;

#ifndef INKCANVAS_CORE
    virtual void Draw(QPainter& painter) override;
#endif

private:
    Rect rectangle_;
};

INKCANVAS_END_NAMESPACE

#endif // GEOMETRY_H
