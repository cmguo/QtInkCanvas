#include "Windows/Input/mouseeventargs.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/stylusdevice.h"
#include <stdexcept>

INKCANVAS_BEGIN_NAMESPACE

MouseEventArgs::MouseEventArgs(QGraphicsSceneMouseEvent &)
    : MouseEventArgs(Mouse::PrimaryDevice, Mouse::GetTimestamp())
{
    _stylusDevice = nullptr;
}

MouseEventArgs::MouseEventArgs(QTouchEvent &event)
    : MouseEventArgs(Mouse::PrimaryDevice, event.timestamp())
{
    _stylusDevice = Stylus::GetDevice(event.device());
}

/// <summary>
///     Initializes a new instance of the MouseEventArgs class.
/// </summary>
/// <param name="mouse">
///     The logical Mouse device associated with this event.
/// </param>
/// <param name="timestamp">
///     The time when the input occured.
/// </param>
MouseEventArgs::MouseEventArgs(MouseDevice* mouse, int timestamp)
    : InputEventArgs(mouse, timestamp)
{
    if( mouse == nullptr )
    {
        throw std::runtime_error("mouse");
    }
    _stylusDevice = nullptr;
}

/// <summary>
///     Initializes a new instance of the MouseEventArgs class.
/// </summary>
/// <param name="mouse">
///     The logical Mouse device associated with this event.
/// </param>
/// <param name="timestamp">
///     The time when the input occured.
/// </param>
/// <param name="stylusDevice">
///     The stylus device that was involved with this event.
/// </param>
MouseEventArgs::MouseEventArgs(MouseDevice* mouse, int timestamp, StylusDevice* stylusDevice)
    : InputEventArgs(mouse, timestamp)
{
    if( mouse == nullptr )
    {
        throw std::runtime_error("mouse");
    }
    _stylusDevice = stylusDevice;
}


/// <summary>
///     Read-only access to the mouse device associated with this
///     event.
/// </summary>
MouseDevice* MouseEventArgs::GetMouseDevice()
{
    return (MouseDevice*) Device();
}

/// <summary>
///     Calculates the position of the mouse relative to
///     a particular element.
/// </summary>
QPointF MouseEventArgs::GetPosition(Visual* relativeTo)
{
    return GetMouseDevice()->GetPosition(relativeTo);
}

/// <summary>
///     The state of the left button.
/// </summary>
MouseButtonState MouseEventArgs::LeftButton()
{
    return GetMouseDevice()->LeftButton();
}

/// <summary>
///     The state of the right button.
/// </summary>
MouseButtonState MouseEventArgs::RightButton()
{
    return GetMouseDevice()->RightButton();
}

/// <summary>
///     The state of the middle button.
/// </summary>
MouseButtonState MouseEventArgs::MiddleButton()
{
    return GetMouseDevice()->MiddleButton();
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
/*
void MouseEventArgs::InvokeEventHandler(Delegate genericHandler, object genericTarget)
{
    MouseEventHandler handler = (MouseEventHandler) genericHandler;
    handler(genericTarget, this);
}
*/

INKCANVAS_END_NAMESPACE
