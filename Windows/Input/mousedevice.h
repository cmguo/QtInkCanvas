#ifndef MOUSEDEVICE_H
#define MOUSEDEVICE_H

#include "Windows/Input/inputdevice.h"
#include "Windows/Input/mousebuttonstate.h"
#include "Windows/routedeventargs.h"

class UIElement;
class MouseDevice;

class MouseEvent : public RoutedEvent
{
public:
    MouseEvent(int type);
    virtual void handle(QEvent &event, QList<RoutedEventHandler> handlers) override;
};

class Mouse
{
public:
    static MouseDevice* PrimaryDevice;

    static MouseEvent MouseDownEvent;

    static MouseEvent MouseMoveEvent;

    static MouseEvent MouseUpEvent;

    static MouseEvent MouseEnterEvent;

    static MouseEvent MouseLeaveEvent;

    static MouseEvent QueryCursorEvent;

    static void UpdateCursor(UIElement* element);

};

class MouseDevice : public InputDevice
{
    Q_OBJECT
public:
    MouseDevice();

    void SetLastPosition(QPointF const & pos);

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

    MouseButtonState XButton1();

    MouseButtonState XButton2();

    UIElement* Captured();

    virtual UIElement* Target();

    virtual PresentationSource* ActiveSource();

    virtual int Id() override;

    virtual QSharedPointer<StylusPointDescription> PointDescription();

    virtual QVector<int> PacketData(QInputEvent& event);

private:
    QSharedPointer<StylusPointDescription> description_;
    QPointF lastPosition_;
};

#endif // MOUSEDEVICE_H
