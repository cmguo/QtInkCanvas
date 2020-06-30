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

#if WIN32
#include <Windows.h>
#include <sysinfoapi.h>
#endif

INKCANVAS_BEGIN_NAMESPACE

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
#ifdef WIN32
#if QT_VERSION >= 0x050A00
        if (args.Cursor() == QCursor())
#else
        if (args.Cursor().shape() == QCursor().shape())
#endif
            element->unsetCursor(); // not work on macosx
        else
#endif
            element->setCursor(args.Cursor());
    }
}

void Mouse::SetLastInput(QGraphicsSceneMouseEvent &input)
{
    PrimaryDevice->SetLastPosition(input.pos());
}

int Mouse::GetTimestamp()
{
#if WIN32
    return ::GetTickCount();
#else
    return clock();
#endif
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
Point MouseDevice::GetPosition(Visual* relativeTo)
{
    (void) relativeTo;
    UIElement* e = UIElement::fromItem(relativeTo);
    if (!e || !e->GetLimitInputPosition())
        return lastPosition_;
    QRectF r(relativeTo->boundingRect());
    if (!r.contains(lastPosition_)) {
        if (lastPosition_.x() < r.left())
            lastPosition_.setX(r.left());
        if (lastPosition_.y() < r.top())
            lastPosition_.setY(r.top());
        if (lastPosition_.x() > r.right())
            lastPosition_.setX(r.right());
        if (lastPosition_.y() > r.bottom())
            lastPosition_.setY(r.bottom());
    }
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

SharedPointer<StylusPointDescription> MouseDevice::PointDescription()
{
    return description_;
}

Array<int> MouseDevice::PacketData(QEvent& event)
{
    QGraphicsSceneMouseEvent& mouseEvent(static_cast<QGraphicsSceneMouseEvent&>(event));
    Array<int> data(2);
    QPoint pt2 = mouseEvent.pos().toPoint();
    data[0] = pt2.x();
    data[1] = pt2.y();
    return data;
}

INKCANVAS_END_NAMESPACE
