#ifndef WINDOWS_INPUT_STYLUSPOINTPROPERTYINFODEFAULTS_H
#define WINDOWS_INPUT_STYLUSPOINTPROPERTYINFODEFAULTS_H

#include "Windows/Input/styluspointpropertyinfo.h"

// namespace System.Windows.Input
INKCANVAS_BEGIN_NAMESPACE

class StylusPointPropertyInfoDefaults
{
public:
    /// <summary>
    /// X
    /// </summary>
    static StylusPointPropertyInfo X;

    /// <summary>
    /// Y
    /// </summary>
    static StylusPointPropertyInfo Y;

    /// <summary>
    /// Z
    /// </summary>
    static StylusPointPropertyInfo Z;

    /// <summary>
    /// Width
    /// </summary>
    static StylusPointPropertyInfo Width;

    /// <summary>
    /// Height
    /// </summary>
    static StylusPointPropertyInfo Height;

    /// <summary>
    /// SystemTouch
    /// </summary>
    static StylusPointPropertyInfo SystemTouch;

    /// <summary>
    /// PacketStatus

    static StylusPointPropertyInfo PacketStatus;

    /// <summary>
    /// SerialNumber

    static StylusPointPropertyInfo SerialNumber;

    /// <summary>
    /// NormalPressure
    /// </summary>
    static StylusPointPropertyInfo NormalPressure;

    /// <summary>
    /// TangentPressure
    /// </summary>
    static StylusPointPropertyInfo TangentPressure;

    /// <summary>
    /// ButtonPressure
    /// </summary>
    static StylusPointPropertyInfo ButtonPressure;

    /// <summary>
    /// XTiltOrientation
    /// </summary>
    static StylusPointPropertyInfo XTiltOrientation;

    /// <summary>
    /// YTiltOrientation
    /// </summary>
    static StylusPointPropertyInfo YTiltOrientation;

    /// <summary>
    /// AzimuthOrientation
    /// </summary>
    static StylusPointPropertyInfo AzimuthOrientation;

    /// <summary>
    /// AltitudeOrientation
    /// </summary>
    static StylusPointPropertyInfo AltitudeOrientation;

    /// <summary>
    /// TwistOrientation
    /// </summary>
    static StylusPointPropertyInfo TwistOrientation;

    /// <summary>
    /// PitchRotation

    static StylusPointPropertyInfo PitchRotation;

    /// <summary>
    /// RollRotation

    static StylusPointPropertyInfo RollRotation;

    /// <summary>
    /// YawRotation

    static StylusPointPropertyInfo YawRotation;

    /// <summary>
    /// TipButton

    static StylusPointPropertyInfo TipButton;

    /// <summary>
    /// BarrelButton

    static StylusPointPropertyInfo BarrelButton;

    /// <summary>
    /// SecondaryTipButton

    static StylusPointPropertyInfo SecondaryTipButton;

    /// <summary>
    /// Default Value
    /// </summary>
    static StylusPointPropertyInfo DefaultValue;

    /// <summary>
    /// DefaultButton

    static StylusPointPropertyInfo DefaultButton;

    /// <summary>
    /// For a given StylusPointProperty, return the default property info
    /// </summary>
    /// <param name="stylusPointProperty">stylusPointProperty
    /// <returns></returns>
    static StylusPointPropertyInfo GetStylusPointPropertyInfoDefault(StylusPointProperty const & stylusPointProperty);
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INPUT_STYLUSPOINTPROPERTYINFODEFAULTS_H
