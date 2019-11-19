#ifndef STYLUSPOINTPROPERTYIDS_H
#define STYLUSPOINTPROPERTYIDS_H

#include <QUuid>

class StylusPointPropertyIds
{
public:
    /// <summary>
    /// The x-coordinate in the tablet coordinate space.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid X = {0x598A6A8F, 0x52C0, 0x4BA0, 0x93, 0xAF, 0xAF, 0x35, 0x74, 0x11, 0xA5, 0x61};
    /// <summary>
    /// The y-coordinate in the tablet coordinate space.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid Y= {0xB53F9F75, 0x04E0, 0x4498, 0xA7, 0xEE, 0xC3, 0x0D, 0xBB, 0x5A, 0x90, 0x11};
    /// <summary>
    /// The z-coordinate or distance of the pen tip from the tablet surface.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid Z= {0x735ADB30, 0x0EBB, 0x4788, 0xA0, 0xE4, 0x0F, 0x31, 0x64, 0x90, 0x05, 0x5D};
    /// <summary>
    /// The width value of touch on the tablet surface.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid Width= {0xbaabe94d, 0x2712, 0x48f5, 0xbe, 0x9d, 0x8f, 0x8b, 0x5e, 0xa0, 0x71, 0x1a};
    /// <summary>
    /// The height value of touch on the tablet surface.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid Height= {0xe61858d2, 0xe447, 0x4218, 0x9d, 0x3f, 0x18, 0x86, 0x5c, 0x20, 0x3d, 0xf4};
    /// <summary>
    /// SystemTouch
    /// </summary>
    /// <externalapi>
    static constexpr QUuid SystemTouch= {0xe706c804, 0x57f0, 0x4f00, 0x8a, 0x0c, 0x85, 0x3d, 0x57, 0x78, 0x9b, 0xe9};
    /// <summary>
    /// The current status of the pen pointer.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid PacketStatus= {0x6E0E07BF, 0xAFE7, 0x4CF7, 0x87, 0xD1, 0xAF, 0x64, 0x46, 0x20, 0x84, 0x18};
    /// <summary>
    /// Identifies the packet.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid SerialNumber= {0x78A81B56, 0x0935, 0x4493, 0xBA, 0xAE, 0x00, 0x54, 0x1A, 0x8A, 0x16, 0xC4};
    /// <summary>
    /// Downward pressure of the pen tip on the tablet surface.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid NormalPressure= {0x7307502D, 0xF9F4, 0x4E18, 0xB3, 0xF2, 0x2C, 0xE1, 0xB1, 0xA3, 0x61, 0x0C};
    /// <summary>
    /// Diagonal pressure of the pen tip on the tablet surface.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid TangentPressure= {0x6DA4488B, 0x5244, 0x41EC, 0x90, 0x5B, 0x32, 0xD8, 0x9A, 0xB8, 0x08, 0x09};
    /// <summary>
    /// Pressure on a pressure sensitive button.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid ButtonPressure= {0x8B7FEFC4, 0x96AA, 0x4BFE, 0xAC, 0x26, 0x8A, 0x5F, 0x0B, 0xE0, 0x7B, 0xF5};
    /// <summary>
    /// The x-tilt orientation is the angle between the y,z-plane and the pen and y-axis plane.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid XTiltOrientation= {0xA8D07B3A, 0x8BF0, 0x40B0, 0x95, 0xA9, 0xB8, 0x0A, 0x6B, 0xB7, 0x87, 0xBF};
    /// <summary>
    /// The y-tilt orientation is the angle between the x,z-plane and the pen and x-axis plane.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid YTiltOrientation= {0x0E932389, 0x1D77, 0x43AF, 0xAC, 0x00, 0x5B, 0x95, 0x0D, 0x6D, 0x4B, 0x2D};
    /// <summary>
    /// Clockwise rotation of the pen about the z axis through a full circular range.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid AzimuthOrientation= {0x029123B4, 0x8828, 0x410B, 0xB2, 0x50, 0xA0, 0x53, 0x65, 0x95, 0xE5, 0xDC};
    /// <summary>
    /// Angle between the axis of the pen and the surface of the tablet.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid AltitudeOrientation= {0x82DEC5C7, 0xF6BA, 0x4906, 0x89, 0x4F, 0x66, 0xD6, 0x8D, 0xFC, 0x45, 0x6C};
    /// <summary>
    /// Clockwise rotation of the pen about its own axis.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid TwistOrientation= {0x0D324960, 0x13B2, 0x41E4, 0xAC, 0xE6, 0x7A, 0xE9, 0xD4, 0x3D, 0x2D, 0x3B};
    /// <summary>
    /// Identifies whether the tip is above or below a horizontal line that is perpendicular to the writing surface.  Requires 3D digitizer.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid PitchRotation= {0x7F7E57B7, 0xBE37, 0x4BE1, 0xA3, 0x56, 0x7A, 0x84, 0x16, 0x0E, 0x18, 0x93};
    /// <summary>
    /// Clockwise rotation of the pen about its own axis.  Requires 3D digitizer.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid RollRotation= {0x5D5D5E56, 0x6BA9, 0x4C5B, 0x9F, 0xB0, 0x85, 0x1C, 0x91, 0x71, 0x4E, 0x56};
    /// <summary>
    /// Yaw identifies whether the tip is turning left or right around the center of its horzontal axis (pen is horizontal).  Requires 3D digitizer.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid YawRotation= {0x6A849980, 0x7C3A, 0x45B7, 0xAA, 0x82, 0x90, 0xA2, 0x62, 0x95, 0x0E, 0x89};
    /// <summary>
    /// Identifies the tip button of a stylus.  Used for identifying StylusButtons in StylusPointDescription.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid TipButton= {0x39143d3, 0x78cb, 0x449c, 0xa8, 0xe7, 0x67, 0xd1, 0x88, 0x64, 0xc3, 0x32};
    /// <summary>
    /// Identifies the button on the barrel of a stylus.  Used for identifying StylusButtons in StylusPointDescription.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid BarrelButton= {0xf0720328, 0x663b, 0x418f, 0x85, 0xa6, 0x95, 0x31, 0xae, 0x3e, 0xcd, 0xfa};
    /// <summary>
    /// Identifies the secondary tip barrel button of a stylus.  Used for identifying StylusButtons in StylusPointDescription.
    /// </summary>
    /// <externalapi>
    static constexpr QUuid SecondaryTipButton= {0x67743782, 0xee5, 0x419a, 0xa1, 0x2b, 0x27, 0x3a, 0x9e, 0xc0, 0x8f, 0x3d};

    /// <summary>
    /// Called by the StylusPointProperty constructor.
    /// Any new QUuids in this static class should be added here
    /// </summary>
    /// <param name="QUuid">QUuid
    static bool IsKnownId(QUuid const & QUuid);

    /// <summary>
    /// Called by the StylusPointProperty constructor.
    /// Any new QUuids in this static class should be added here
    /// </summary>
    /// <param name="QUuid">QUuid
    static QString GetStringRepresentation(QUuid const & QUuid);

    /// <summary>
    /// Called by the StylusPointProperty constructor.
    /// Any new button QUuids in this static class should be added here
    /// </summary>
    /// <param name="QUuid">QUuid
    static bool IsKnownButton(QUuid const & QUuid);
};

#endif // STYLUSPOINTPROPERTYIDS_H
