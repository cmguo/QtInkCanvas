#ifndef STYLUSPOINTPROPERTYINFO_H
#define STYLUSPOINTPROPERTYINFO_H

#include "styluspointproperty.h"

enum StylusPointPropertyUnit {
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 未指定属性的度量单位。
    None = 0,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性以英寸为单位。
    Inches = 1,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性以厘米为单位。
    Centimeters = 2,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性以度为单位。
    Degrees = 3,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性以弧度为单位。
    Radians = 4,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性以秒为单位。
    Seconds = 5,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性以磅为单位。
    Pounds = 6,
    //
    // 摘要:
    //     指示 System.Windows.Input.StylusPoint 属性的单位元语法。
    Grams = 7
};

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
    StylusPointPropertyInfo() {};

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
