#include "Windows/Input/styluspointproperties.h"

INKCANVAS_BEGIN_NAMESPACE

StylusPointProperty const StylusPointProperties::X(StylusPointPropertyIds::X, false);

/// <summary>
/// Y
/// </summary>
StylusPointProperty const StylusPointProperties::Y(StylusPointPropertyIds::Y, false);

/// <summary>
/// Z
/// </summary>
StylusPointProperty const StylusPointProperties::Z(StylusPointPropertyIds::Z, false);

/// <summary>
/// Width
/// </summary>
StylusPointProperty const StylusPointProperties::Width(StylusPointPropertyIds::Width, false);

/// <summary>
/// Height
/// </summary>
StylusPointProperty const StylusPointProperties::Height(StylusPointPropertyIds::Height, false);

/// <summary>
/// SystemContact
/// </summary>
StylusPointProperty const StylusPointProperties::SystemTouch(StylusPointPropertyIds::SystemTouch, false);

/// <summary>
/// PacketStatus
/// </summary>
StylusPointProperty const StylusPointProperties::PacketStatus(StylusPointPropertyIds::PacketStatus, false);

/// <summary>
/// SerialNumber
/// </summary>
StylusPointProperty const StylusPointProperties::SerialNumber(StylusPointPropertyIds::SerialNumber, false);

/// <summary>
/// NormalPressure
/// </summary>
StylusPointProperty const StylusPointProperties::NormalPressure(StylusPointPropertyIds::NormalPressure, false);

/// <summary>
/// TangentPressure
/// </summary>
StylusPointProperty const StylusPointProperties::TangentPressure(StylusPointPropertyIds::TangentPressure, false);

/// <summary>
/// ButtonPressure
/// </summary>
StylusPointProperty const StylusPointProperties::ButtonPressure(StylusPointPropertyIds::ButtonPressure, false);

/// <summary>
/// XTiltOrientation
/// </summary>
StylusPointProperty const StylusPointProperties::XTiltOrientation(StylusPointPropertyIds::XTiltOrientation, false);

/// <summary>
/// YTiltOrientation
/// </summary>
StylusPointProperty const StylusPointProperties::YTiltOrientation(StylusPointPropertyIds::YTiltOrientation, false);

/// <summary>
/// AzimuthOrientation
/// </summary>
StylusPointProperty const StylusPointProperties::AzimuthOrientation(StylusPointPropertyIds::AzimuthOrientation, false);

/// <summary>
/// AltitudeOrientation
/// </summary>
StylusPointProperty const StylusPointProperties::AltitudeOrientation(StylusPointPropertyIds::AltitudeOrientation, false);

/// <summary>
/// TwistOrientation
/// </summary>
StylusPointProperty const StylusPointProperties::TwistOrientation(StylusPointPropertyIds::TwistOrientation, false);

/// <summary>
/// PitchRotation
/// </summary>
StylusPointProperty const StylusPointProperties::PitchRotation(StylusPointPropertyIds::PitchRotation, false);

/// <summary>
/// RollRotation
/// </summary>
StylusPointProperty const StylusPointProperties::RollRotation(StylusPointPropertyIds::RollRotation, false);

/// <summary>
/// YawRotation
/// </summary>
StylusPointProperty const StylusPointProperties::YawRotation(StylusPointPropertyIds::YawRotation, false);

/// <summary>
/// TipButton
/// </summary>
StylusPointProperty const StylusPointProperties::TipButton(StylusPointPropertyIds::TipButton, true);

/// <summary>
/// BarrelButton
/// </summary>
StylusPointProperty const StylusPointProperties::BarrelButton(StylusPointPropertyIds::BarrelButton, true);

/// <summary>
/// SecondaryTipButton
/// </summary>
StylusPointProperty const StylusPointProperties::SecondaryTipButton(StylusPointPropertyIds::SecondaryTipButton, true);

INKCANVAS_END_NAMESPACE
