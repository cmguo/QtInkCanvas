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

class QPainter;

class Geometry
{
public:
    Geometry();

    virtual ~Geometry();

    virtual QRectF Bounds() = 0;

    virtual void Draw(QPainter& painter) = 0;

    bool tryTakeOwn(void * owner);

    bool releaseOwn(void * owner);

private:
    void * owner_ = nullptr;
};

#include <QPainterPath>

class PathGeometry : public Geometry
{
public:
    PathGeometry();

    Geometry * GetOutlinedPathGeometry();

    void Add(QPainterPath& path);

    PathGeometry* Combine(Geometry * geometry);

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    QPainterPath path_;
};

class GeometryGroup : public Geometry
{
public:
    virtual ~GeometryGroup() override;

    QList<Geometry*>& Children();

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    QList<Geometry*> children_;
};

class LineGeometry : public Geometry
{
public:
    LineGeometry(QPointF point0, QPointF point1);

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    QLineF line_;
};

class RectangleGeometry : public Geometry
{
public:
    RectangleGeometry(QRectF rectangle);
    RectangleGeometry(QRectF rectangle, double radiusX, double radiusY);

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    QRectF rectangle_;
    QSizeF radius_;
};

class EllipseGeometry : public Geometry
{
public:
    EllipseGeometry(QPointF center, double radiusX, double radiusY);

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painter) override;

private:
    QRectF rectangle_;
};

#endif // GEOMETRY_H
