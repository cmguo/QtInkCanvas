#ifndef MOUSEBUTTONEVENTARGS_H
#define MOUSEBUTTONEVENTARGS_H

#include "Windows/Input/mouseeventargs.h"

INKCANVAS_BEGIN_NAMESPACE

enum class MouseButton
{

    /// <summary>
    ///    The left mouse button.
    /// </summary>
    Left,

    /// <summary>
    ///    The middle mouse button.
    /// </summary>
    Middle,

    /// <summary>
    ///    The right mouse button.
    /// </summary>
    Right,

    /// <summary>
    ///    The fourth mouse button.
    /// </summary>
    XButton1,

    /// <summary>
    ///    The fifth mouse button.
    /// </summary>
    XButton2
};

class MouseButtonUtilities
{
    /// <summary>
    ///     Private placeholder constructor
    /// </summary>
    /// <remarks>
    ///     There is present to supress the autogeneration of a public one, which
    ///     triggers an FxCop violation, as this is an internal class that is never instantiated
    /// </remarks>
private:
    MouseButtonUtilities()
    {
    }

    /// <summary>
    ///     Ensures MouseButton is set to a valid value.
    /// </summary>
    /// <remarks>
    ///     There is a proscription against using Enum.IsDefined().  (it is slow)
    ///     So we manually validate using a switch statement.
    /// </remarks>
    //[FriendAccessAllowed]
public:
    static void Validate(MouseButton button)
    {
        switch(button)
        {
            case MouseButton::Left:
            case MouseButton::Middle:
            case MouseButton::Right:
            case MouseButton::XButton1:
            case MouseButton::XButton2:
                break;
            default:
                throw std::runtime_error("button");
        }
    }

};

// namespace System.Windows.Input

/// <summary>
///     The MouseButtonEventArgs describes the state of a Mouse button.
/// </summary>
class MouseButtonEventArgs : public MouseEventArgs
{
public:
    MouseButtonEventArgs(QGraphicsSceneMouseEvent& event);

    MouseButtonEventArgs(QTouchEvent& event);

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
    MouseButtonEventArgs(MouseDevice* mouse,
                                int timestamp,
                                MouseButton button);

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
    MouseButtonEventArgs(MouseDevice* mouse,
                                int timestamp,
                                MouseButton button,
                                StylusDevice* stylusDevice);

    /// <summary>
    ///     Read-only access to the button being described.
    /// </summary>
    MouseButton ChangedButton()
    {
        return _button;
    }

    /// <summary>
    ///     Read-only access to the button state.
    /// </summary>
    MouseButtonState ButtonState();

    /// <summary>
    ///     Read access to the button click count.
    /// </summary>
    int ClickCount()
    {
        return _count;
    }
    void SetClickCount(int value)
    {
        _count = value;
    }

    /// <summary>
    ///     The mechanism used to call the type-specific handler on the
    ///     target.
    /// </summary>
    /// <param name="genericHandler">
    ///     The generic handler to call in a type-specific way.
    /// </param>
    /// <param name="genericTarget">
    ///     The target to call the handler on.
    /// </param>
    //virtual void InvokeEventHandler(Delegate genericHandler, QObject* genericTarget);

private:
    MouseButton _button;
    int _count;
};

INKCANVAS_END_NAMESPACE

#endif // MOUSEBUTTONEVENTARGS_H
