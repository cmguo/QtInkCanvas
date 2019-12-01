#include "Windows/Input/mousebuttoneventargs.h"

/// <summary>
///     Initializes a new instance of the MouseButtonEventArgs class.
/// </summary>
/// <param name="mouse">
///     The logical Mouse device associated with this event.
/// </param>
/// <param name="timestamp">
///     The time when the input occured.
/// </param>
/// <param name="button">
///     The mouse button whose state is being described.
/// </param>
MouseButtonEventArgs::MouseButtonEventArgs(MouseDevice* mouse,
                            int timestamp,
                            MouseButton button)
    : MouseEventArgs(mouse, timestamp)
{
    MouseButtonUtilities::Validate(button);

    _button = button;
    _count = 1;
}

/// <summary>
///     Initializes a new instance of the MouseButtonEventArgs class.
/// </summary>
/// <param name="mouse">
///     The logical Mouse device associated with this event.
/// </param>
/// <param name="timestamp">
///     The time when the input occured.
/// </param>
/// <param name="button">
///     The Mouse button whose state is being described.
/// </param>
/// <param name="stylusDevice">
///     The stylus device that was involved with this event.
/// </param>
MouseButtonEventArgs::MouseButtonEventArgs(MouseDevice* mouse,
                            int timestamp,
                            MouseButton button,
                            StylusDevice* stylusDevice)
    : MouseEventArgs(mouse, timestamp, stylusDevice)
{
    MouseButtonUtilities::Validate(button);

    _button = button;
    _count = 1;
}


/// <summary>
///     Read-only access to the button state.
/// </summary>
MouseButtonState MouseButtonEventArgs::ButtonState()
{
    MouseButtonState state = MouseButtonState::Released;

    switch(_button)
    {
        case MouseButton::Left:
            state = GetMouseDevice()->LeftButton();
            break;

        case MouseButton::Right:
            state = GetMouseDevice()->RightButton();
            break;

        case MouseButton::Middle:
            state = GetMouseDevice()->MiddleButton();
            break;

        case MouseButton::XButton1:
            state = GetMouseDevice()->XButton1();
            break;

        case MouseButton::XButton2:
            state = GetMouseDevice()->XButton2();
            break;
    }

    return state;
}

