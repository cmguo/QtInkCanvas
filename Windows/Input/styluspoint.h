#ifndef WINDOWS_INPUT_STYLUSPOINT_H
#define WINDOWS_INPUT_STYLUSPOINT_H

#include "InkCanvas_global.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluspointproperties.h"
#include "Windows/point.h"
#include "sharedptr.h"

INKCANVAS_BEGIN_NAMESPACE

class StylusPointDescription;

// namespace System.Windows.Input

class INKCANVAS_EXPORT StylusPoint
{
public:
    static constexpr float DefaultPressure = 0.5f;

private:
    double _x;
    double _y;
    float _pressureFactor;
    Array<int> _additionalValues;
    mutable SharedPointer<StylusPointDescription> _stylusPointDescription;

    /// <summary>
    /// StylusPoint
    /// </summary>
    /// <param name="x">x
    /// <param name="y">y
public:
    StylusPoint() : StylusPoint(0, 0) {}

    StylusPoint(Point const & p)
        : StylusPoint(p.X(), p.Y())
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
                SharedPointer<StylusPointDescription> stylusPointDescription,
                Array<int> const & additionalValues);

    /// <summary>
    /// ctor
    /// </summary>
    StylusPoint(
        double x,
        double y,
        float pressureFactor,
        SharedPointer<StylusPointDescription> stylusPointDescription,
        Array<int> const & additionalValues,
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
    SharedPointer<StylusPointDescription> Description() const;

    void SetDescription(SharedPointer<StylusPointDescription> value);

    /// <summary>
    /// Returns true if this StylusPoint supports the specified property
    /// </summary>
    /// <param name="stylusPointProperty">The StylusPointProperty to see if this StylusPoint supports
    bool HasProperty(StylusPointProperty const & stylusPointProperty) const;

    /// <summary>
    /// Provides read access to all stylus properties
    /// </summary>
    /// <param name="stylusPointProperty">The StylusPointPropertyIds of the property to retrieve
    int GetPropertyValue(StylusPointProperty const & stylusPointProperty) const;

    /// <summary>
    /// Allows supported properties to be set
    /// </summary>
    /// <param name="stylusPointProperty">The property to set, it must exist on this StylusPoint
    /// <param name="value">value
    void SetPropertyValue(StylusPointProperty const & stylusPointProperty, int value);

    /// <summary>
    /// Optimization that lets the ctor call setvalue repeatly without causing a copy of the int[]
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    /// <param name="value">value
    /// <param name="copyBeforeWrite">
    void SetPropertyValue(StylusPointProperty const & stylusPointProperty, int value, bool copyBeforeWrite);

    /// <summary>
    /// Explicit cast converter between StylusPoint and Point
    /// </summary>
    /// <param name="stylusPoint">stylusPoint
    operator Point() const
    {
        return Point(_x, _y);
    }

    /// <summary>
    /// Allows languages that don't support operator overloading
    /// to convert to a point
    /// </summary>
    Point ToPoint() const
    {
        return Point(_x, _y);
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
    friend bool operator ==(StylusPoint const & stylusPoint1, StylusPoint const & stylusPoint2)
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
    static bool Equals(StylusPoint const & stylusPoint1, StylusPoint const & stylusPoint2);

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
    bool Equals(StylusPoint & value) const
    {
        return StylusPoint::Equals(*this, value);
    }

    /// <summary>
    /// Returns the HashCode for this StylusPoint
    /// </summary>
    /// <returns>
    /// int - the HashCode for this StylusPoint
    /// </returns>
    int GetHashCode() const;

#ifdef INKCANVAS_QT
    friend uint qHash(StylusPoint const & o)
    {
        return o.GetHashCode();
    }
#endif

    /// <summary>
    /// Used by the StylusPointCollection.ToHimetricArray method
    /// </summary>
    /// <returns></returns>
    Array<int> GetAdditionalData() const
    {
        //return a direct ref
        return _additionalValues;
    }

    /// <summary>
    /// helper used by SPC.Reformat to preserve the pressureFactor
    /// </summary>
    float GetUntruncatedPressureFactor() const
    {
        return _pressureFactor;
    }

    /// <summary>
    /// GetPacketData - returns avalon space packet data with true pressure if it exists
    /// </summary>
    Array<int> GetPacketData() const;

    /// <summary>
    /// helper to determine if a stroke has default pressure
    /// This is used by ISF serialization to not serialize pressure
    /// </summary>
    bool HasDefaultPressure() const;

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
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INPUT_STYLUSPOINT_H
