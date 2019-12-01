#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QRectF>
#include <QList>

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

// namespace System.Windows.Media

class Geometry
{
public:
    Geometry();

    virtual ~Geometry();
};

#include <QPainterPath>

class PathGeometry : public Geometry
{
public:
    PathGeometry();

    Geometry * GetOutlinedPathGeometry();

    void Add(QPainterPath& path);

    PathGeometry* Combine(Geometry * geometry);

private:
    QPainterPath path_;
};

class GeometryGroup : public Geometry
{
public:
    QList<Geometry*>& Children();

private:
    QList<Geometry*> children_;
};

class LineGeometry : public Geometry
{
public:
    LineGeometry(QPointF point0, QPointF point1);
private:
    QLine line_;
};

class RectangleGeometry : public Geometry
{
public:
    RectangleGeometry(QRectF rectangle);
    RectangleGeometry(QRectF rectangle, double radiusX, double radiusY);
private:
    QRectF rectangle_;
    QSizeF radius_;
};

class EllipseGeometry : public Geometry
{
public:
    EllipseGeometry(QPointF center, double radiusX, double radiusY);
private:
    QRectF rectangle_;
};

#endif // GEOMETRY_H
