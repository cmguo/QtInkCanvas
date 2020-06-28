#ifndef MOUSEDEVICE_H
#define MOUSEDEVICE_H

#include "Windows/Input/inputdevice.h"
#include "Windows/Input/mousebuttonstate.h"
#include "Windows/routedeventargs.h"
#include "Windows/point.h"

class QGraphicsSceneMouseEvent;

INKCANVAS_BEGIN_NAMESPACE

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

    static MouseEvent MouseHoverEvent;

    static MouseEvent MouseLeaveEvent;

    static MouseEvent QueryCursorEvent;

    static void UpdateCursor(UIElement* element);

    static void SetLastInput(QGraphicsSceneMouseEvent& input);

    static int GetTimestamp();
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
    Point GetPosition(Visual* relativeTo);

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

    virtual UIElement* Target() override;

    virtual PresentationSource* ActiveSource() override;

    virtual int Id() override;

    virtual SharedPointer<StylusPointDescription> PointDescription() override;

    virtual Array<int> PacketData(QEvent& event) override;

private:
    SharedPointer<StylusPointDescription> description_;
    QPointF lastPosition_;
};

INKCANVAS_END_NAMESPACE

#endif // MOUSEDEVICE_H
