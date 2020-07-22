#ifndef STYLUSPOINTPROPERTYIDS_H
#define STYLUSPOINTPROPERTYIDS_H

#include "InkCanvas_global.h"

#include "guid.h"

// namespace System.Windows.Input
INKCANVAS_BEGIN_NAMESPACE

class StylusPointPropertyIds
{
public:
    /// <summary>
    /// The x-coordinate in the tablet coordinate space.
    /// </summary>
    /// <externalapi>
    static const Guid X ;
    /// <summary>
    /// The y-coordinate in the tablet coordinate space.
    /// </summary>
    /// <externalapi>
    static const Guid Y;
    /// <summary>
    /// The z-coordinate or distance of the pen tip from the tablet surface.
    /// </summary>
    /// <externalapi>
    static const Guid Z;
    /// <summary>
    /// The width value of touch on the tablet surface.
    /// </summary>
    /// <externalapi>
    static const Guid Width;
    /// <summary>
    /// The height value of touch on the tablet surface.
    /// </summary>
    /// <externalapi>
    static const Guid Height;
    /// <summary>
    /// SystemTouch
    /// </summary>
    /// <externalapi>
    static const Guid SystemTouch;
    /// <summary>
    /// The current status of the pen pointer.
    /// </summary>
    /// <externalapi>
    static const Guid PacketStatus;
    /// <summary>
    /// Identifies the packet.
    /// </summary>
    /// <externalapi>
    static const Guid SerialNumber;
    /// <summary>
    /// Downward pressure of the pen tip on the tablet surface.
    /// </summary>
    /// <externalapi>
    static const Guid NormalPressure;
    /// <summary>
    /// Diagonal pressure of the pen tip on the tablet surface.
    /// </summary>
    /// <externalapi>
    static const Guid TangentPressure;
    /// <summary>
    /// Pressure on a pressure sensitive button.
    /// </summary>
    /// <externalapi>
    static const Guid ButtonPressure;
    /// <summary>
    /// The x-tilt orientation is the angle between the y,z-plane and the pen and y-axis plane.
    /// </summary>
    /// <externalapi>
    static const Guid XTiltOrientation;
    /// <summary>
    /// The y-tilt orientation is the angle between the x,z-plane and the pen and x-axis plane.
    /// </summary>
    /// <externalapi>
    static const Guid YTiltOrientation;
    /// <summary>
    /// Clockwise rotation of the pen about the z axis through a full circular range.
    /// </summary>
    /// <externalapi>
    static const Guid AzimuthOrientation;
    /// <summary>
    /// Angle between the axis of the pen and the surface of the tablet.
    /// </summary>
    /// <externalapi>
    static const Guid AltitudeOrientation;
    /// <summary>
    /// Clockwise rotation of the pen about its own axis.
    /// </summary>
    /// <externalapi>
    static const Guid TwistOrientation;
    /// <summary>
    /// Identifies whether the tip is above or below a horizontal line that is perpendicular to the writing surface.  Requires 3D digitizer.
    /// </summary>
    /// <externalapi>
    static const Guid PitchRotation;
    /// <summary>
    /// Clockwise rotation of the pen about its own axis.  Requires 3D digitizer.
    /// </summary>
    /// <externalapi>
    static const Guid RollRotation;
    /// <summary>
    /// Yaw identifies whether the tip is turning left or right around the center of its horzontal axis (pen is horizontal).  Requires 3D digitizer.
    /// </summary>
    /// <externalapi>
    static const Guid YawRotation;
    /// <summary>
    /// Identifies the tip button of a stylus.  Used for identifying StylusButtons in StylusPointDescription.
    /// </summary>
    /// <externalapi>
    static const Guid TipButton;
    /// <summary>
    /// Identifies the button on the barrel of a stylus.  Used for identifying StylusButtons in StylusPointDescription.
    /// </summary>
    /// <externalapi>
    static const Guid BarrelButton;
    /// <summary>
    /// Identifies the secondary tip barrel button of a stylus.  Used for identifying StylusButtons in StylusPointDescription.
    /// </summary>
    /// <externalapi>
    static const Guid SecondaryTipButton;

    /// <summary>
    /// Called by the StylusPointProperty constructor.
    /// Any new Guids in this static class should be added here
    /// </summary>
    /// <param name="Guid">Guid
    static bool IsKnownId(Guid const & Guid);

    /// <summary>
    /// Called by the StylusPointProperty constructor.
    /// Any new Guids in this static class should be added here
    /// </summary>
    /// <param name="Guid">Guid
    static char const * GetStringRepresentation(Guid const & Guid);

    /// <summary>
    /// Called by the StylusPointProperty constructor.
    /// Any new button Guids in this static class should be added here
    /// </summary>
    /// <param name="Guid">Guid
    static bool IsKnownButton(Guid const & Guid);
};

INKCANVAS_END_NAMESPACE

#endif // STYLUSPOINTPROPERTYIDS_H
