#ifndef DOUBLEUTIL_H
#define DOUBLEUTIL_H

#include <QSizeF>
#include <QPointF>
#include <QRectF>
#include <QList>

/// <summary>
/// Utility to provide double calculations.
/// </summary>
struct DoubleUtil
{
public:
    //static constexpr double DBL_EPSILON  =   2.2204460492503131e-016; /* smallest such that 1.0+DBL_EPSILON != 1.0 */
    //static constexpr float  FLT_MIN      =   1.175494351e-38F; /* Number close to zero, where float.MinValue is -float.MaxValue */

    /// <summary>
    /// AreClose - Returns whether or not two doubles are "close".  That is, whether or
    /// not they are within epsilon of each other.  Note that this epsilon is proportional
    /// to the numbers themselves to that AreClose survives scalar multiplication.
    /// There are plenty of ways for this to return false even for numbers which
    /// are theoretically identical, so no code calling this should fail to work if this
    /// returns false.  This is important enough to repeat:
    /// NB: NO CODE CALLING THIS FUNCTION SHOULD DEPEND ON ACCURATE RESULTS - this should be
    /// used for optimizations *only*.
    /// </summary>
    /// <returns>
    /// bool - the result of the AreClose comparision.
    /// </returns>
    /// <param name="value1"> The first double to compare. </param>
    /// <param name="value2"> The second double to compare. </param>
    static bool AreClose(double value1, double value2)
    {
        //in case they are Infinities (then epsilon check does not work)
        if(value1 == value2) return true;
        // This computes (|value1-value2| / (|value1| + |value2| + 10.0)) < DBL_EPSILON
        double eps = (qAbs(value1) + qAbs(value2) + 10.0) * DBL_EPSILON;
        double delta = value1 - value2;
        return(-eps < delta) && (eps > delta);
    }

    /// <summary>
    /// LessThan - Returns whether or not the first double is less than the second double.
    /// That is, whether or not the first is strictly less than *and* not within epsilon of
    /// the other number.  Note that this epsilon is proportional to the numbers themselves
    /// to that AreClose survives scalar multiplication.  Note,
    /// There are plenty of ways for this to return false even for numbers which
    /// are theoretically identical, so no code calling this should fail to work if this
    /// returns false.  This is important enough to repeat:
    /// NB: NO CODE CALLING THIS FUNCTION SHOULD DEPEND ON ACCURATE RESULTS - this should be
    /// used for optimizations *only*.
    /// </summary>
    /// <returns>
    /// bool - the result of the LessThan comparision.
    /// </returns>
    /// <param name="value1"> The first double to compare. </param>
    /// <param name="value2"> The second double to compare. </param>
    static bool LessThan(double value1, double value2)
    {
        return (value1 < value2) && !AreClose(value1, value2);
    }


    /// <summary>
    /// GreaterThan - Returns whether or not the first double is greater than the second double.
    /// That is, whether or not the first is strictly greater than *and* not within epsilon of
    /// the other number.  Note that this epsilon is proportional to the numbers themselves
    /// to that AreClose survives scalar multiplication.  Note,
    /// There are plenty of ways for this to return false even for numbers which
    /// are theoretically identical, so no code calling this should fail to work if this
    /// returns false.  This is important enough to repeat:
    /// NB: NO CODE CALLING THIS FUNCTION SHOULD DEPEND ON ACCURATE RESULTS - this should be
    /// used for optimizations *only*.
    /// </summary>
    /// <returns>
    /// bool - the result of the GreaterThan comparision.
    /// </returns>
    /// <param name="value1"> The first double to compare. </param>
    /// <param name="value2"> The second double to compare. </param>
    static bool GreaterThan(double value1, double value2)
    {
        return (value1 > value2) && !AreClose(value1, value2);
    }

    /// <summary>
    /// LessThanOrClose - Returns whether or not the first double is less than or close to
    /// the second double.  That is, whether or not the first is strictly less than or within
    /// epsilon of the other number.  Note that this epsilon is proportional to the numbers
    /// themselves to that AreClose survives scalar multiplication.  Note,
    /// There are plenty of ways for this to return false even for numbers which
    /// are theoretically identical, so no code calling this should fail to work if this
    /// returns false.  This is important enough to repeat:
    /// NB: NO CODE CALLING THIS FUNCTION SHOULD DEPEND ON ACCURATE RESULTS - this should be
    /// used for optimizations *only*.
    /// </summary>
    /// <returns>
    /// bool - the result of the LessThanOrClose comparision.
    /// </returns>
    /// <param name="value1"> The first double to compare. </param>
    /// <param name="value2"> The second double to compare. </param>
    static bool LessThanOrClose(double value1, double value2)
    {
        return (value1 < value2) || AreClose(value1, value2);
    }

    /// <summary>
    /// GreaterThanOrClose - Returns whether or not the first double is greater than or close to
    /// the second double.  That is, whether or not the first is strictly greater than or within
    /// epsilon of the other number.  Note that this epsilon is proportional to the numbers
    /// themselves to that AreClose survives scalar multiplication.  Note,
    /// There are plenty of ways for this to return false even for numbers which
    /// are theoretically identical, so no code calling this should fail to work if this
    /// returns false.  This is important enough to repeat:
    /// NB: NO CODE CALLING THIS FUNCTION SHOULD DEPEND ON ACCURATE RESULTS - this should be
    /// used for optimizations *only*.
    /// </summary>
    /// <returns>
    /// bool - the result of the GreaterThanOrClose comparision.
    /// </returns>
    /// <param name="value1"> The first double to compare. </param>
    /// <param name="value2"> The second double to compare. </param>
    static bool GreaterThanOrClose(double value1, double value2)
    {
        return (value1 > value2) || AreClose(value1, value2);
    }

    /// <summary>
    /// IsOne - Returns whether or not the double is "close" to 1.  Same as AreClose(double, 1),
    /// but this is faster.
    /// </summary>
    /// <returns>
    /// bool - the result of the AreClose comparision.
    /// </returns>
    /// <param name="value"> The double to compare to 1. </param>
    static bool IsOne(double value)
    {
        return qAbs(value-1.0) < 10.0 * DBL_EPSILON;
    }

    /// <summary>
    /// IsZero - Returns whether or not the double is "close" to 0.  Same as AreClose(double, 0),
    /// but this is faster.
    /// </summary>
    /// <returns>
    /// bool - the result of the AreClose comparision.
    /// </returns>
    /// <param name="value"> The double to compare to 0. </param>
    static bool IsZero(double value)
    {
        return qAbs(value) < 10.0 * DBL_EPSILON;
    }

    // The Point, Size, QRectF const &and Matrix class have moved to WinCorLib.  However, we provide
    // internal AreClose methods for our own use here.

    /// <summary>
    /// Compares two points for fuzzy equality.  This function
    /// helps compensate for the fact that double values can
    /// acquire error when operated upon
    /// </summary>
    /// <param name='point1'>The first QPointF const &to compare</param>
    /// <param name='point2'>The second QPointF const &to compare</param>
    /// <returns>Whether or not the two points are equal</returns>
    static bool AreClose(QPointF const &point1, QPointF const &point2)
    {
        return AreClose(point1.x(), point2.x()) &&
        AreClose(point1.y(), point2.y());
    }

    /// <summary>
    /// Compares two QSizeF const &instances for fuzzy equality.  This function
    /// helps compensate for the fact that double values can
    /// acquire error when operated upon
    /// </summary>
    /// <param name='size1'>The first QSizeF const &to compare</param>
    /// <param name='size2'>The second QSizeF const &to compare</param>
    /// <returns>Whether or not the two QSizeF const &instances are equal</returns>
    static bool AreClose(QSizeF const &size1, QSizeF const &size2)
    {
        return AreClose(size1.width(), size2.width()) &&
               AreClose(size1.height(), size2.height());
    }

    /// <summary>
    /// Compares two rectangles for fuzzy equality.  This function
    /// helps compensate for the fact that double values can
    /// acquire error when operated upon
    /// </summary>
    /// <param name='rect1'>The first rectangle to compare</param>
    /// <param name='rect2'>The second rectangle to compare</param>
    /// <returns>Whether or not the two rectangles are equal</returns>
    static bool AreClose(QRectF const &rect1, QRectF const &rect2)
    {
        // If they're both empty, don't bother with the double logic.
        if (rect1.isEmpty())
        {
            return rect2.isEmpty();
        }

        // At this point, rect1 isn't empty, so the first thing we can test is
        // rect2.isEmpty(), followed by property-wise compares.

        return (!rect2.isEmpty()) &&
            AreClose(rect1.x(), rect2.x()) &&
            AreClose(rect1.y(), rect2.y()) &&
            AreClose(rect1.height(), rect2.height()) &&
            AreClose(rect1.width(), rect2.width());
    }

    /// <summary>
    ///
    /// </summary>
    /// <param name="val"></param>
    /// <returns></returns>
    static bool IsBetweenZeroAndOne(double val)
    {
        return (GreaterThanOrClose(val, 0) && LessThanOrClose(val, 1));
    }

    /// <summary>
    ///
    /// </summary>
    /// <param name="val"></param>
    /// <returns></returns>
    static int DoubleToInt(double val)
    {
        return qRound(val);
    }


    /// <summary>
    /// rectHasNaN - this returns true if this QRectF const &has X, Y , Height or Width as NaN.
    /// </summary>
    /// <param name='r'>The rectangle to test</param>
    /// <returns>returns whether the QRectF const &has NaN</returns>
    static bool RectHasNaN(QRectF const &r)
    {
        if (    IsNaN(r.x())
             || IsNaN(r.y())
             || IsNaN(r.height())
             || IsNaN(r.width()) )
        {
            return true;
        }
        return false;
    }


    // The standard CLR double.IsNaN() function is approximately 100 times slower than our own wrapper,
    // so please make sure to use IsNaN() in performance sensitive code.
    // PS item that tracks the CLR improvement is DevDiv Schedule : 26916.
    // IEEE 754 : If the argument is any value in the range 0x7ff0000000000001L through 0x7fffffffffffffffL
    // or in the range 0xfff0000000000001L through 0xffffffffffffffffL, the result will be NaN.
    static bool IsNaN(double value)
    {
        return qIsNaN(value);
    }
};

inline qreal Determinant(QPointF const & p1, QPointF const & p2)
{
    return p1.x() * p2.y() - p2.x() * p1.y();
}

inline qreal LengthSquared(QPointF const & p)
{
    return QPointF::dotProduct(p, p);
}

inline qreal Length(QPointF const & p)
{
    return sqrt(QPointF::dotProduct(p, p));
}

template <typename T>
inline void RemoveRange(QList<T> list, int start, int count)
{
    list.erase(list.begin() + start, list.begin() + start + count);
}

#endif // DOUBLEUTIL_H
