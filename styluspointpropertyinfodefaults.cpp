#include "styluspointpropertyinfodefaults.h"
#include "styluspointproperties.h"

/// <summary>
/// X
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::X(
        StylusPointProperties::X,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::Centimeters,
                                    1000.0f);

/// <summary>
/// Y
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::Y(
        StylusPointProperties::Y,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::Centimeters,
                                    1000.0f);

/// <summary>
/// Z
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::Z(
        StylusPointProperties::Z,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::Centimeters,
                                    1000.0f);

/// <summary>
/// Width
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::Width(
        StylusPointProperties::Width,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::Centimeters,
                                    1000.0f);

/// <summary>
/// Height
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::Height
        (StylusPointProperties::Height,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::Centimeters,
                                    1000.0f);

/// <summary>
/// SystemTouch
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::SystemTouch
         (StylusPointProperties::SystemTouch,
                                    0,
                                    1,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// PacketStatus

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::PacketStatus
        (StylusPointProperties::PacketStatus,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// SerialNumber

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::SerialNumber
        (StylusPointProperties::SerialNumber,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// NormalPressure
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::NormalPressure
        (StylusPointProperties::NormalPressure,
                                    0,
                                    1023,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// TangentPressure
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::TangentPressure
        (StylusPointProperties::TangentPressure,
                                    0,
                                    1023,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// ButtonPressure
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::ButtonPressure
        (StylusPointProperties::ButtonPressure,
                                    0,
                                    1023,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// XTiltOrientation
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::XTiltOrientation
        (StylusPointProperties::XTiltOrientation,
                                    0,
                                    3600,
                                    StylusPointPropertyUnit::Degrees,
                                    10.0f);

/// <summary>
/// YTiltOrientation
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::YTiltOrientation
        (StylusPointProperties::YTiltOrientation,
                                    0,
                                    3600,
                                    StylusPointPropertyUnit::Degrees,
                                    10.0f);

/// <summary>
/// AzimuthOrientation
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::AzimuthOrientation
        (StylusPointProperties::AzimuthOrientation,
                                    0,
                                    3600,
                                    StylusPointPropertyUnit::Degrees,
                                    10.0f);

/// <summary>
/// AltitudeOrientation
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::AltitudeOrientation
        (StylusPointProperties::AltitudeOrientation,
                                    -900,
                                    900,
                                    StylusPointPropertyUnit::Degrees,
                                    10.0f);

/// <summary>
/// TwistOrientation
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::TwistOrientation
        (StylusPointProperties::TwistOrientation,
                                    0,
                                    3600,
                                    StylusPointPropertyUnit::Degrees,
                                    10.0f);

/// <summary>
/// PitchRotation

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::PitchRotation
        (StylusPointProperties::PitchRotation,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// RollRotation

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::RollRotation
        (StylusPointProperties::RollRotation,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// YawRotation

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::YawRotation
        (StylusPointProperties::YawRotation,
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// TipButton

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::TipButton
        (StylusPointProperties::TipButton,
                                    0,
                                    1,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// BarrelButton

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::BarrelButton
        (StylusPointProperties::BarrelButton,
                                    0,
                                    1,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// SecondaryTipButton

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::SecondaryTipButton
        (StylusPointProperties::SecondaryTipButton,
                                    0,
                                    1,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// Default Value
/// </summary>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::DefaultValue
        (StylusPointProperty(QUuid::createUuid(), false),
                                    INT_MIN,
                                    INT_MAX,
                                    StylusPointPropertyUnit::None,
                                    1.0F);

/// <summary>
/// DefaultButton

StylusPointPropertyInfo StylusPointPropertyInfoDefaults::DefaultButton
        (StylusPointProperty(QUuid::createUuid(), true),
                                    0,
                                    1,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

/// <summary>
/// For a given StylusPointProperty, return the default property info
/// </summary>
/// <param name="stylusPointProperty">stylusPointProperty
/// <returns></returns>
StylusPointPropertyInfo StylusPointPropertyInfoDefaults::GetStylusPointPropertyInfoDefault(StylusPointProperty const & stylusPointProperty)
{
    if (stylusPointProperty.Id() == StylusPointPropertyIds::X)
    {
        return StylusPointPropertyInfoDefaults::X;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::Y)
    {
        return StylusPointPropertyInfoDefaults::Y;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::Z)
    {
        return StylusPointPropertyInfoDefaults::Z;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::Width)
    {
        return StylusPointPropertyInfoDefaults::Width;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::Height)
    {
        return StylusPointPropertyInfoDefaults::Height;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::SystemTouch)
    {
        return StylusPointPropertyInfoDefaults::SystemTouch;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::PacketStatus)
    {
        return StylusPointPropertyInfoDefaults::PacketStatus;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::SerialNumber)
    {
        return StylusPointPropertyInfoDefaults::SerialNumber;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::NormalPressure)
    {
        return StylusPointPropertyInfoDefaults::NormalPressure;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::TangentPressure)
    {
        return StylusPointPropertyInfoDefaults::TangentPressure;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::ButtonPressure)
    {
        return StylusPointPropertyInfoDefaults::ButtonPressure;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::XTiltOrientation)
    {
        return StylusPointPropertyInfoDefaults::XTiltOrientation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::YTiltOrientation)
    {
        return StylusPointPropertyInfoDefaults::YTiltOrientation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::AzimuthOrientation)
    {
        return StylusPointPropertyInfoDefaults::AzimuthOrientation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::AltitudeOrientation)
    {
        return StylusPointPropertyInfoDefaults::AltitudeOrientation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::TwistOrientation)
    {
        return StylusPointPropertyInfoDefaults::TwistOrientation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::PitchRotation)
    {
        return StylusPointPropertyInfoDefaults::PitchRotation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::RollRotation)
    {
        return StylusPointPropertyInfoDefaults::RollRotation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::YawRotation)
    {
        return StylusPointPropertyInfoDefaults::YawRotation;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::TipButton)
    {
        return StylusPointPropertyInfoDefaults::TipButton;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::BarrelButton)
    {
        return StylusPointPropertyInfoDefaults::BarrelButton;
    }
    if (stylusPointProperty.Id() == StylusPointPropertyIds::SecondaryTipButton)
    {
        return StylusPointPropertyInfoDefaults::SecondaryTipButton;
    }

    //
    // return a default
    //
    if (stylusPointProperty.IsButton())
    {
        return StylusPointPropertyInfoDefaults::DefaultButton;
    }
    return StylusPointPropertyInfoDefaults::DefaultValue;
}
