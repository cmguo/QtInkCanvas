#ifndef STYLUSPOINTPROPERTIES_H
#define STYLUSPOINTPROPERTIES_H

#include "Windows/Input/styluspointproperty.h"

// namespace System.Windows.Input
INKCANVAS_BEGIN_NAMESPACE

class StylusPointProperties
{
public:
    /// <summary>
    /// X
    /// </summary>
    static StylusPointProperty const X;

    /// <summary>
    /// Y
    /// </summary>
    static StylusPointProperty const Y;

    /// <summary>
    /// Z
    /// </summary>
    static StylusPointProperty const Z;

    /// <summary>
    /// Width
    /// </summary>
    static StylusPointProperty const Width;

    /// <summary>
    /// Height
    /// </summary>
    static StylusPointProperty const Height;

    /// <summary>
    /// SystemContact
    /// </summary>
    static StylusPointProperty const SystemTouch;

    /// <summary>
    /// PacketStatus
    /// </summary>
    static StylusPointProperty const PacketStatus;

    /// <summary>
    /// SerialNumber
    /// </summary>
    static StylusPointProperty const SerialNumber;

    /// <summary>
    /// NormalPressure
    /// </summary>
    static StylusPointProperty const NormalPressure;

    /// <summary>
    /// TangentPressure
    /// </summary>
    static StylusPointProperty const TangentPressure;

    /// <summary>
    /// ButtonPressure
    /// </summary>
    static StylusPointProperty const ButtonPressure;

    /// <summary>
    /// XTiltOrientation
    /// </summary>
    static StylusPointProperty const XTiltOrientation;

    /// <summary>
    /// YTiltOrientation
    /// </summary>
    static StylusPointProperty const YTiltOrientation;

    /// <summary>
    /// AzimuthOrientation
    /// </summary>
    static StylusPointProperty const AzimuthOrientation;

    /// <summary>
    /// AltitudeOrientation
    /// </summary>
    static StylusPointProperty const AltitudeOrientation;

    /// <summary>
    /// TwistOrientation
    /// </summary>
    static StylusPointProperty const TwistOrientation;

    /// <summary>
    /// PitchRotation
    /// </summary>
    static StylusPointProperty const PitchRotation;

    /// <summary>
    /// RollRotation
    /// </summary>
    static StylusPointProperty const RollRotation;

    /// <summary>
    /// YawRotation
    /// </summary>
    static StylusPointProperty const YawRotation;

    /// <summary>
    /// TipButton
    /// </summary>
    static StylusPointProperty const TipButton;

    /// <summary>
    /// BarrelButton
    /// </summary>
    static StylusPointProperty const BarrelButton;

    /// <summary>
    /// SecondaryTipButton
    /// </summary>
    static StylusPointProperty const SecondaryTipButton;
};

INKCANVAS_END_NAMESPACE

#endif // STYLUSPOINTPROPERTIES_H
