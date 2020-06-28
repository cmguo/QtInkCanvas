#ifndef SIZE_H
#define SIZE_H

#include "Windows/point.h"

#ifndef INKCANVAS_CORE
#include <QSizeF>
#endif

#include <stdexcept>

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Size - A value type which defined a size in terms of non-negative width and height
/// </summary>
class INKCANVAS_EXPORT Size
{
    //#region Constructors
public:
    Size() : _width(0), _height(0) {}

#ifndef INKCANVAS_CORE
    Size(QSizeF const & size) : Size(size.width(), size.height()) {}
#endif

    /// <summary>
    /// Constructor which sets the size's initial values.  Width and Height must be non-negative
    /// </summary>
    /// <param name="width"> double - The initial Width </param>
    /// <param name="height"> double - THe initial Height </param>
    Size(double width, double height)
    {
        if (width < 0 || height < 0)
        {
            //throw std::runtime_error();
        }

        _width = width;
        _height = height;
    }

    //#endregion Constructors

    //#region Statics

    /// <summary>
    /// Empty - a static property which provides an Empty size.  Width and Height are
    /// negative-infinity.  This is the only situation
    /// where size can be negative.
    /// </summary>
    static Size Empty()
    {
        return s_empty;
    }

    //#endregion Statics

    //#region Public Methods and Properties

    /// <summary>
    /// IsEmpty - this returns true if this size is the Empty size.
    /// Note: If size is 0 this Size still contains a 0 or 1 dimensional set
    /// of points, so this method should not be used to check for 0 area.
    /// </summary>
    bool IsEmpty() const
    {
        return _width < 0;
    }

    /// <summary>
    /// Width - Default is 0, must be non-negative
    /// </summary>
    double Width() const
    {
        return _width;
    }
    void setWidth(double value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Size_CannotModifyEmptySize");
        }

        if (value < 0)
        {
            throw std::runtime_error("Size_WidthCannotBeNegative");
        }

        _width = value;
    }

    /// <summary>
    /// Height - Default is 0, must be non-negative.
    /// </summary>
    double Height() const
    {
        return _height;
    }
    void setHeight(double value)
    {
        if (IsEmpty())
        {
            throw std::runtime_error("Size_CannotModifyEmptySize");
        }

        if (value < 0)
        {
            throw std::runtime_error("Size_WidthCannotBeNegative");
        }

        _height = value;
    }

    //#endregion Public Methods

    //#region Public Operators

    /// <summary>
    /// Explicit conversion to Vector.
    /// </summary>
    /// <returns>
    /// Vector - A Vector equal to this Size
    /// </returns>
    /// <param name="size"> Size - the Size to convert to a Vector </param>
    operator Vector() const
    {
        return Vector(_width, _height);
    }

    /// <summary>
    /// Explicit conversion to Point
    /// </summary>
    /// <returns>
    /// Point - A Point equal to this Size
    /// </returns>
    /// <param name="size"> Size - the Size to convert to a Point </param>
    operator Point() const
    {
        return Point(_width, _height);
    }

#ifndef INKCANVAS_CORE
    operator QSizeF() const { return {_width, _height}; }
#endif

    //#endregion Public Operators

    //#region Private Methods

private:
    static Size CreateEmptySize()
    {
        Size size;
        // We can't set these via the property setters because negatives widths
        // are rejected in those APIs.
        size._width = Double::NegativeInfinity;
        size._height = Double::NegativeInfinity;
        return size;
    }

    //#endregion Private Methods

    //#region Private Fields

private:
    static Size const s_empty;

    //#endregion Private Fields
    //------------------------------------------------------
    //
    //  Public Methods
    //
    //------------------------------------------------------

    //#region Public Methods




    /// <summary>
    /// Compares two Size instances for exact equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double.NaN is not equal to itself.
    /// </summary>
    /// <returns>
    /// bool - true if the two Size instances are exactly equal, false otherwise
    /// </returns>
    /// <param name='size1'>The first Size to compare</param>
    /// <param name='size2'>The second Size to compare</param>
    friend bool operator == (Size const & size1, Size const & size2)
    {
        return size1._width == size2._width &&
               size1._height == size2._height;
    }

    /// <summary>
    /// Compares two Size instances for exact inequality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double.NaN is not equal to itself.
    /// </summary>
    /// <returns>
    /// bool - true if the two Size instances are exactly unequal, false otherwise
    /// </returns>
    /// <param name='size1'>The first Size to compare</param>
    /// <param name='size2'>The second Size to compare</param>
    friend bool operator != (Size size1, Size size2)
    {
        return !(size1 == size2);
    }
    /// <summary>
    /// Compares two Size instances for object equality.  In this equality
    /// Double.NaN is equal to itself, unlike in numeric equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which
    /// are logically equal may fail.
    /// </summary>
    /// <returns>
    /// bool - true if the two Size instances are exactly equal, false otherwise
    /// </returns>
    /// <param name='size1'>The first Size to compare</param>
    /// <param name='size2'>The second Size to compare</param>
    static bool Equals (Size const & size1, Size const & size2)
    {
        if (size1.IsEmpty())
        {
            return size2.IsEmpty();
        }
        else
        {
            return size1 == size2;
        }
    }

    /// <summary>
    /// Equals - compares this Size with the passed in object.  In this equality
    /// Double.NaN is equal to itself, unlike in numeric equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which
    /// are logically equal may fail.
    /// </summary>
    /// <returns>
    /// bool - true if "value" is equal to "this".
    /// </returns>
    /// <param name='value'>The Size to compare to "this"</param>
    bool Equals(Size const & value) const
    {
        return Equals(*this, value);
    }
    /// <summary>
    /// Returns the HashCode for this Size
    /// </summary>
    /// <returns>
    /// int - the HashCode for this Size
    /// </returns>
public:
    int GetHashCode()
    {
        if (IsEmpty())
        {
            return 0;
        }
        else
        {
            // Perform field-by-field XOR of HashCodes
            return static_cast<int>(_width) ^ static_cast<int>(_height);
        }
    }


    //#endregion Public Methods

    //------------------------------------------------------
    //
    //  Public Properties
    //
    //------------------------------------------------------




    //#region Public Properties



    //#endregion Public Properties

    //------------------------------------------------------
    //
    //  Protected Methods
    //
    //------------------------------------------------------

    //#region Protected Methods





    //#endregion ProtectedMethods

    //------------------------------------------------------
    //
    //  Internal Methods
    //
    //------------------------------------------------------

    //#region Internal Methods


    //#endregion Internal Methods

    //------------------------------------------------------
    //
    //  Internal Properties
    //
    //------------------------------------------------------

    //#region Internal Properties


    //#endregion Internal Properties

    //------------------------------------------------------
    //
    //  Dependency Properties
    //
    //------------------------------------------------------

    //#region Dependency Properties



    //#endregion Dependency Properties

    //------------------------------------------------------
    //
    //  Internal Fields
    //
    //------------------------------------------------------

    //#region Internal Fields


private:
    friend class Rect;

    double _width;
    double _height;

};

INKCANVAS_END_NAMESPACE

#endif // SIZE_H
