#include "Windows/uielement.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/Input/pencontexts.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"

#include <QEvent>
#include <QDebug>

RoutedEvent UIElement::LostMouseCaptureEvent(QEvent::UngrabMouse);

RoutedEvent UIElement::LostStylusCaptureEvent(QEvent::TouchCancel);

UIElement::UIElement()
{
}

struct RoutedEventAndHandlers
{
    RoutedEvent* route;
    QList<RoutedEventHandler> handlers;
};

struct RoutedEventStore
{
    QMap<RoutedEvent*, RoutedEventAndHandlers*> byroute;
    QMap<int, RoutedEventAndHandlers*> bytype;
};

Q_DECLARE_METATYPE(RoutedEventStore*)

void UIElement::AddHandler(RoutedEvent &event, const RoutedEventHandler &handler)
{
    RoutedEventStore * store;
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        store = property("RoutedEventStore").value<RoutedEventStore*>();
    } else {
        store = new RoutedEventStore;
        setProperty("RoutedEventStore", QVariant::fromValue(store));
        privateFlags_ |= HasRoutedEventStore;
    }
    RoutedEventAndHandlers* rh = store->byroute.value(&event, nullptr);
    if (rh == nullptr) {
        rh = new RoutedEventAndHandlers;
        rh->route = &event;
        store->byroute.insert(&event, rh);
        if (event.type())
            store->bytype.insert(event.type(), rh);
        if (event.type() == QEvent::HoverEnter)
            setAttribute(Qt::WA_Hover);
    }
    if (!rh->handlers.contains(handler))
        rh->handlers.append(handler);
}

void UIElement::RemoveHandler(RoutedEvent &event, const RoutedEventHandler &handler)
{
    RoutedEventStore * store;
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        store = property("RoutedEventStore").value<RoutedEventStore*>();
    } else {
        store = new RoutedEventStore;
        setProperty("RoutedEventStore", QVariant::fromValue(store));
        privateFlags_ |= HasRoutedEventStore;
    }
    RoutedEventAndHandlers* rh = store->byroute.value(&event, nullptr);
    if (rh) {
        rh->handlers.removeOne(handler);
    }
}

void UIElement::ApplyTemplate()
{
    OnPreApplyTemplate();
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
    update();
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
    //return underMouse();
    return true;
}

bool UIElement::IsStylusOver()
{
    return false;
}

bool UIElement::Focus()
{
    setFocus();
    return true;
}

void UIElement::CaptureMouse()
{
    grabMouse();
}

bool UIElement::IsMouseCaptured()
{
    return Mouse::PrimaryDevice->Captured() == this;
}

void UIElement::ReleaseMouseCapture()
{
    releaseMouse();
}

bool UIElement::IsVisible()
{
    return isVisible();
}

bool UIElement::IsHitTestVisible()
{
    return isVisible();
}

bool UIElement::IsEnabled()
{
    return isEnabled();
}

bool UIElement::ForceCursor()
{
    return false;
}

bool UIElement::ClipToBounds()
{
    return false;
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

StylusPlugInCollection& UIElement::StylusPlugIns()
{
    QVariant p = property("StylusPlugIns");
    if (p.isValid())
        return *p.value<StylusPlugInCollection*>();
    StylusPlugInCollection* c = new StylusPlugInCollection(this);
    setProperty("StylusPlugIns", QVariant::fromValue(c));
    return *c;
}

PenContexts* UIElement::GetPenContexts()
{
    QVariant p = property("PenContexts");
    if (p.isValid())
        return p.value<PenContexts*>();
    PenContexts* c = new PenContexts(this);
    setProperty("PenContexts", QVariant::fromValue(c));
    privateFlags_.setFlag(HasPenContexts);
    return c;
}

void UIElement::RaiseEvent(RoutedEventArgs &args)
{
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        RoutedEventStore* store = property("RoutedEventStore").value<RoutedEventStore*>();
        RoutedEventAndHandlers* rh = store->byroute.value(&args.GetRoutedEvent(), nullptr);
        if (rh) {
            QEvent e(QEvent::None);
            rh->route->handle(e, args, rh->handlers);
            if (e.isAccepted())
                return;
        }
    }
    UIElement * parent = qobject_cast<UIElement*>(parentWidget());
    if (parent)
        parent->RaiseEvent(args);
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

void UIElement::OnPreApplyTemplate()
{
}

bool UIElement::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Show:
        //qDebug() << "event " << event;
        emit IsVisibleChanged();
        break;
    case QEvent::EnabledChange:
        //qDebug() << "event " << event;
        emit IsEnabledChanged();
        break;
    default:
        break;
    }
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        RoutedEventStore* store = property("RoutedEventStore").value<RoutedEventStore*>();
        RoutedEventAndHandlers* rh = store->bytype.value(event->type(), nullptr);
        if (rh) {
            //qDebug() << "event " << event;
            rh->route->handle(*event, rh->handlers);
            if (event->isAccepted())
                return true;
        }
    }
    if (privateFlags_.testFlag(HasPenContexts)) {
        GetPenContexts()->FireCustomData();
    }
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

