#ifndef STYLUSPOINT_H
#define STYLUSPOINT_H

#include "styluspointdescription.h"
#include "styluspointproperties.h"

#include <QPointF>
#include <QVector>
#include <QMap>
#include <QSharedPointer>

class StylusPointDescription;

class StylusPoint
{
public:
    static constexpr float DefaultPressure = 0.5f;

private:
    double _x;
    double _y;
    float _pressureFactor;
    QVector<int> _additionalValues;
    QSharedPointer<StylusPointDescription> _stylusPointDescription;

    /// <summary>
    /// StylusPoint
    /// </summary>
    /// <param name="x">x
    /// <param name="y">y
public:
    StylusPoint() {}

    StylusPoint(QPointF const & p)
        : StylusPoint(p.x(), p.y())
    {
    }

    StylusPoint(double x, double y);

    /// <summary>
    /// StylusPoint
    /// </summary>
    /// <param name="x">x
    /// <param name="y">y
    /// <param name="pressureFactor">pressureFactor
    StylusPoint(double x, double y, float pressureFactor);


    /// <summary>
    /// StylusPoint
    /// </summary>
    /// <param name="x">x
    /// <param name="y">y
    /// <param name="pressureFactor">pressureFactor
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="additionalValues">additionalValues
    StylusPoint(double x, double y, float pressureFactor,
                QSharedPointer<StylusPointDescription> stylusPointDescription,
                QVector<int> const & additionalValues);

    /// <summary>
    /// ctor
    /// </summary>
    StylusPoint(
        double x,
        double y,
        float pressureFactor,
        QSharedPointer<StylusPointDescription> stylusPointDescription,
        QVector<int> const & additionalValues,
        bool validateAdditionalData,
        bool validatePressureFactor);


    /// <summary>
    /// The Maximum X or Y value supported for backwards compatibility with previous inking platforms
    /// </summary>
    static constexpr double MaxXY = 81164736.28346430;

    /// <summary>
    /// The Minimum X or Y value supported for backwards compatibility with previous inking platforms
    /// </summary>
    static constexpr double MinXY = -81164736.32125960;

    /// <summary>
    /// X
    /// </summary>
    double X() const
    {
        return _x;
    }

    void SetX(double value);

    /// <summary>
    /// Y
    /// </summary>
    double Y() const
    {
        return _y;
    }

    void SetY(double value);

    /// <summary>
    /// PressureFactor.  A value between 0.0 (no pressure) and 1.0 (max pressure)
    /// </summary>
    float PressureFactor() const;

    void SetPressureFactor(float value);

    /// <summary>
    /// Describes the properties this StylusPoint contains
    /// </summary>
    QSharedPointer<StylusPointDescription> Description();

    void SetDescription(QSharedPointer<StylusPointDescription> value);

    /// <summary>
    /// Returns true if this StylusPoint supports the specified property
    /// </summary>
    /// <param name="stylusPointProperty">The StylusPointProperty to see if this StylusPoint supports
    bool HasProperty(StylusPointProperty & stylusPointProperty);

    /// <summary>
    /// Provides read access to all stylus properties
    /// </summary>
    /// <param name="stylusPointProperty">The StylusPointPropertyIds of the property to retrieve
    int GetPropertyValue(StylusPointProperty & stylusPointProperty);

    /// <summary>
    /// Allows supported properties to be set
    /// </summary>
    /// <param name="stylusPointProperty">The property to set, it must exist on this StylusPoint
    /// <param name="value">value
    void SetPropertyValue(StylusPointProperty & stylusPointProperty, int value);

    /// <summary>
    /// Optimization that lets the ctor call setvalue repeatly without causing a copy of the int[]
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    /// <param name="value">value
    /// <param name="copyBeforeWrite">
    void SetPropertyValue(StylusPointProperty & stylusPointProperty, int value, bool copyBeforeWrite);

    /// <summary>
    /// Explicit cast converter between StylusPoint and Point
    /// </summary>
    /// <param name="stylusPoint">stylusPoint
    operator QPointF() const
    {
        return QPointF(_x, _y);
    }

    /// <summary>
    /// Allows languages that don't support operator overloading
    /// to convert to a point
    /// </summary>
    QPointF ToPoint()
    {
        return QPointF(_x, _y);
    }

    /// <summary>
    /// Compares two StylusPoint instances for exact equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double.NaN is not equal to itself.
    /// Descriptions must match for equality to succeed and additional values must match
    /// </summary>
    /// <returns>
    /// bool - true if the two Stylus instances are exactly equal, false otherwise
    /// </returns>
    /// <param name="stylusPoint1">The first StylusPoint to compare
    /// <param name="stylusPoint2">The second StylusPoint to compare
    friend bool operator ==(StylusPoint & stylusPoint1, StylusPoint & stylusPoint2)
    {
        return StylusPoint::Equals(stylusPoint1, stylusPoint2);
    }

    /// <summary>
    /// Compares two StylusPoint instances for exact inequality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double.NaN is not equal to itself.
    /// </summary>
    /// <returns>
    /// bool - true if the two Stylus instances are exactly inequal, false otherwise
    /// </returns>
    /// <param name="stylusPoint1">The first StylusPoint to compare
    /// <param name="stylusPoint2">The second StylusPoint to compare
    friend bool operator !=(StylusPoint & stylusPoint1, StylusPoint & stylusPoint2)
    {
        return !StylusPoint::Equals(stylusPoint1, stylusPoint2);
    }

    /// <summary>
    /// Compares two StylusPoint instances for exact equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double.NaN is not equal to itself.
    /// Descriptions must match for equality to succeed and additional values must match
    /// </summary>
    /// <returns>
    /// bool - true if the two Stylus instances are exactly equal, false otherwise
    /// </returns>
    /// <param name="stylusPoint1">The first StylusPoint to compare
    /// <param name="stylusPoint2">The second StylusPoint to compare
    static bool Equals(StylusPoint & stylusPoint1, StylusPoint & stylusPoint2);

    /// <summary>
    /// Compares two StylusPoint instances for exact equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which are logically equal may fail.
    /// Furthermore, using this equality operator, Double.NaN is not equal to itself.
    /// Descriptions must match for equality to succeed and additional values must match
    /// </summary>
    /// <returns>
    /// bool - true if the object is an instance of StylusPoint and if it's equal to "this".
    /// </returns>
    /// <param name="o">The object to compare to "this"
    //override bool Equals(object o)
    //{
    //    if ((null == o) || !(o is StylusPoint))
    //    {
    //        return false;
    //    }

    //    StylusPoint value = (StylusPoint)o;
    //    return StylusPoint.Equals(this, value);
    //}

    /// <summary>
    /// Equals - compares this StylusPoint with the passed in object.  In this equality
    /// Double.NaN is equal to itself, unlike in numeric equality.
    /// Note that double values can acquire error when operated upon, such that
    /// an exact comparison between two values which
    /// are logically equal may fail.
    /// </summary>
    /// <returns>
    /// bool - true if "value" is equal to "this".
    /// </returns>
    /// <param name="value">The StylusPoint to compare to "this"
    bool Equals(StylusPoint & value)
    {
        return StylusPoint::Equals(*this, value);
    }

    /// <summary>
    /// Returns the HashCode for this StylusPoint
    /// </summary>
    /// <returns>
    /// int - the HashCode for this StylusPoint
    /// </returns>
    //override int GetHashCode()
    //{
    //    int hash =
    //        _x.GetHashCode() ^
    //        _y.GetHashCode() ^
    //        _pressureFactor.GetHashCode();

    //    if (_stylusPointDescription != null)
    //    {
    //        hash ^= _stylusPointDescription.GetHashCode();
    //    }

    //    if (_additionalValues != null)
    //    {
    //        for (int x = 0; x < _additionalValues.Length; x++)
    //        {
    //            hash ^= _additionalValues[x]; //don't call GetHashCode on integers, it just returns the int
    //        }
    //    }

    //    return hash;
    //}

    /// <summary>
    /// Used by the StylusPointCollection.ToHimetricArray method
    /// </summary>
    /// <returns></returns>
    QVector<int> GetAdditionalData()
    {
        //return a direct ref
        return _additionalValues;
    }

    /// <summary>
    /// helper used by SPC.Reformat to preserve the pressureFactor
    /// </summary>
    float GetUntruncatedPressureFactor()
    {
        return _pressureFactor;
    }

    /// <summary>
    /// GetPacketData - returns avalon space packet data with true pressure if it exists
    /// </summary>
    QVector<int> GetPacketData();

    /// <summary>
    /// helper to determine if a stroke has default pressure
    /// This is used by ISF serialization to not serialize pressure
    /// </summary>
    bool HasDefaultPressure();

    /// <summary>
    /// Used by the SetPropertyData to make a copy of the data
    /// before modifying it.  This is required so that we don't
    /// have two StylusPoint's sharing the same int[]
    /// which can happen when you call: StylusPoint p = otherStylusPoint
    /// because the CLR just does a memberwise copy
    /// </summary>
    /// <returns></returns>
    void CopyAdditionalData();

    /// <summary>
    /// helper that returns a double clamped to MaxXY or MinXY
    /// We only accept values in this range to support ISF serialization
    /// </summary>
    static double GetClampedXYValue(double xyValue);

public:
    static qreal length(QPointF const & p)
    {
        return sqrt(QPointF::dotProduct(p, p));
    }

    static qreal Determinant(QPointF const & p1, QPointF const & p2)
    {
        return p1.x() * p2.y() - p2.x() * p1.y();
    }
};

#endif // STYLUSPOINT_H
