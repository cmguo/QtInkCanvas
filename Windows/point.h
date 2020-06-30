#ifndef POINT_H
#define POINT_H

#include "Windows/vector.h"

#ifdef INKCANVAS_QT
#include <QPointF>
#endif

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Point - Defaults to 0,0
/// </summary>
class INKCANVAS_EXPORT Point
{
    //#region Constructors
public:
    Point() : _x(0), _y(0) {}

#ifdef INKCANVAS_QT
    Point(QPointF const & point) : Point(point.x(), point.y()) {}
#endif

    /// <summary>
    /// Constructor which accepts the X and Y values
    /// </summary>
    /// <param name="x">The value for the X coordinate of the new Point</param>
    /// <param name="y">The value for the Y coordinate of the new Point</param>
    Point(double x, double y)
    {
        _x = x;
        _y = y;
    }

    //#endregion Constructors

    //#region Public Methods

    /// <summary>
    /// Offset - update the location by adding offsetX to X and offsetY to Y
    /// </summary>
    /// <param name="offsetX"> The offset in the x dimension </param>
    /// <param name="offsetY"> The offset in the y dimension </param>
    void Offset(double offsetX, double offsetY)
    {
        _x += offsetX;
        _y += offsetY;
    }

    /// <summary>
    /// Operator Point + Vector
    /// </summary>
    /// <returns>
    /// Point - The result of the addition
    /// </returns>
    /// <param name="point"> The Point to be added to the Vector </param>
    /// <param name="vector"> The Vectr to be added to the Point </param>
    friend Point operator + (Point const & point, Vector const & vector)
    {
         return Point(point._x + vector._x, point._y + vector._y);
    }

    friend Point operator + (Vector const & vector, Point const & point);

    /// <summary>
    /// Add: Point + Vector
    /// </summary>
    /// <returns>
    /// Point - The result of the addition
    /// </returns>
    /// <param name="point"> The Point to be added to the Vector </param>
    /// <param name="vector"> The Vector to be added to the Point </param>
    static Point Add(Point const & point, Vector const & vector)
    {
        return point + vector;
    }

    /// <summary>
    /// Operator Point - Vector
    /// </summary>
    /// <returns>
    /// Point - The result of the subtraction
    /// </returns>
    /// <param name="point"> The Point from which the Vector is subtracted </param>
    /// <param name="vector"> The Vector which is subtracted from the Point </param>
    friend Point operator - (Point const & point, Vector const & vector)
    {
         return Point(point._x - vector._x, point._y - vector._y);
    }

    /// <summary>
    /// Subtract: Point - Vector
    /// </summary>
    /// <returns>
    /// Point - The result of the subtraction
    /// </returns>
    /// <param name="point"> The Point from which the Vector is subtracted </param>
    /// <param name="vector"> The Vector which is subtracted from the Point </param>
    static Point Subtract(Point const & point, Vector const & vector)
    {
        return point - vector;
    }

    /// <summary>
    /// Operator Point - Point
    /// </summary>
    /// <returns>
    /// Vector - The result of the subtraction
    /// </returns>
    /// <param name="point1"> The Point from which point2 is subtracted </param>
    /// <param name="point2"> The Point subtracted from point1 </param>
    friend Vector operator - (Point const & point1, Point const & point2)
    {
        return Vector(point1._x - point2._x, point1._y - point2._y);
    }

    /// <summary>
    /// Subtract: Point - Point
    /// </summary>
    /// <returns>
    /// Vector - The result of the subtraction
    /// </returns>
    /// <param name="point1"> The Point from which point2 is subtracted </param>
    /// <param name="point2"> The Point subtracted from point1 </param>
    static Vector Subtract(Point const & point1, Point const & point2)
    {
        return point1 - point2;
    }

    /// <summary>
    /// Operator Point * Matrix
    /// </summary>
    //friend Point operator * (Point const & point, Matrix const & matrix);

    /// <summary>
    /// Explicit conversion to Vector
    /// </summary>
    /// <returns>
    /// Vector - A Vector equal to this Point
    /// </returns>
    /// <param name="point"> Point - the Point to convert to a Vector </param>
    operator Vector() const
    {
        return Vector(_x, _y);
    }

    //#endregion Public Methods

    double X() const { return _x; }

    double Y() const { return _y; }

    void setX(double x) { _x = x; }

    void setY(double y) { _y = y; }

#ifdef INKCANVAS_QT
    operator QPointF() const { return {_x, _y}; }
#endif

    friend bool operator==(Point const & l, Point const & r)
    {
        return l.X() == r.X() && l.Y() == r.Y();
    }

    friend bool operator!=(Point const & l, Point const & r)
    {
        return !(l == r);
    }

    static bool Equals(Point const & l, Point const & r)
    {
        return Double::Equals(l.X(), r.X()) &&
                Double::Equals(l.Y(), r.Y());
    }

private:
    friend class Vector;
    friend class Rect;
    friend class Matrix;

    double _x;
    double _y;
};

INKCANVAS_END_NAMESPACE

#endif // POINT_H
