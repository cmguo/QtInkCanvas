#ifndef WINDOWS_INPUT_STYLUSPOINTPROPERTYINFO_H
#define WINDOWS_INPUT_STYLUSPOINTPROPERTYINFO_H

#include "InkCanvas_global.h"
#include "Windows/Input/styluspointproperty.h"

INKCANVAS_BEGIN_NAMESPACE

enum StylusPointPropertyUnit {

    None = 0,

    Inches = 1,

    Centimeters = 2,

    Degrees = 3,

    Radians = 4,

    Seconds = 5,

    Pounds = 6,

    Grams = 7
};

// namespace System.Windows.Input

class StylusPointPropertyInfo
{
private:
    /// <summary>
    /// Instance data
    /// </summary>
    StylusPointProperty const * prop_;
    int                     _min;
    int                     _max;
    float                   _resolution;
    StylusPointPropertyUnit _unit;

    /// <summary>
    /// For a given StylusPointProperty, instantiates a StylusPointPropertyInfo with default values
    /// </summary>
    /// <param name="stylusPointProperty">
public:
    StylusPointPropertyInfo() : prop_(&StylusPointProperty::Empty) {}

    StylusPointPropertyInfo(StylusPointProperty const & stylusPointProperty);

    /// <summary>
    /// StylusPointProperty
    /// </summary>
    /// <param name="stylusPointProperty">
    /// <param name="minimum">minimum
    /// <param name="maximum">maximum
    /// <param name="unit">unit
    /// <param name="resolution">resolution
    StylusPointPropertyInfo(StylusPointProperty const & stylusPointProperty, int minimum, int maximum,
                            StylusPointPropertyUnit unit, float resolution);

    operator StylusPointProperty const & () const
    {
        return *prop_;
    }

    Guid const & Id() const
    {
        return prop_->Id();
    }

    bool IsButton() const
    {
        return prop_->IsButton();
    }

    /// <summary>
    /// Minimum
    /// </summary>
    int Minimum() const
    {
        return _min;
    }

    /// <summary>
    /// Maximum
    /// </summary>
    int Maximum() const
    {
        return _max;
    }

    /// <summary>
    /// Resolution
    /// </summary>
    float Resolution() const
    {
        return _resolution;
    }

    /// <summary>
    /// Unit
    /// </summary>
    StylusPointPropertyUnit Unit() const
    {
        return _unit;
    }

    /// <summary>
    /// helper method for comparing compat for two StylusPointPropertyInfos
    /// </summary>
    static bool AreCompatible(StylusPointPropertyInfo const & stylusPointPropertyInfo1, StylusPointPropertyInfo const & stylusPointPropertyInfo2);
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INPUT_STYLUSPOINTPROPERTYINFO_H
