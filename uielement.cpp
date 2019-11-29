#include "uielement.h"
#include "inputdevice.h"

#include <QEvent>

QMap<RoutedEvent*, int> UIElement::events_;

UIElement::UIElement()
{
    if (events_.empty()) {
        events_[&Mouse::MouseMoveEvent] = QEvent::MouseMove;
        events_[&Stylus::StylusMoveEvent] = QEvent::TouchUpdate;
        events_[&LostMouseCaptureEvent] = QEvent::UngrabMouse;
        events_[&LostStylusCaptureEvent] = QEvent::TouchCancel;
    }
}

void UIElement::AddHandler(RoutedEvent &event, const RoutedEventHandler &handler)
{
    if (events_.contains(&event)) {
        QList<RoutedEventHandler>& handlers = handlers_[events_[&event]];
        if (!handlers.contains(handler))
            handlers.append(handler);
    }
}

void UIElement::RemoveHandler(RoutedEvent &event, const RoutedEventHandler &handler)
{
    if (events_.contains(&event)) {
        QList<RoutedEventHandler>& handlers = handlers_[events_[&event]];
        handlers.removeOne(handler);
    }
}
