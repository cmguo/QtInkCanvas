#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QPointF>
#include <QSharedPointer>
#include <QObject>

class Visual;
class PresentationSource;
class RoutedEvent;

/// <summary>
///     Provides the base class for all input devices.
/// </summary>
class InputDevice : public QObject
{
    Q_OBJECT
protected:
    /// <summary>
    ///     Constructs an instance of the InputDevice class.
    /// </summary>
    InputDevice()
    {
        // Only we can create these.
        //
    }

public:
    /// <summary>
    ///     Returns the element that input from this device is sent to.
    /// </summary>
    virtual Visual* Target() = 0;

    /// <summary>
    ///     Returns the PresentationSource that is reporting input for this device.
    /// </summary>
    virtual PresentationSource* ActiveSource() = 0;
};

class StylusPointCollection;
class StylusPointDescription;

class StylusDevice;

class Stylus
{
public:
    static StylusDevice* CurrentStylusDevice();

    static RoutedEvent StylusMoveEvent;
};

class StylusDevice : public InputDevice
{
    Q_OBJECT
public:
    bool InAir();

    bool Inverted();

    QSharedPointer<StylusPointCollection> GetStylusPoints(Visual * widget, QSharedPointer<StylusPointDescription> description);

    Visual* Captured();

};

enum MouseButtonState
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

class UIElement;
class MouseDevice;

class Mouse
{
public:
    static MouseDevice* PrimaryDevice;

    static RoutedEvent MouseDownEvent;

    static RoutedEvent MouseMoveEvent;

    static RoutedEvent MouseUpEvent;

    static RoutedEvent MouseEnterEvent;

    static RoutedEvent MouseLeaveEvent;

    static void UpdateCursor();

};

class MouseDevice : public InputDevice
{
    Q_OBJECT
public:
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

    MouseButtonState XButton1();

    MouseButtonState XButton2();

    Visual* Captured();

};


#endif // INPUTDEVICE_H
