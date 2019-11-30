#include "uielement.h"
#include "inputdevice.h"

#include <QEvent>

RoutedEvent UIElement::LostMouseCaptureEvent;

RoutedEvent UIElement::LostStylusCaptureEvent;

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

void UIElement::ApplyTemplate()
{
}

QList<UIElement*> UIElement::Children()
{
    QList<UIElement*> list;
    for (QObject * c : children()) {
        UIElement * ui = qobject_cast<UIElement*>(c);
        if (ui)
            list.append(ui);
    }
    return list;
}

UIElement* UIElement::Parent()
{
    return qobject_cast<UIElement*>(parent());
}

void UIElement::InvalidateVisual()
{
}


QTransform UIElement::LayoutTransform()
{
    return QTransform();
}

QTransform UIElement::RenderTransform()
{
    return QTransform();
}

QTransform UIElement::TransformToVisual(UIElement* visual)
{
    return QTransform();
}

QTransform UIElement::TransformToAncestor(UIElement* visual)
{
    return QTransform();
}

QTransform UIElement::TransformToDescendant(UIElement* visual)
{
    return QTransform();
}

QSizeF UIElement::DesiredSize()
{
    return QSizeF();
}

QSizeF UIElement::RenderSize()
{
    return QSizeF();
}

void UIElement::SetRenderSize(QSizeF)
{
}

void UIElement::Arrange(QRectF rect)
{
}

void UIElement::InvalidateMeasure()
{
}

void UIElement::InvalidateArrange()
{
}

bool UIElement::IsMeasureValid()
{
    return false;
}

bool UIElement::IsArrangeValid()
{
    return false;
}

void UIElement::UpdateLayout()
{
}

bool UIElement::IsMouseOver()
{
    return false;
}

bool UIElement::IsStylusOver()
{
    return false;
}

bool UIElement::Focus()
{
    return false;
}

void UIElement::CaptureMouse()
{
}

bool UIElement::IsMouseCaptured()
{
    return false;
}

bool UIElement::IsEnabled()
{
    return false;
}

bool UIElement::ForceCursor()
{
    return false;
}

bool UIElement::ClipToBounds()
{
    return false;
}

void UIElement::ReleaseMouseCapture()
{
}

void UIElement::CaptureStylus()
{
}

bool UIElement::IsStylusCaptured()
{
    return false;
}

bool UIElement::IsAncestorOf(UIElement*)
{
    return false;
}

void UIElement::ReleaseStylusCapture()
{
}


void UIElement::Measure(QSizeF)
{
}

void UIElement::SetWidth(double)
{
}

void UIElement::SetHeight(double)
{
}

QList<StylusPlugIn*>& UIElement::StylusPlugIns()
{
    return stylusPlugIns_;
}

void UIElement::OnChildDesiredSizeChanged(UIElement* child)
{
}

QList<UIElement*> UIElement::CreateUIElementCollection(UIElement* logicalParent)
{
    return {};
}

QList<UIElement*> UIElement::LogicalChildren()
{
    return {};
}

QList<UIElement*> UIElement::InternalChildren()
{
    return {};
}

Geometry* UIElement::GetLayoutClip(QSizeF layoutSlotSize)
{
    return nullptr;
}

QSizeF UIElement::ArrangeOverride(QSizeF arrangeSize)
{
    return QSizeF();
}

QSizeF UIElement::MeasureOverride(QSizeF availableSize)
{
    return QSizeF();
}

bool UIElement::event(QEvent *event)
{
    return QWidget::event(event);
}

QRectF FrameworkElement::Margin()
{
    return QRectF();
}

double FrameworkElement::ActualWidth()
{
    return 0;
}

double FrameworkElement::ActualHeight()
{
    return 0;
}

void FrameworkElement::BringIntoView(QRectF)
{
}

FlowDirection FrameworkElement::GetFlowDirection()
{
    return FlowDirection::LeftToRight;
}

int FrameworkElement::VisualChildrenCount()
{
    return 0;
}

Visual* FrameworkElement::GetVisualChild(int index)
{
    return nullptr;
}

Adorner::Adorner(UIElement* adornedElement)
{

}

UIElement* Adorner::AdornedElement()
{
    return nullptr;
}

