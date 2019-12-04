#ifndef STYLUSPOINTPROPERTIES_H
#define STYLUSPOINTPROPERTIES_H

#include "Windows/Input/styluspointproperty.h"

// namespace System.Windows.Input

class StylusPointProperties
{
public:
    /// <summary>
    /// X
    /// </summary>
    static StylusPointProperty X;

    /// <summary>
    /// Y
    /// </summary>
    static StylusPointProperty Y;

    /// <summary>
    /// Z
    /// </summary>
    static StylusPointProperty Z;

    /// <summary>
    /// Width
    /// </summary>
    static StylusPointProperty Width;

    /// <summary>
    /// Height
    /// </summary>
    static StylusPointProperty Height;

    /// <summary>
    /// SystemContact
    /// </summary>
    static StylusPointProperty SystemTouch;

    /// <summary>
    /// PacketStatus
    /// </summary>
    static StylusPointProperty PacketStatus;

    /// <summary>
    /// SerialNumber
    /// </summary>
    static StylusPointProperty SerialNumber;

    /// <summary>
    /// NormalPressure
    /// </summary>
    static StylusPointProperty NormalPressure;

    /// <summary>
    /// TangentPressure
    /// </summary>
    static StylusPointProperty TangentPressure;

    /// <summary>
    /// ButtonPressure
    /// </summary>
    static StylusPointProperty ButtonPressure;

    /// <summary>
    /// XTiltOrientation
    /// </summary>
    static StylusPointProperty XTiltOrientation;

    /// <summary>
    /// YTiltOrientation
    /// </summary>
    static StylusPointProperty YTiltOrientation;

    /// <summary>
    /// AzimuthOrientation
    /// </summary>
    static StylusPointProperty AzimuthOrientation;

    /// <summary>
    /// AltitudeOrientation
    /// </summary>
    static StylusPointProperty AltitudeOrientation;

    /// <summary>
    /// TwistOrientation
    /// </summary>
    static StylusPointProperty TwistOrientation;

    /// <summary>
    /// PitchRotation
    /// </summary>
    static StylusPointProperty PitchRotation;

    /// <summary>
    /// RollRotation
    /// </summary>
    static StylusPointProperty RollRotation;

    /// <summary>
    /// YawRotation
    /// </summary>
    static StylusPointProperty YawRotation;

    /// <summary>
    /// TipButton
    /// </summary>
    static StylusPointProperty TipButton;

    /// <summary>
    /// BarrelButton
    /// </summary>
    static StylusPointProperty BarrelButton;

    /// <summary>
    /// SecondaryTipButton
    /// </summary>
    static StylusPointProperty SecondaryTipButton;
};

#endif // STYLUSPOINTPROPERTIES_H
