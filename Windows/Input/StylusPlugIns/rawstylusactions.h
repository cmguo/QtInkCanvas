#ifndef RAWSTYLUSACTIONS_H
#define RAWSTYLUSACTIONS_H

#include <QFlags>

/// <summary>
///    The raw actions being reported from the stylus.
/// </summary>
/// <remarks>
///     Note that multiple actions can be reported at once.
/// </remarks>
//[Flags]
enum class RawStylusActions
{
    /// <summary>
    ///     NoAction
    /// </summary>
    None             = 0x000,

    /// <summary>
    ///     The stylus became active in the application.  The application
    ///     may need to refresh its stylus state.
    /// </summary>
    Activate         = 0x001,

    /// <summary>
    ///     The stylus became inactive in the application.  The application
    ///     may need to clear its stylus state.
    /// </summary>
    Deactivate       = 0x002,

    /// <summary>
    ///     The stylus just came in contact with the digitizer
    /// </summary>
    Down             = 0x004,

    /// <summary>
    ///     The stylus just lost contact with the digitizer
    /// </summary>
    Up               = 0x008,

    /// <summary>
    ///     The stylus is sending more data while in contact with the digitizer.
    /// </summary>
    Move             = 0x010,

    /// <summary>
    ///     The stylus is sending more data while hovering in-air over the digitizer.
    /// </summary>
    InAirMove        = 0x020,

    /// <summary>
    ///     The stylus is now in range of the digitizer.
    /// </summary>
    InRange          = 0x040,

    /// <summary>
    ///     The stylus is now out of range of the digitizer.
    /// </summary>
    OutOfRange       = 0x080,

    /// <summary>
    ///     The stylus is now out of range of the digitizer.
    /// </summary>
    SystemGesture    = 0x100,
};

inline RawStylusActions operator | (RawStylusActions lhs, RawStylusActions rhs)
{
    using T = std::underlying_type_t <RawStylusActions>;
    return static_cast<RawStylusActions>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

/// <summary>
/// Internal helper for validating RawStylusActions
/// </summary>
class RawStylusActionsHelper
{
private:
    static const RawStylusActions MaxActions;

    static bool IsValid(RawStylusActions action);
};

#endif // RAWSTYLUSACTIONS_H
