#include "Windows/Input/mousedevice.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"
#include "Windows/Input/mousebuttoneventargs.h"
#include "Windows/routedeventargs.h"
#include "Windows/uielement.h"

#include <QMouseEvent>
#include <QWidget>
#include <QApplication>

MouseEvent::MouseEvent(int type)
    : RoutedEvent(type)
{
}

void MouseEvent::handle(QEvent &event, QList<RoutedEventHandler> handlers)
{
    MouseButtonEventArgs args(static_cast<QMouseEvent&>(event));
    RoutedEvent::handle(event, args, handlers);
}

MouseDevice* Mouse::PrimaryDevice = nullptr;

MouseEvent Mouse::MouseDownEvent(QEvent::MouseButtonPress);

MouseEvent Mouse::MouseMoveEvent(QEvent::MouseMove);

MouseEvent Mouse::MouseUpEvent(QEvent::MouseButtonRelease);

MouseEvent Mouse::MouseEnterEvent(QEvent::HoverEnter);

MouseEvent Mouse::MouseLeaveEvent(QEvent::HoverLeave);

MouseEvent Mouse::QueryCursorEvent(QEvent::CursorChange);

void Mouse::UpdateCursor()
{

}

MouseDevice::MouseDevice()
{
    description_.reset(new StylusPointDescription);
}

void MouseDevice::SetLastPosition(const QPointF &pos)
{
    lastPosition_ = pos;
}

/// <summary>
///     Calculates the position of the mouse relative to
///     a particular element.
/// </summary>
QPointF MouseDevice::GetPosition(Visual* relativeTo)
{
    (void) relativeTo;
    return lastPosition_;
}

/// <summary>
///     The state of the left button.
/// </summary>
MouseButtonState MouseDevice::LeftButton()
{
    return QApplication::mouseButtons().testFlag(Qt::LeftButton)
            ? MouseButtonState::Pressed : MouseButtonState::Released;
}

/// <summary>
///     The state of the right button.
/// </summary>
MouseButtonState MouseDevice::RightButton()
{
    return QApplication::mouseButtons().testFlag(Qt::RightButton)
            ? MouseButtonState::Pressed : MouseButtonState::Released;
}

/// <summary>
///     The state of the middle button.
/// </summary>
MouseButtonState MouseDevice::MiddleButton()
{
    return QApplication::mouseButtons().testFlag(Qt::MiddleButton)
            ? MouseButtonState::Pressed : MouseButtonState::Released;
}

MouseButtonState MouseDevice::XButton1()
{
    return QApplication::mouseButtons().testFlag(Qt::XButton1)
            ? MouseButtonState::Pressed : MouseButtonState::Released;
}

MouseButtonState MouseDevice::XButton2()
{
    return QApplication::mouseButtons().testFlag(Qt::XButton2)
            ? MouseButtonState::Pressed : MouseButtonState::Released;
}

UIElement* MouseDevice::Captured()
{
    return qobject_cast<UIElement*>(QWidget::mouseGrabber());
}

UIElement* MouseDevice::Target()
{
    return nullptr;
}

PresentationSource* MouseDevice::ActiveSource()
{
    return nullptr;
}

int MouseDevice::Id()
{
    return 0;
}

QSharedPointer<StylusPointDescription> MouseDevice::PointDescription()
{
    return description_;
}

QVector<int> MouseDevice::PacketData(QInputEvent& event)
{
    QMouseEvent& mouseEvent(static_cast<QMouseEvent&>(event));
    QVector<int> data;
    QPoint pt2 = mouseEvent.pos();
    data.append(pt2.x());
    data.append(pt2.y());
    data.append(0);
    return data;
}
