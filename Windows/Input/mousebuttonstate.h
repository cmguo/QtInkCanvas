#ifndef MOUSEBUTTONSTATE_H
#define MOUSEBUTTONSTATE_H

enum class MouseButtonState
{
    /// <summary>
    ///    The button is released.
    /// </summary>
    Released,

    /// <summary>
    ///    The button is pressed.
    /// </summary>
    Pressed,

    // Update the IsValid helper in RawMouseState.cs if this enum changes.
};

#endif // MOUSEBUTTONSTATE_H
