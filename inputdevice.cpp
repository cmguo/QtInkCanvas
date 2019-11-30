#include "inputdevice.h"
#include "routedeventargs.h"

StylusDevice* Stylus::CurrentStylusDevice()
{
    return nullptr;
}

RoutedEvent Stylus::StylusMoveEvent;

bool StylusDevice::InAir()
{
    return false;
}

bool StylusDevice::Inverted()
{
    return false;
}

QSharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual * widget, QSharedPointer<StylusPointDescription> description)
{
    return nullptr;
}

Visual* StylusDevice::Captured()
{
    return nullptr;
}

MouseDevice* Mouse::PrimaryDevice = nullptr;

RoutedEvent Mouse::MouseDownEvent;

RoutedEvent Mouse::MouseMoveEvent;

RoutedEvent Mouse::MouseUpEvent;

RoutedEvent Mouse::MouseEnterEvent;

RoutedEvent Mouse::MouseLeaveEvent;

void Mouse::UpdateCursor()
{

}

/// <summary>
///     Calculates the position of the mouse relative to
///     a particular element.
/// </summary>
QPointF MouseDevice::GetPosition(UIElement* relativeTo)
{
    return QPointF();
}

/// <summary>
///     The state of the left button.
/// </summary>
MouseButtonState MouseDevice::LeftButton()
{
    return MouseButtonState::Released;
}

/// <summary>
///     The state of the right button.
/// </summary>
MouseButtonState MouseDevice::RightButton()
{
    return MouseButtonState::Released;
}

/// <summary>
///     The state of the middle button.
/// </summary>
MouseButtonState MouseDevice::MiddleButton()
{
    return MouseButtonState::Released;
}

MouseButtonState MouseDevice::XButton1()
{
    return MouseButtonState::Released;
}

MouseButtonState MouseDevice::XButton2()
{
    return MouseButtonState::Released;
}

Visual* MouseDevice::Captured()
{
    return nullptr;
}

