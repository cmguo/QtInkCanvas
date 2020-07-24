#include "Windows/Input/styluspointpropertyids.h"

INKCANVAS_BEGIN_NAMESPACE

bool StylusPointPropertyIds::IsKnownId(Guid const & Guid)
{
    if (Guid == X ||
        Guid == Y ||
        Guid == Z ||
        Guid == Width ||
        Guid == Height ||
        Guid == SystemTouch ||
        Guid == PacketStatus ||
        Guid == SerialNumber ||
        Guid == NormalPressure ||
        Guid == TangentPressure ||
        Guid == ButtonPressure ||
        Guid == XTiltOrientation ||
        Guid == YTiltOrientation ||
        Guid == AzimuthOrientation ||
        Guid == AltitudeOrientation ||
        Guid == TwistOrientation ||
        Guid == PitchRotation ||
        Guid == RollRotation ||
        Guid == YawRotation ||
        Guid == TipButton ||
        Guid == BarrelButton ||
        Guid == SecondaryTipButton)
    {
        return true;
    }
    return false;
}

char const * StylusPointPropertyIds::GetStringRepresentation(Guid const & Guid)
{
    if (Guid == X)
    {
        return "X";
    }
    if (Guid == Y)
    {
        return "Y";
    }
    if (Guid == Z)
    {
        return "Z";
    }
    if (Guid == Width)
    {
        return "Width";
    }
    if (Guid == Height)
    {
        return "Height";
    }
    if (Guid == SystemTouch)
    {
        return "SystemTouch";
    }
    if (Guid == PacketStatus)
    {
        return "PacketStatus";
    }
    if (Guid == SerialNumber)
    {
        return "SerialNumber";
    }
    if (Guid == NormalPressure)
    {
        return "NormalPressure";
    }
    if (Guid == TangentPressure)
    {
        return "TangentPressure";
    }
    if (Guid == ButtonPressure)
    {
        return "ButtonPressure";
    }
    if (Guid == XTiltOrientation)
    {
        return "XTiltOrientation";
    }
    if (Guid == YTiltOrientation)
    {
        return "YTiltOrientation";
    }
    if (Guid == AzimuthOrientation)
    {
        return "AzimuthOrientation";
    }
    if (Guid == AltitudeOrientation)
    {
        return "AltitudeOrientation";
    }
    if (Guid == TwistOrientation)
    {
        return "TwistOrientation";
    }
    if (Guid == PitchRotation)
    {
        return "PitchRotation";
    }
    if (Guid == RollRotation)
    {
        return "RollRotation";
    }
    if (Guid == AltitudeOrientation)
    {
        return "AltitudeOrientation";
    }
    if (Guid == YawRotation)
    {
        return "YawRotation";
    }
    if (Guid == TipButton)
    {
        return "TipButton";
    }
    if (Guid == BarrelButton)
    {
        return "BarrelButton";
    }
    if (Guid == SecondaryTipButton)
    {
        return "SecondaryTipButton";
    }
    return "Unknown";
}

bool StylusPointPropertyIds::IsKnownButton(Guid const & Guid)
{
    if (Guid == TipButton ||
        Guid == BarrelButton ||
        Guid == SecondaryTipButton)
    {
        return true;
    }
    return false;
}

INKCANVAS_END_NAMESPACE
