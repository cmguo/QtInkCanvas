#ifndef RECT_H
#define RECT_H

#include "Windows/point.h"
#include "Windows/size.h"
#include "cmath.h"
#include "double.h"

#ifndef INKCANVAS_CORE
#include <QRectF>
#endif

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Rect - The primitive which represents a rectangle.  Rects are stored as
/// X, Y (Location) and Width and Height (Size).  As a result, Rects cannot have negative
/// Width or Height.
/// </summary>
class INKCANVAS_EXPORT Rect
{
    //#region Constructors
public:
    Rect() : _x(0), _y(0), _width(0), _height(0) {}

#ifndef INKCANVAS_CORE
    Rect(QRectF const & rect) : Rect(rect.topLeft(), rect.size()) {}
#endif

    /// <summary>
    /// Constructor which sets the initial values to the values of the parameters
    /// </summary>
    Rect(Point const & location,
                Size const & size)
    {
        if (size.IsEmpty())
        {
            *this = s_empty;
        }
        else
        {
            _x = location._x;
            _y = location._y;
            _width = size._width;
            _height = size._height;
        }
    }

    /// <summary>
    /// Constructor which sets the initial values to the values of the parameters.
    /// Width and Height must be non-negative
    /// </summary>
    Rect(double x,
                double y,
                double width,
                double height)
    {
        if (width < 0 || height < 0)
        {
            throw std::runtime_error("Size_WidthAndHeightCannotBeNegative");
        }

        _x    = x;
        _y     = y;
        _width   = width;
        _height  = height;
    }

    /// <summary>
    /// Constructor which sets the initial values to bound the two points provided.
    /// </summary>
    Rect(Point const & point1,
                Point const & point2)
    {
        _x = Math::Min(point1._x, point2._x);
        _y = Math::Min(point1._y, point2._y);

        //  Max with 0 to prevent double weirdness from causing us to be (-epsilon..0)
        _width = Math::Max(Math::Max(point1._x, point2._x) - _x, 0);
        _height = Math::Max(Math::Max(point1._y, point2._y) - _y, 0);
    }

    /// <summary>
    /// Constructor which sets the initial values to bound the point provided and the point
    /// which results from point + vector.
    /// </summary>
    Rect(Point const & point,
                Vector const & vector): Rect(point, point+vector)
    {
    }

    /// <summary>
    /// Constructor which sets the initial values to bound the (0,0) point and the point
    /// that results from (0,0) + size.
    /// </summary>
    Rect(Size const & size)
    {
        if(size.IsEmpty())
        {
            *this = s_empty;
        }
        else
        {
            _x = _y = 0;
            _width = size._width;
            _height = size._height;
        }
    }

    //#endregion Constructors

    //#region Statics

    /// <summary>
    /// Empty - a static property which provides an Empty rectangle.  X and Y are positive-infinity
    /// and Width and Height are negative infinity.  This is the only situation where Width or
    /// Height can be negative.
    /// </summary>
    static Rect Empty()
    {
        return s_empty;
    }

    //#endregion Statics

    //#region Public Properties

    /// <summary>
    /// IsEmpty - this returns true if this rect is the Empty rectangle.
    /// Note: If width or height are 0 this Rectangle still contains a 0 or 1 dimensional set
    /// of points, so this method should not be used to check for 0 area.
    /// </summary>
    bool IsEmpty() const;

    /// <summary>
    /// Location - The Point representing the origin of the Rectangle
    /// </summary>
    Point Location() const
    {
        return Point(_x, _y);
    }
    void SetLocation(Point const & value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotModifyEmptyRect");
        }

        _x = value._x;
        _y = value._y;
    }

    /// <summary>
    /// Size - The Size representing the area of the Rectangle
    /// </summary>
    Size GetSize() const
    {
        if (IsEmpty())
            return Size::Empty();
        return Size(_width, _height);
    }
    void SetSize(Size const & value)
    {
        if (value.IsEmpty())
        {
            *this = s_empty;
        }
        else
        {
            if (IsEmpty())
            {
                throw std::runtime_error("Rect_CannotModifyEmptyRect");
            }

            _width = value._width;
            _height = value._height;
        }
    }

    /// <summary>
    /// X - The X coordinate of the Location.
    /// If this is the empty rectangle, the value will be positive infinity.
    /// If this rect is Empty, setting this property is illegal.
    /// </summary>
    double X() const
    {
        return _x;
    }
    void SetX(double value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotModifyEmptyRect");
        }

        _x = value;
    }

    /// <summary>
    /// Y - The Y coordinate of the Location
    /// If this is the empty rectangle, the value will be positive infinity.
    /// If this rect is Empty, setting this property is illegal.
    /// </summary>
    double Y() const
    {
        return _y;
    }
    void SetY(double value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotModifyEmptyRect");
        }

        _y = value;
    }

    /// <summary>
    /// Width - The Width component of the Size.  This cannot be set to negative, and will only
    /// be negative if this is the empty rectangle, in which case it will be negative infinity.
    /// If this rect is Empty, setting this property is illegal.
    /// </summary>
    double Width() const
    {
        return _width;
    }
    void SetWidth(double value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotModifyEmptyRect");
        }

        if (value < 0)
        {
            throw std::runtime_error("Size_WidthCannotBeNegative");
        }

        _width = value;
    }

    /// <summary>
    /// Height - The Height component of the Size.  This cannot be set to negative, and will only
    /// be negative if this is the empty rectangle, in which case it will be negative infinity.
    /// If this rect is Empty, setting this property is illegal.
    /// </summary>
    double Height() const
    {
        return _height;
    }
    void SetHeight(double value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotModifyEmptyRect");
        }

        if (value < 0)
        {
            throw std::runtime_error("Size_HeightCannotBeNegative");
        }

        _height = value;
    }

    /// <summary>
    /// Left Property - This is a read-only alias for X
    /// If this is the empty rectangle, the value will be positive infinity.
    /// </summary>
    double Left() const
    {
        return _x;
    }

    /// <summary>
    /// Top Property - This is a read-only alias for Y
    /// If this is the empty rectangle, the value will be positive infinity.
    /// </summary>
    double Top() const
    {
        return _y;
    }

    /// <summary>
    /// Right Property - This is a read-only alias for X + Width
    /// If this is the empty rectangle, the value will be negative infinity.
    /// </summary>
    double Right() const
    {
        if (IsEmpty())
        {
            return Double::NegativeInfinity;
        }

        return _x + _width;
    }

    /// <summary>
    /// Bottom Property - This is a read-only alias for Y + Height
    /// If this is the empty rectangle, the value will be negative infinity.
    /// </summary>
    double Bottom() const
    {
        if (IsEmpty())
        {
            return Double::NegativeInfinity;
        }

        return _y + _height;
    }

    /// <summary>
    /// TopLeft Property - This is a read-only alias for the Point which is at X, Y
    /// If this is the empty rectangle, the value will be positive infinity, positive infinity.
    /// </summary>
    Point TopLeft() const
    {
        return Point(Left(), Top());
    }

    /// <summary>
    /// TopRight Property - This is a read-only alias for the Point which is at X + Width, Y
    /// If this is the empty rectangle, the value will be negative infinity, positive infinity.
    /// </summary>
    Point TopRight() const
    {
        return Point(Right(), Top());
    }

    /// <summary>
    /// BottomLeft Property - This is a read-only alias for the Point which is at X, Y + Height
    /// If this is the empty rectangle, the value will be positive infinity, negative infinity.
    /// </summary>
    Point BottomLeft() const
    {
        return Point(Left(), Bottom());
    }

    /// <summary>
    /// BottomRight Property - This is a read-only alias for the Point which is at X + Width, Y + Height
    /// If this is the empty rectangle, the value will be negative infinity, negative infinity.
    /// </summary>
    Point BottomRight() const
    {
        return Point(Right(), Bottom());
    }

#ifndef INKCANVAS_CORE
    operator QRectF() const { return QRectF(TopLeft(), GetSize()); }
#endif

    //#endregion Public Properties

    //#region Public Methods

    /// <summary>
    /// Contains - Returns true if the Point is within the rectangle, inclusive of the edges.
    /// Returns false otherwise.
    /// </summary>
    /// <param name="point"> The point which is being tested </param>
    /// <returns>
    /// Returns true if the Point is within the rectangle.
    /// Returns false otherwise
    /// </returns>
    bool Contains(Point const & point) const
    {
        return Contains(point._x, point._y);
    }

    /// <summary>
    /// Contains - Returns true if the Point represented by x,y is within the rectangle inclusive of the edges.
    /// Returns false otherwise.
    /// </summary>
    /// <param name="x"> X coordinate of the point which is being tested </param>
    /// <param name="y"> Y coordinate of the point which is being tested </param>
    /// <returns>
    /// Returns true if the Point represented by x,y is within the rectangle.
    /// Returns false otherwise.
    /// </returns>
    bool Contains(double x, double y) const
    {
        if (IsEmpty())
        {
            return false;
        }

        return ContainsInternal(x,y);
    }

    /// <summary>
    /// Contains - Returns true if the Rect non-Empty and is entirely contained within the
    /// rectangle, inclusive of the edges.
    /// Returns false otherwise
    /// </summary>
    bool Contains(Rect const & rect) const
    {
        if (IsEmpty() || rect.IsEmpty())
        {
            return false;
        }

        return (_x <= rect._x &&
                _y <= rect._y &&
                _x+_width >= rect._x+rect._width &&
                _y+_height >= rect._y+rect._height );
    }

    /// <summary>
    /// IntersectsWith - Returns true if the Rect intersects with this rectangle
    /// Returns false otherwise.
    /// Note that if one edge is coincident, this is considered an intersection.
    /// </summary>
    /// <returns>
    /// Returns true if the Rect intersects with this rectangle
    /// Returns false otherwise.
    /// or Height
    /// </returns>
    /// <param name="rect"> Rect </param>
    bool IntersectsWith(Rect const & rect) const
    {
        if (IsEmpty() || rect.IsEmpty())
        {
            return false;
        }

        return (rect.Left() <= Right()) &&
               (rect.Right() >= Left()) &&
               (rect.Top() <= Bottom()) &&
               (rect.Bottom() >= Top());
    }

    /// <summary>
    /// Intersect - Update this rectangle to be the intersection of this and rect
    /// If either this or rect are Empty, the result is Empty as well.
    /// </summary>
    /// <param name="rect"> The rect to intersect with this </param>
    void Intersect(Rect const & rect)
    {
        if (!IntersectsWith(rect))
        {
            *this = Empty();
        }
        else
        {
            double left   = Math::Max(Left(), rect.Left());
            double top    = Math::Max(Top(), rect.Top());

            //  Max with 0 to prevent double weirdness from causing us to be (-epsilon..0)
            _width = Math::Max(Math::Min(Right(), rect.Right()) - left, 0);
            _height = Math::Max(Math::Min(Bottom(), rect.Bottom()) - top, 0);

            _x = left;
            _y = top;
        }
    }

    /// <summary>
    /// Intersect - Return the result of the intersection of rect1 and rect2.
    /// If either this or rect are Empty, the result is Empty as well.
    /// </summary>
    static Rect Intersect(Rect rect1, Rect const & rect2)
    {
        rect1.Intersect(rect2);
        return rect1;
    }

    /// <summary>
    /// Union - Update this rectangle to be the union of this and rect.
    /// </summary>
    void Union(Rect const & rect)
    {
        if (IsEmpty())
        {
            *this = rect;
        }
        else if (!rect.IsEmpty())
        {
            double left = Math::Min(Left(), rect.Left());
            double top = Math::Min(Top(), rect.Top());


            // We need this check so that the math does not result in NaN
            if ((rect.Width() == Double::PositiveInfinity) || (Width() == Double::PositiveInfinity))
            {
                _width = Double::PositiveInfinity;
            }
            else
            {
                //  Max with 0 to prevent double weirdness from causing us to be (-epsilon..0)
                double maxRight = Math::Max(Right(), rect.Right());
                _width = Math::Max(maxRight - left, 0);
            }

            // We need this check so that the math does not result in NaN
            if ((rect.Height() == Double::PositiveInfinity) || (Height() == Double::PositiveInfinity))
            {
                _height = Double::PositiveInfinity;
            }
            else
            {
                //  Max with 0 to prevent double weirdness from causing us to be (-epsilon..0)
                double maxBottom = Math::Max(Bottom(), rect.Bottom());
                _height = Math::Max(maxBottom - top, 0);
            }

            _x = left;
            _y = top;
        }
    }

    /// <summary>
    /// Union - Return the result of the union of rect1 and rect2.
    /// </summary>
    static Rect Union(Rect rect1, Rect const & rect2)
    {
        rect1.Union(rect2);
        return rect1;
    }

    /// <summary>
    /// Union - Update this rectangle to be the union of this and point.
    /// </summary>
    void Union(Point const & point)
    {
        Union(Rect(point, point));
    }

    /// <summary>
    /// Union - Return the result of the union of rect and point.
    /// </summary>
    static Rect& Union(Rect& rect, Point const & point)
    {
        rect.Union(Rect(point, point));
        return rect;
    }

    /// <summary>
    /// Offset - translate the Location by the offset provided.
    /// If this is Empty, this method is illegal.
    /// </summary>
    void Offset(Vector const & offsetVector)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotCallMethod");
        }

        _x += offsetVector._x;
        _y += offsetVector._y;
    }

    /// <summary>
    /// Offset - translate the Location by the offset provided
    /// If this is Empty, this method is illegal.
    /// </summary>
    void Offset(double offsetX, double offsetY)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotCallMethod");
        }

        _x += offsetX;
        _y += offsetY;
    }

    /// <summary>
    /// Offset - return the result of offsetting rect by the offset provided
    /// If this is Empty, this method is illegal.
    /// </summary>
    static Rect Offset(Rect rect, Vector const & offsetVector)
    {
        rect.Offset(offsetVector.X(), offsetVector.Y());
        return rect;
    }

    /// <summary>
    /// Offset - return the result of offsetting rect by the offset provided
    /// If this is Empty, this method is illegal.
    /// </summary>
    static Rect Offset(Rect rect, double offsetX, double offsetY)
    {
        rect.Offset(offsetX, offsetY);
        return rect;
    }

    /// <summary>
    /// Inflate - inflate the bounds by the size provided, in all directions
    /// If this is Empty, this method is illegal.
    /// </summary>
    void Inflate(Size const & size)
    {
        Inflate(size._width, size._height);
    }

    /// <summary>
    /// Inflate - inflate the bounds by the size provided, in all directions.
    /// If -width is > Width / 2 or -height is > Height / 2, this Rect becomes Empty
    /// If this is Empty, this method is illegal.
    /// </summary>
    void Inflate(double width, double height)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Rect_CannotCallMethod");
        }

        _x -= width;
        _y -= height;

        // Do two additions rather than multiplication by 2 to avoid spurious overflow
        // That is: (A + 2 * B) != ((A + B) + B) if 2*B overflows.
        // Note that multiplication by 2 might work in this case because A should start
        // positive & be "clamped" to positive after, but consider A = Inf & B = -MAX.
        _width += width;
        _width += width;
        _height += height;
        _height += height;

        // We catch the case of inflation by less than -width/2 or -height/2 here.  This also
        // maintains the invariant that either the Rect is Empty or _width and _height are
        // non-negative, even if the user parameters were NaN, though this isn't strictly maintained
        // by other methods.
        if ( !(_width >= 0 && _height >= 0) )
        {
            *this = s_empty;
        }
    }

    /// <summary>
    /// Inflate - return the result of inflating rect by the size provided, in all directions
    /// If this is Empty, this method is illegal.
    /// </summary>
    static Rect Inflate(Rect rect, Size size)
    {
        rect.Inflate(size._width, size._height);
        return rect;
    }

    /// <summary>
    /// Inflate - return the result of inflating rect by the size provided, in all directions
    /// If this is Empty, this method is illegal.
    /// </summary>
    static Rect Inflate(Rect rect, double width, double height)
    {
        rect.Inflate(width, height);
        return rect;
    }

    /// <summary>
    /// Returns the bounds of the transformed rectangle.
    /// The Empty Rect is not affected by this call.
    /// </summary>
    /// <returns>
    /// The rect which results from the transformation.
    /// </returns>
    /// <param name="rect"> The Rect to transform. </param>
    /// <param name="matrix"> The Matrix by which to transform. </param>
//    static Rect& Transform(Rect & rect, Matrix const & matrix)
//    {
//        MatrixUtil.TransformRect(rect, matrix);
//        return rect;
//    }

    /// <summary>
    /// Updates rectangle to be the bounds of the original value transformed
    /// by the matrix.
    /// The Empty Rect is not affected by this call.
    /// </summary>
    /// <param name="matrix"> Matrix </param>
//    void Transform(Matrix matrix)
//    {
//        MatrixUtil.TransformRect(*this, matrix);
//    }

    /// <summary>
    /// Scale the rectangle in the X and Y directions
    /// </summary>
    /// <param name="scaleX"> The scale in X </param>
    /// <param name="scaleY"> The scale in Y </param>
    void Scale(double scaleX, double scaleY)
    {
        if (IsEmpty())
        {
            return;
        }

        _x *= scaleX;
        _y *= scaleY;
        _width *= scaleX;
        _height *= scaleY;

        // If the scale in the X dimension is negative, we need to normalize X and Width
        if (scaleX < 0)
        {
            // Make X the left-most edge again
            _x += _width;

            // and make Width positive
            _width *= -1;
        }

        // Do the same for the Y dimension
        if (scaleY < 0)
        {
            // Make Y the top-most edge again
            _y += _height;

            // and make Height positive
            _height *= -1;
        }
    }

    //#endregion Public Methods

    //#region Private Methods

private:
    /// <summary>
    /// ContainsInternal - Performs just the "point inside" logic
    /// </summary>
    /// <returns>
    /// bool - true if the point is inside the rect
    /// </returns>
    /// <param name="x"> The x-coord of the point to test </param>
    /// <param name="y"> The y-coord of the point to test </param>
    bool ContainsInternal(double x, double y) const
    {
        // We include points on the edge as "contained".
        // We do "x - _width <= _x" instead of "x <= _x + _width"
        // so that this check works when _width is PositiveInfinity
        // and _x is NegativeInfinity.
        return ((x >= _x) && (x - _width <= _x) &&
                (y >= _y) && (y - _height <= _y));
    }

    static Rect CreateEmptyRect()
    {
        Rect rect;
        // We can't set these via the property setters because negatives widths
        // are rejected in those APIs.
        rect._x = Double::PositiveInfinity;
        rect._y = Double::PositiveInfinity;
        rect._width = Double::NegativeInfinity;
        rect._height = Double::NegativeInfinity;
        return rect;
    }

    //#endregion Private Methods

    //#region Private Fields
private:
    static Rect s_empty;

    //#endregion Private Fields
    //------------------------------------------------------
    //
    //  Public Methods
    //
    //------------------------------------------------------

    //#region Public Methods




    /// <summary>
    /// Compares two Rect instances for exact equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double::NaN is not equal to itself.
    /// </summary>
    /// <returns>
    /// bool - true if the two Rect instances are exactly equal, false otherwise
    /// </returns>
    /// <param name='rect1'>The first Rect to compare</param>
    /// <param name='rect2'>The second Rect to compare</param>
    friend bool operator == (Rect const & rect1, Rect const & rect2)
    {
        return rect1.X() == rect2.X() &&
               rect1.Y() == rect2.Y() &&
               rect1.Width() == rect2.Width() &&
               rect1.Height() == rect2.Height();
    }

    /// <summary>
    /// Compares two Rect instances for exact inequality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double::NaN is not equal to itself.
    /// </summary>
    /// <returns>
    /// bool - true if the two Rect instances are exactly unequal, false otherwise
    /// </returns>
    /// <param name='rect1'>The first Rect to compare</param>
    /// <param name='rect2'>The second Rect to compare</param>
    friend bool operator != (Rect const & rect1, Rect const & rect2)
    {
        return !(rect1 == rect2);
    }
    /// <summary>
    /// Compares two Rect instances for object equality.  In this equality
    /// Double::NaN is equal to itself, unlike in numeric equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which
    /// are logically equal may fail.
    /// </summary>
    /// <returns>
    /// bool - true if the two Rect instances are exactly equal, false otherwise
    /// </returns>
    /// <param name='rect1'>The first Rect to compare</param>
    /// <param name='rect2'>The second Rect to compare</param>
    static bool Equals (Rect const & rect1, Rect const & rect2)
    {
        if (rect1.IsEmpty())
        {
            return rect2.IsEmpty();
        }
        else
        {
            return Double::Equals(rect1.X(), rect2.X()) &&
                   Double::Equals(rect1.Y(), rect2.Y()) &&
                   Double::Equals(rect1.Width(), rect2.Width()) &&
                   Double::Equals(rect1.Height(), rect2.Height());
        }
    }

    //#region Internal Fields

private:
    friend class MatrixUtil;

    double _x;
    double _y;
    double _width;
    double _height;

    //#endregion Internal Fields


};

INKCANVAS_END_NAMESPACE

#endif // RECT_H
