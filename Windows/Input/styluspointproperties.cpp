#include "Windows/Input/styluspointproperties.h"

INKCANVAS_BEGIN_NAMESPACE

StylusPointProperty StylusPointProperties::X(StylusPointPropertyIds::X, false);

/// <summary>
/// Y
/// </summary>
StylusPointProperty StylusPointProperties::Y(StylusPointPropertyIds::Y, false);

/// <summary>
/// Z
/// </summary>
StylusPointProperty StylusPointProperties::Z(StylusPointPropertyIds::Z, false);

/// <summary>
/// Width
/// </summary>
StylusPointProperty StylusPointProperties::Width(StylusPointPropertyIds::Width, false);

/// <summary>
/// Height
/// </summary>
StylusPointProperty StylusPointProperties::Height(StylusPointPropertyIds::Height, false);

/// <summary>
/// SystemContact
/// </summary>
StylusPointProperty StylusPointProperties::SystemTouch(StylusPointPropertyIds::SystemTouch, false);

/// <summary>
/// PacketStatus
/// </summary>
StylusPointProperty StylusPointProperties::PacketStatus(StylusPointPropertyIds::PacketStatus, false);

/// <summary>
/// SerialNumber
/// </summary>
StylusPointProperty StylusPointProperties::SerialNumber(StylusPointPropertyIds::SerialNumber, false);

/// <summary>
/// NormalPressure
/// </summary>
StylusPointProperty StylusPointProperties::NormalPressure(StylusPointPropertyIds::NormalPressure, false);

/// <summary>
/// TangentPressure
/// </summary>
StylusPointProperty StylusPointProperties::TangentPressure(StylusPointPropertyIds::TangentPressure, false);

/// <summary>
/// ButtonPressure
/// </summary>
StylusPointProperty StylusPointProperties::ButtonPressure(StylusPointPropertyIds::ButtonPressure, false);

/// <summary>
/// XTiltOrientation
/// </summary>
StylusPointProperty StylusPointProperties::XTiltOrientation(StylusPointPropertyIds::XTiltOrientation, false);

/// <summary>
/// YTiltOrientation
/// </summary>
StylusPointProperty StylusPointProperties::YTiltOrientation(StylusPointPropertyIds::YTiltOrientation, false);

/// <summary>
/// AzimuthOrientation
/// </summary>
StylusPointProperty StylusPointProperties::AzimuthOrientation(StylusPointPropertyIds::AzimuthOrientation, false);

/// <summary>
/// AltitudeOrientation
/// </summary>
StylusPointProperty StylusPointProperties::AltitudeOrientation(StylusPointPropertyIds::AltitudeOrientation, false);

/// <summary>
/// TwistOrientation
/// </summary>
StylusPointProperty StylusPointProperties::TwistOrientation(StylusPointPropertyIds::TwistOrientation, false);

/// <summary>
/// PitchRotation
/// </summary>
StylusPointProperty StylusPointProperties::PitchRotation(StylusPointPropertyIds::PitchRotation, false);

/// <summary>
/// RollRotation
/// </summary>
StylusPointProperty StylusPointProperties::RollRotation(StylusPointPropertyIds::RollRotation, false);

/// <summary>
/// YawRotation
/// </summary>
StylusPointProperty StylusPointProperties::YawRotation(StylusPointPropertyIds::YawRotation, false);

/// <summary>
/// TipButton
/// </summary>
StylusPointProperty StylusPointProperties::TipButton(StylusPointPropertyIds::TipButton, true);

/// <summary>
/// BarrelButton
/// </summary>
StylusPointProperty StylusPointProperties::BarrelButton(StylusPointPropertyIds::BarrelButton, true);

/// <summary>
/// SecondaryTipButton
/// </summary>
StylusPointProperty StylusPointProperties::SecondaryTipButton(StylusPointPropertyIds::SecondaryTipButton, true);

INKCANVAS_END_NAMESPACE
