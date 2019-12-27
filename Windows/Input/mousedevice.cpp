#include "Windows/Input/mousedevice.h"
#include "Windows/Input/styluspoint.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"
#include "Windows/Input/mousebuttoneventargs.h"
#include "Windows/Input/querycursoreventargs.h"
#include "Windows/routedeventargs.h"
#include "Windows/uielement.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QApplication>
#include <QWidget>

#include <Windows.h>
#include <sysinfoapi.h>

MouseEvent::MouseEvent(int type)
    : RoutedEvent(type)
{
}

void MouseEvent::handle(QEvent &event, QList<RoutedEventHandler> handlers)
{
    MouseButtonEventArgs args(static_cast<QGraphicsSceneMouseEvent&>(event));
    args.MarkAsUserInitiated();
    RoutedEvent::handle(event, args, handlers);
}

MouseDevice* Mouse::PrimaryDevice = nullptr;

MouseEvent Mouse::MouseDownEvent(QEvent::GraphicsSceneMousePress);

MouseEvent Mouse::MouseMoveEvent(QEvent::GraphicsSceneMouseMove);

MouseEvent Mouse::MouseUpEvent(QEvent::GraphicsSceneMouseRelease);

MouseEvent Mouse::MouseEnterEvent(QEvent::GraphicsSceneHoverEnter);

MouseEvent Mouse::MouseHoverEvent(QEvent::GraphicsSceneHoverMove);

MouseEvent Mouse::MouseLeaveEvent(QEvent::GraphicsSceneHoverLeave);

MouseEvent Mouse::QueryCursorEvent(0);

void Mouse::UpdateCursor(UIElement* element)
{
    QueryCursorEventArgs args(Mouse::PrimaryDevice, 0);
    args.SetRoutedEvent(Mouse::QueryCursorEvent);
    element->RaiseEvent(args);
    if (args.Handled()) {
        if (args.Cursor() == QCursor())
            element->unsetCursor();
        else
            element->setCursor(args.Cursor());
    }
}

int Mouse::GetTimestamp()
{
    return ::GetTickCount();
}

MouseDevice::MouseDevice()
{
    description_.reset(new StylusPointDescription);
    description_.reset(
                new StylusPointDescription(description_->GetStylusPointProperties(), -1));
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
    QWidget* grabber = QWidget::mouseGrabber();
    QGraphicsView * view = qobject_cast<QGraphicsView*>(grabber);
    if (view == nullptr)
        return nullptr;
    return UIElement::fromItem(view->scene()->mouseGrabberItem());
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

QVector<int> MouseDevice::PacketData(QEvent& event)
{
    QGraphicsSceneMouseEvent& mouseEvent(static_cast<QGraphicsSceneMouseEvent&>(event));
    QVector<int> data;
    QPoint pt2 = mouseEvent.pos().toPoint();
    data.append(pt2.x());
    data.append(pt2.y());
    return data;
}
