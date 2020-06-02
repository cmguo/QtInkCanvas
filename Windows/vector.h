#ifndef VECTOR_H
#define VECTOR_H

#include "cmath.h"
#include "double.h"

INKCANVAS_BEGIN_NAMESPACE

class Point;

/// <summary>
/// Vector - A value type which defined a vector in terms of X and Y
/// </summary>
class Vector
{
    //#region Constructors
public:
    Vector() : _x(0), _y(0) {}

    /// <summary>
    /// Constructor which sets the vector's initial values
    /// </summary>
    /// <param name="x"> double - The initial X </param>
    /// <param name="y"> double - THe initial Y </param>
    Vector(double x, double y)
    {
        _x = x;
        _y = y;
    }

    //#endregion Constructors

    //#region Public Methods

    /// <summary>
    /// Length Property - the length of this Vector
    /// </summary>
    double Length() const
    {
        return Math::Sqrt(_x*_x + _y*_y);
    }

    /// <summary>
    /// LengthSquared Property - the squared length of this Vector
    /// </summary>
    double LengthSquared() const
    {
        return _x*_x + _y*_y;
    }

    /// <summary>
    /// Normalize - Updates this Vector to maintain its direction, but to have a length
    /// of 1.  This is equivalent to dividing this Vector by Length
    /// </summary>
    void Normalize()
    {
        // Avoid overflow
        (*this) = (*this) / Math::Max(Math::Abs(_x),Math::Abs(_y));
        (*this) = (*this) / Length();
    }

    /// <summary>
    /// CrossProduct - Returns the cross product: vector1.X*vector2.Y - vector1.Y*vector2.X
    /// </summary>
    /// <returns>
    /// Returns the cross product: vector1.X*vector2.Y - vector1.Y*vector2.X
    /// </returns>
    /// <param name="vector1"> The first Vector </param>
    /// <param name="vector2"> The second Vector </param>
    static double CrossProduct(Vector vector1, Vector vector2)
    {
        return vector1._x * vector2._y - vector1._y * vector2._x;
    }

    /// <summary>
    /// AngleBetween - the angle between 2 vectors
    /// </summary>
    /// <returns>
    /// Returns the the angle in degrees between vector1 and vector2
    /// </returns>
    /// <param name="vector1"> The first Vector </param>
    /// <param name="vector2"> The second Vector </param>
    static double AngleBetween(Vector const & vector1, Vector const & vector2)
    {
        double sin = vector1._x * vector2._y - vector2._x * vector1._y;
        double cos = vector1._x * vector2._x + vector1._y * vector2._y;

        return Math::Atan2(sin, cos) * (180 / Math::PI);
    }

    //#endregion Public Methods

    //#region Public Operators
    /// <summary>
    /// Operator -Vector (unary negation)
    /// </summary>
    Vector operator - () const
    {
        return Vector(-_x,-_y);
    }

    /// <summary>
    /// Negates the values of X and Y on this Vector
    /// </summary>
    void Negate()
    {
        _x = -_x;
        _y = -_y;
    }

    /// <summary>
    /// Operator Vector + Vector
    /// </summary>
    friend Vector operator + (Vector const & vector1, Vector const & vector2)
    {
        return Vector(vector1._x + vector2._x,
                          vector1._y + vector2._y);
    }

    Vector& operator +=(Vector const & o)
    {
        _x += o._x; _y += o._y; return *this;
    }

    /// <summary>
    /// Add: Vector + Vector
    /// </summary>
    static Vector Add(Vector const & vector1, Vector const & vector2)
    {
        return Vector(vector1._x + vector2._x,
                          vector1._y + vector2._y);
    }

    /// <summary>
    /// Operator Vector - Vector
    /// </summary>
    friend Vector operator - (Vector const & vector1, Vector const & vector2)
    {
        return Vector(vector1._x - vector2._x,
                          vector1._y - vector2._y);
    }

    Vector& operator -=(Vector const & o)
    {
        _x -= o._x; _y -= o._y; return *this;
    }

    /// <summary>
    /// Subtract: Vector - Vector
    /// </summary>
    static Vector Subtract(Vector const & vector1, Vector const & vector2)
    {
        return Vector(vector1._x - vector2._x,
                          vector1._y - vector2._y);
    }

    /// <summary>
    /// Operator Vector + Point
    /// </summary>
    friend Point operator + (Vector const & vector, Point const & point);

    friend Point operator + (Point const & point, Vector const & vector);

    friend Point operator - (Point const & point, Vector const & vector);

    /// <summary>
    /// Add: Vector + Point
    /// </summary>
    static Point Add(Vector const & vector, Point const & point);

    /// <summary>
    /// Operator Vector * double
    /// </summary>
    friend Vector operator * (Vector vector, double scalar)
    {
        return Vector(vector._x * scalar,
                          vector._y * scalar);
    }

    Vector& operator *=(double scalar)
    {
        _x *= scalar; _y *= scalar; return *this;
    }

    /// <summary>
    /// Multiply: Vector * double
    /// </summary>
    static Vector Multiply(Vector vector, double scalar)
    {
        return Vector(vector._x * scalar,
                          vector._y * scalar);
    }

    /// <summary>
    /// Operator double * Vector
    /// </summary>
    friend Vector operator * (double scalar, Vector vector)
    {
        return Vector(vector._x * scalar,
                          vector._y * scalar);
    }

    /// <summary>
    /// Multiply: double * Vector
    /// </summary>
    friend Vector Multiply(double scalar, Vector vector)
    {
        return Vector(vector._x * scalar,
                          vector._y * scalar);
    }

    /// <summary>
    /// Operator Vector / double
    /// </summary>
    friend Vector operator / (Vector vector, double scalar)
    {
        return vector * (1.0 / scalar);
    }

    Vector& operator /=(double scalar)
    {
        _x /= scalar; _y /= scalar; return *this;
    }

    /// <summary>
    /// Multiply: Vector / double
    /// </summary>
    static Vector Divide(Vector vector, double scalar)
    {
        return vector * (1.0 / scalar);
    }

    /// <summary>
    /// Operator Vector * Vector, interpreted as their dot product
    /// </summary>
    friend double operator * (Vector const & vector1, Vector const & vector2)
    {
        return vector1._x * vector2._x + vector1._y * vector2._y;
    }

    /// <summary>
    /// Multiply - Returns the dot product: vector1.X*vector2.X + vector1.Y*vector2.Y
    /// </summary>
    /// <returns>
    /// Returns the dot product: vector1.X*vector2.X + vector1.Y*vector2.Y
    /// </returns>
    /// <param name="vector1"> The first Vector </param>
    /// <param name="vector2"> The second Vector </param>
    static double Multiply(Vector const & vector1, Vector const & vector2)
    {
        return vector1._x * vector2._x + vector1._y * vector2._y;
    }

    /// <summary>
    /// Determinant - Returns the determinant det(vector1, vector2)
    /// </summary>
    /// <returns>
    /// Returns the determinant: vector1.X*vector2.Y - vector1.Y*vector2.X
    /// </returns>
    /// <param name="vector1"> The first Vector </param>
    /// <param name="vector2"> The second Vector </param>
    static double Determinant(Vector const & vector1, Vector const & vector2)
    {
        return vector1._x * vector2._y - vector1._y * vector2._x;
    }


    friend bool operator==(Vector const & l, Vector const & r)
    {
        return l.X() == r.X() && l.Y() == r.Y();
    }

    friend bool operator!=(Vector const & l, Vector const & r)
    {
        return !(l == r);
    }

    static bool Equals(Vector const & l, Vector const & r)
    {
        return Double::Equals(l.X(), r.X()) &&
                Double::Equals(l.Y(), r.Y());
    }

    /// <summary>
    /// Explicit conversion to Point
    /// </summary>
    /// <returns>
    /// Point - A Point equal to this Vector
    /// </returns>
    /// <param name="vector"> Vector - the Vector to convert to a Point </param>
    operator Point() const;

    //#endregion Public Operators

    double X() const { return _x; }

    void SetX(double value) { _x = value; }

    double Y() const { return _y; }

    void SetY(double value) { _y = value; }

private:
    friend class Point;
    friend class Rect;
    friend class Matrix;

    double _x;
    double _y;
};

#ifndef POINT_H

INKCANVAS_END_NAMESPACE

#include "point.h"

INKCANVAS_BEGIN_NAMESPACE

Point operator +(const Vector &vector, const Point &point)
{
    return Point(point._x + vector._x, point._y + vector._y);
}

Point Vector::Add(const Vector &vector, const Point &point)
{
    return Point(point._x + vector._x, point._y + vector._y);
}

Vector::operator Point() const
{
    return Point(_x, _y);
}

#endif

INKCANVAS_END_NAMESPACE

#endif // VECTOR_H
