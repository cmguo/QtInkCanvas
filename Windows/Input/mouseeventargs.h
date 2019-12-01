#ifndef MOUSEEVENTARGS_H
#define MOUSEEVENTARGS_H

#include "inputeventargs.h"
#include "inputdevice.h"

class UIElement;

// namespace System.Windows.Input

/// <summary>
///     The MouseEventArgs class provides access to the logical
///     Mouse device for all derived event args.
/// </summary>
class MouseEventArgs : public InputEventArgs
{
public:
    /// <summary>
    ///     Initializes a new instance of the MouseEventArgs class.
    /// </summary>
    /// <param name="mouse">
    ///     The logical Mouse device associated with this event.
    /// </param>
    /// <param name="timestamp">
    ///     The time when the input occured.
    /// </param>
    MouseEventArgs(MouseDevice* mouse, int timestamp);

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
    MouseEventArgs(MouseDevice* mouse, int timestamp, StylusDevice* stylusDevice);

    /// <summary>
    ///     Read-only access to the mouse device associated with this
    ///     event.
    /// </summary>
    MouseDevice* GetMouseDevice()
    {
        return (MouseDevice*) Device();
    }

    /// <summary>
    ///     Read-only access to the stylus Mouse associated with this event.
    /// </summary>
    StylusDevice* GetStylusDevice()
    {
        return _stylusDevice;
    }

    /// <summary>
    ///     Calculates the position of the mouse relative to
    ///     a particular element.
    /// </summary>
    QPointF GetPosition(UIElement* relativeTo);

    /// <summary>
    ///     The state of the left button.
    /// </summary>
    MouseButtonState LeftButton();

    /// <summary>
    ///     The state of the right button.
    /// </summary>
    MouseButtonState RightButton();

    /// <summary>
    ///     The state of the middle button.
    /// </summary>
    MouseButtonState MiddleButton();

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
    //virtual void InvokeEventHandler(RoutedEventHandler genericHandler, object genericTarget);

private:
    StylusDevice* _stylusDevice;
};

#endif // MOUSEEVENTARGS_H
