#ifndef STYLUSPOINTPROPERTYINFO_H
#define STYLUSPOINTPROPERTYINFO_H

#include "Windows/Input/styluspointproperty.h"

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

class StylusPointPropertyInfo : public StylusPointProperty
{
private:
    /// <summary>
    /// Instance data
    /// </summary>
    int                     _min;
    int                     _max;
    float                   _resolution;
    StylusPointPropertyUnit _unit;

    /// <summary>
    /// For a given StylusPointProperty, instantiates a StylusPointPropertyInfo with default values
    /// </summary>
    /// <param name="stylusPointProperty">
public:
    StylusPointPropertyInfo() {}

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

    /// <summary>
    /// Minimum
    /// </summary>
    int Minimum()
    {
        return _min;
    }

    /// <summary>
    /// Maximum
    /// </summary>
    int Maximum()
    {
        return _max;
    }

    /// <summary>
    /// Resolution
    /// </summary>
    float Resolution()
    {
        return _resolution;
    }

    /// <summary>
    /// Unit
    /// </summary>
    StylusPointPropertyUnit Unit()
    {
        return _unit;
    }

    /// <summary>
    /// helper method for comparing compat for two StylusPointPropertyInfos
    /// </summary>
    static bool AreCompatible(StylusPointPropertyInfo const & stylusPointPropertyInfo1, StylusPointPropertyInfo const & stylusPointPropertyInfo2);
};

#endif // STYLUSPOINTPROPERTYINFO_H
