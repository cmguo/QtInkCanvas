#ifndef MOUSEEVENTARGS_H
#define MOUSEEVENTARGS_H

#include "Windows/Input/inputeventargs.h"
#include "Windows/Input/mousebuttonstate.h"

class QGraphicsSceneMouseEvent;

INKCANVAS_BEGIN_NAMESPACE

class Visual;
class MouseDevice;
class StylusDevice;

// namespace System.Windows.Input

/// <summary>
///     The MouseEventArgs class provides access to the logical
///     Mouse device for all derived event args.
/// </summary>
class MouseEventArgs : public InputEventArgs
{
public:
    MouseEventArgs(QGraphicsSceneMouseEvent &event);

    MouseEventArgs(QTouchEvent &event);

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
    MouseDevice* GetMouseDevice();

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
    QPointF GetPosition(Visual* relativeTo);

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

INKCANVAS_END_NAMESPACE

#endif // MOUSEEVENTARGS_H
