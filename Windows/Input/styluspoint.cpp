#include "Windows/Input/styluspoint.h"
#include "Internal/debug.h"
#include "double.h"
#include "single.h"
#include <stdexcept>

INKCANVAS_BEGIN_NAMESPACE

StylusPoint::StylusPoint(double x, double y)
    : StylusPoint(x, y, DefaultPressure, nullptr, Array<int>(), false, false)
{
}

/// <summary>
/// StylusPoint
/// </summary>
/// <param name="x">x
/// <param name="y">y
/// <param name="pressureFactor">pressureFactor
StylusPoint::StylusPoint(double x, double y, float pressureFactor)
    : StylusPoint(x, y, pressureFactor, nullptr, Array<int>(), false, true)
{
}


/// <summary>
/// StylusPoint
/// </summary>
/// <param name="x">x
/// <param name="y">y
/// <param name="pressureFactor">pressureFactor
/// <param name="stylusPointDescription">stylusPointDescription
/// <param name="additionalValues">additionalValues
StylusPoint::StylusPoint(double x, double y, float pressureFactor,
                         SharedPointer<StylusPointDescription> stylusPointDescription, Array<int> const & additionalValues)
    : StylusPoint(x, y, pressureFactor, stylusPointDescription, additionalValues, true, true)
{
}

/// <summary>
/// ctor
/// </summary>
StylusPoint::StylusPoint(
    double x,
    double y,
    float pressureFactor,
    SharedPointer<StylusPointDescription> stylusPointDescription,
    Array<int> const & additionalValues,
    bool validateAdditionalData,
    bool validatePressureFactor)
{
    if (Double::IsNaN(x))
    {
        throw std::runtime_error("x");
    }
    if (Double::IsNaN(y))
    {
        throw std::runtime_error("y");
    }


    //we don't validate pressure when called by StylusPointDescription.Reformat
    if (validatePressureFactor &&
        (pressureFactor == Single::NaN || pressureFactor < 0.0f || pressureFactor > 1.0f))
    {
        throw std::runtime_error("pressureFactor");
    }
    //
    // only accept values between MaxXY and MinXY
    // we don't throw when passed a value outside of that range, we just silently trunctate
    //
    _x = GetClampedXYValue(x);
    _y = GetClampedXYValue(y);
    _stylusPointDescription = stylusPointDescription;
    _additionalValues = additionalValues;
    _pressureFactor = pressureFactor;

    if (validateAdditionalData)
    {

        //
        // called from the verbose ctor
        //
        if (nullptr == stylusPointDescription)
        {
            throw std::runtime_error("stylusPointDescription");
        }


        //if (additionalValues != nullptr)
        {
            List<StylusPointPropertyInfo> properties
                = stylusPointDescription->GetStylusPointProperties();

            int expectedAdditionalValues = properties.Count() - StylusPointDescription::RequiredCountOfProperties; //for x, y, pressure
            if (additionalValues.Length() != expectedAdditionalValues)
            {
                throw std::runtime_error("additionalValues");
            }

            //
            // any buttons passed in must each be in their own int.  We need to
            // pack them all into one int here
            //
            Array<int> newAdditionalValues(stylusPointDescription->GetExpectedAdditionalDataCount());

            _additionalValues = newAdditionalValues;
            for (int i = StylusPointDescription::RequiredCountOfProperties, j = 0; i < properties.Count(); i++, j++)
            {
                //
                // use SetPropertyValue, it validates buttons, but does not copy the
                // int[] on writes (since we pass the bool flag)
                //
                SetPropertyValue(properties[i], additionalValues[j], false/*copy on write*/);
            }
        }
    }
}



void StylusPoint::SetX(double value)
{
    if (Double::IsNaN(value))
    {
        throw std::runtime_error("X");
    }
    //
    // only accept values between MaxXY and MinXY
    // we don't throw when passed a value outside of that range, we just silently trunctate
    //
    _x = GetClampedXYValue(value);
}


void StylusPoint::SetY(double value)
{
    if (Double::IsNaN(value))
    {
        throw std::runtime_error("Y");
    }
    //
    // only accept values between MaxXY and MinXY
    // we don't throw when passed a value outside of that range, we just silently trunctate
    //
    _y = GetClampedXYValue(value);
}

/// <summary>
/// PressureFactor.  A value between 0.0 (no pressure) and 1.0 (max pressure)
/// </summary>
float StylusPoint::PressureFactor() const
{
    //
    // note that pressure can be stored a > 1 or < 0.
    // we need to clamp if this is the case
    //
    if (_pressureFactor > 1.0f)
    {
        return 1.0f;
    }
    if (_pressureFactor < 0.0f)
    {
        return 0.0f;
    }
    return _pressureFactor;
}

void StylusPoint::SetPressureFactor(float value)
{
    if (value < 0.0f || value > 1.0f)
    {
        throw std::runtime_error("PressureFactor");
    }
    _pressureFactor = value;
}

/// <summary>
/// Describes the properties this StylusPoint contains
/// </summary>
SharedPointer<StylusPointDescription> StylusPoint::Description() const
{
    if (nullptr == _stylusPointDescription)
    {
        // this can happen when you call new StylusPoint()
        // a few of the ctor's lazy init this as well
        _stylusPointDescription.reset(new StylusPointDescription());
    }
    return _stylusPointDescription;
}

void StylusPoint::SetDescription(SharedPointer<StylusPointDescription> value)
{
    //
    // called by StylusPointCollection.Add / Set
    // to replace the StylusPoint.Description with the collections.
    //
    Debug::Assert(value != nullptr &&
        StylusPointDescription::AreCompatible(value, Description()));

    _stylusPointDescription = value;
}

/// <summary>
/// Returns true if this StylusPoint supports the specified property
/// </summary>
/// <param name="stylusPointProperty">The StylusPointProperty to see if this StylusPoint supports
bool StylusPoint::HasProperty(StylusPointProperty const & stylusPointProperty) const
{
    return Description()->HasProperty(stylusPointProperty);
}

/// <summary>
/// Provides read access to all stylus properties
/// </summary>
/// <param name="stylusPointProperty">The StylusPointPropertyIds of the property to retrieve
int StylusPoint::GetPropertyValue(StylusPointProperty const & stylusPointProperty) const
{
    if (stylusPointProperty.Id() == StylusPointPropertyIds::X)
    {
        return static_cast<int>(_x);
    }
    else if (stylusPointProperty.Id() == StylusPointPropertyIds::Y)
    {
        return static_cast<int>(_y);
    }
    else if (stylusPointProperty.Id() == StylusPointPropertyIds::NormalPressure)
    {
        StylusPointPropertyInfo info =
            Description()->GetPropertyInfo(StylusPointProperties::NormalPressure);

        int max = info.Maximum();
        return static_cast<int>(_pressureFactor * max);
    }
    else
    {
        int propertyIndex = Description()->GetPropertyIndex(stylusPointProperty.Id());
        if (-1 == propertyIndex)
        {
            throw std::runtime_error("stylusPointProperty");
        }
        if (stylusPointProperty.IsButton())
        {
            //
            // we get button data from a single int in the array
            //
            int buttonData = _additionalValues[_additionalValues.Length() - 1];
            int buttonBitPosition = Description()->GetButtonBitPosition(stylusPointProperty);
            int bit = 1 << buttonBitPosition;
            if ((buttonData & bit) != 0)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return _additionalValues[propertyIndex - 3];
        }
    }
}

/// <summary>
/// Allows supported properties to be set
/// </summary>
/// <param name="stylusPointProperty">The property to set, it must exist on this StylusPoint
/// <param name="value">value
void StylusPoint::SetPropertyValue(StylusPointProperty const & stylusPointProperty, int value)
{
    SetPropertyValue(stylusPointProperty, value, true);
}

/// <summary>
/// Optimization that lets the ctor call setvalue repeatly without causing a copy of the int[]
/// </summary>
/// <param name="stylusPointProperty">stylusPointProperty
/// <param name="value">value
/// <param name="copyBeforeWrite">
void StylusPoint::SetPropertyValue(StylusPointProperty const & stylusPointProperty, int value, bool copyBeforeWrite)
{
    if (stylusPointProperty.Id() == StylusPointPropertyIds::X)
    {
        double dVal = value;
        //
        // only accept values between MaxXY and MinXY
        // we don't throw when passed a value outside of that range, we just silently trunctate
        //
        _x = GetClampedXYValue(dVal);
    }
    else if (stylusPointProperty.Id() == StylusPointPropertyIds::Y)
    {
        double dVal = value;
        //
        // only accept values between MaxXY and MinXY
        // we don't throw when passed a value outside of that range, we just silently trunctate
        //
        _y = GetClampedXYValue(dVal);
    }
    else if (stylusPointProperty.Id() == StylusPointPropertyIds::NormalPressure)
    {
        StylusPointPropertyInfo info =
            Description()->GetPropertyInfo(StylusPointProperties::NormalPressure);

        int min = info.Minimum();
        int max = info.Maximum();
        if (max == 0)
        {
            _pressureFactor = 0.0f;
        }
        else
        {
            _pressureFactor = (static_cast<float>(min + value) / max);
        }
    }
    else
    {
        int propertyIndex = Description()->GetPropertyIndex(stylusPointProperty.Id());
        if (-1 == propertyIndex)
        {
            throw std::runtime_error("propertyId");
        }
        if (stylusPointProperty.IsButton())
        {
            if (value < 0 || value > 1)
            {
                throw std::runtime_error("value");
            }

            if (copyBeforeWrite)
            {
                //CopyAdditionalData();
            }

            //
            // we get button data from a single int in the array
            //
            int buttonData = _additionalValues[_additionalValues.Length() - 1];
            int buttonBitPosition = Description()->GetButtonBitPosition(stylusPointProperty);
            int bit = 1 << buttonBitPosition;
            if (value == 0)
            {
                //turn the bit off
                buttonData &= ~bit;
            }
            else
            {
                //turn the bit on
                buttonData |= bit;
            }
            _additionalValues[_additionalValues.Length() - 1] = buttonData;
        }
        else
        {
            if (copyBeforeWrite)
            {
                //CopyAdditionalData();
            }
            _additionalValues[propertyIndex - 3] = value;
        }
    }
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
bool StylusPoint::Equals(StylusPoint const & stylusPoint1, StylusPoint const & stylusPoint2)
{
    //
    // do the cheap comparison first
    //
    bool membersEqual =
        stylusPoint1._x == stylusPoint2._x &&
        stylusPoint1._y == stylusPoint2._y &&
        stylusPoint1._pressureFactor == stylusPoint2._pressureFactor;

    if (!membersEqual)
    {
        return false;
    }

    //
    // ok, the members are equal.  compare the description and then additional data
    //
    if ((stylusPoint1.Description() == stylusPoint2.Description()) ||
        StylusPointDescription::AreCompatible(stylusPoint1.Description(), stylusPoint2.Description()))
    {
        //
        // descriptions match and there are equal numbers of additional values
        // let's check the values
        //
        for (int x = 0; x < stylusPoint1._additionalValues.Length(); x++)
        {
            if (stylusPoint1._additionalValues[x] != stylusPoint2._additionalValues[x])
            {
                return false;
            }
        }

        //
        // Ok, ok already, we're equal
        //
        return true;
    }

    return false;
}

/// <summary>
/// Returns the HashCode for this StylusPoint
/// </summary>
/// <returns>
/// int - the HashCode for this StylusPoint
/// </returns>
int StylusPoint::GetHashCode() const
{
#ifdef INKCANVAS_QT
    int hash =
        ::qHash(_x) ^
        ::qHash(_y) ^
        ::qHash(_pressureFactor);
#else
    int hash =
            static_cast<int>(_x) ^
            static_cast<int>(_y) ^
            static_cast<int>(_pressureFactor);
#endif

    if (_stylusPointDescription != nullptr)
    {
        //hash ^= _stylusPointDescription->GetHashCode();
    }

    //if (_additionalValues != nullptr)
    {
        for (int x = 0; x < _additionalValues.Length(); x++)
        {
            hash ^= _additionalValues[x]; //don't call GetHashCode on integers, it just returns the int
        }
    }

    return hash;
}


/// <summary>
/// GetPacketData - returns avalon space packet data with true pressure if it exists
/// </summary>
Array<int> StylusPoint::GetPacketData() const
{
    int count = 2; //x, y
    //if (_additionalValues != null)
    {
        count += _additionalValues.Length();
    }
    if (Description()->ContainsTruePressure())
    {
        count++;
    }
    Array<int> data(count);
    data[0] = static_cast<int>(_x);
    data[1] = static_cast<int>(_y);
    int startIndex = 2;
    if (Description()->ContainsTruePressure())
    {
        startIndex = 3;
        data[2] = GetPropertyValue(StylusPointProperties::NormalPressure);
    }
    //if (_additionalValues != null)
    {
        for (int x = 0; x < _additionalValues.Length(); x++)
        {
            data[x + startIndex] = _additionalValues[x];
        }
    }
    return data;
}

/// <summary>
/// helper to determine if a stroke has default pressure
/// This is used by ISF serialization to not serialize pressure
/// </summary>
bool StylusPoint::HasDefaultPressure() const
{
    return _pressureFactor == DefaultPressure;
}

/// <summary>
/// Used by the SetPropertyData to make a copy of the data
/// before modifying it.  This is required so that we don't
/// have two StylusPoint's sharing the same int[]
/// which can happen when you call: StylusPoint p = otherStylusPoint
/// because the CLR just does a memberwise copy
/// </summary>
/// <returns></returns>
void StylusPoint::CopyAdditionalData()
{
    //if (null != _additionalValues)
    {
        Array<int> newData(_additionalValues.Length());
        for (int x = 0; x < _additionalValues.Length(); x++)
        {
            newData[x] = _additionalValues[x];
        }

        _additionalValues = newData;
    }
}

/// <summary>
/// helper that returns a double clamped to MaxXY or MinXY
/// We only accept values in this range to support ISF serialization
/// </summary>
double StylusPoint::GetClampedXYValue(double xyValue)
{
    if (xyValue > MaxXY)
    {
        return MaxXY;
    }
    if (xyValue < MinXY)
    {
        return MinXY;
    }

    return xyValue;
}

INKCANVAS_END_NAMESPACE
