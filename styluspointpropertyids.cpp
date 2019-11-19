#include "styluspointpropertyids.h"

bool StylusPointPropertyIds::IsKnownId(QUuid const & QUuid)
{
    if (QUuid == X ||
        QUuid == Y ||
        QUuid == Z ||
        QUuid == Width ||
        QUuid == Height ||
        QUuid == SystemTouch ||
        QUuid == PacketStatus ||
        QUuid == SerialNumber ||
        QUuid == NormalPressure ||
        QUuid == TangentPressure ||
        QUuid == ButtonPressure ||
        QUuid == XTiltOrientation ||
        QUuid == YTiltOrientation ||
        QUuid == AzimuthOrientation ||
        QUuid == AltitudeOrientation ||
        QUuid == TwistOrientation ||
        QUuid == PitchRotation ||
        QUuid == RollRotation ||
        QUuid == YawRotation ||
        QUuid == TipButton ||
        QUuid == BarrelButton ||
        QUuid == SecondaryTipButton)
    {
        return true;
    }
    return false;
}

QString StylusPointPropertyIds::GetStringRepresentation(QUuid const & QUuid)
{
    if (QUuid == X)
    {
        return "X";
    }
    if (QUuid == Y)
    {
        return "Y";
    }
    if (QUuid == Z)
    {
        return "Z";
    }
    if (QUuid == Width)
    {
        return "Width";
    }
    if (QUuid == Height)
    {
        return "Height";
    }
    if (QUuid == SystemTouch)
    {
        return "SystemTouch";
    }
    if (QUuid == PacketStatus)
    {
        return "PacketStatus";
    }
    if (QUuid == SerialNumber)
    {
        return "SerialNumber";
    }
    if (QUuid == NormalPressure)
    {
        return "NormalPressure";
    }
    if (QUuid == TangentPressure)
    {
        return "TangentPressure";
    }
    if (QUuid == ButtonPressure)
    {
        return "ButtonPressure";
    }
    if (QUuid == XTiltOrientation)
    {
        return "XTiltOrientation";
    }
    if (QUuid == YTiltOrientation)
    {
        return "YTiltOrientation";
    }
    if (QUuid == AzimuthOrientation)
    {
        return "AzimuthOrientation";
    }
    if (QUuid == AltitudeOrientation)
    {
        return "AltitudeOrientation";
    }
    if (QUuid == TwistOrientation)
    {
        return "TwistOrientation";
    }
    if (QUuid == PitchRotation)
    {
        return "PitchRotation";
    }
    if (QUuid == RollRotation)
    {
        return "RollRotation";
    }
    if (QUuid == AltitudeOrientation)
    {
        return "AltitudeOrientation";
    }
    if (QUuid == YawRotation)
    {
        return "YawRotation";
    }
    if (QUuid == TipButton)
    {
        return "TipButton";
    }
    if (QUuid == BarrelButton)
    {
        return "BarrelButton";
    }
    if (QUuid == SecondaryTipButton)
    {
        return "SecondaryTipButton";
    }
    return "Unknown";
}

bool StylusPointPropertyIds::IsKnownButton(QUuid const & QUuid)
{
    if (QUuid == TipButton ||
        QUuid == BarrelButton ||
        QUuid == SecondaryTipButton)
    {
        return true;
    }
    return false;
}
