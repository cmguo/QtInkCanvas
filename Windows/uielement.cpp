#include "Windows/uielement.h"
#include "Windows/dependencypropertychangedeventargs.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/Input/pencontexts.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"

#include <QEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

INKCANVAS_BEGIN_NAMESPACE

RoutedEvent UIElement::LostMouseCaptureEvent(QEvent::UngrabMouse);

RoutedEvent UIElement::LostStylusCaptureEvent(QEvent::TouchCancel);

DependencyProperty const * const UIElement::RenderTransformProperty =
    new DependencyProperty(nullptr);

static constexpr int ITEM_DATA_RECT = UIElement::ITEM_DATA + 1;

UIElement *UIElement::fromItem(QGraphicsItem *item)
{
    if (item == nullptr)
        return nullptr;
    QVariant elem = item->data(ITEM_DATA);
    if (elem.userType() == qMetaTypeId<UIElement*>()) {
        UIElement * ue = elem.value<UIElement*>();
        if (ue == item)
            return ue;
    }
    return nullptr;
}

struct RoutedEventAndHandlers
{
    RoutedEvent* route;
    QList<RoutedEventHandler> handlers;
};

struct RoutedEventStore
{
    int count = 0;
    QMap<RoutedEvent*, RoutedEventAndHandlers*> byroute;
    QMap<int, RoutedEventAndHandlers*> bytype;
};

INKCANVAS_END_NAMESPACE

Q_DECLARE_METATYPE(INKCANVAS_PREPEND_NAMESPACE(RoutedEventStore)*)

INKCANVAS_BEGIN_NAMESPACE

UIElement::UIElement()
{
    //qDebug() << "construct" << static_cast<QObject*>(this);
    setData(ITEM_DATA, QVariant::fromValue(this));
    setData(ITEM_DATA_RECT, QRectF());
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(ItemIsFocusable);
}

UIElement::~UIElement()
{
    //qDebug() << "destruct" << static_cast<QObject*>(this);
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        RoutedEventStore * store = property("RoutedEventStore").value<RoutedEventStore*>();
        for (RoutedEventAndHandlers* rh : store->byroute) {
            delete rh;
        }
        delete store;
    }
}

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
        if (event.type()) {
            //qDebug() << "AddHandler" << static_cast<QEvent::Type>(event.type());
            if (event.type() == QEvent::GraphicsSceneHoverEnter) {
                setAcceptHoverEvents(true);
            }
            store->bytype.insert(event.type(), rh);
        }
    }
    if (!rh->handlers.contains(handler)) {
        rh->handlers.append(handler);
        if (++store->count == 1) {
            UIElement * parent = this;
            while (UIElement * pp = parent->Parent()) {
                parent = pp;
            }
            if (parent != this)
                Arrange(parent->boundingRect());
        }
    }
}

void UIElement::RemoveHandler(RoutedEvent &event, const RoutedEventHandler &handler)
{
    if (!privateFlags_.testFlag(HasRoutedEventStore))
        return;
    RoutedEventStore * store = property("RoutedEventStore").value<RoutedEventStore*>();
    RoutedEventAndHandlers* rh = store->byroute.value(&event, nullptr);
    if (rh) {
        rh->handlers.removeOne(handler);
        if (--store->count == 0) {
            SetRenderSize({0, 0});
        }
    }
}

void UIElement::ApplyTemplate()
{
    OnPreApplyTemplate();
}

List<UIElement*> UIElement::Children()
{
    List<UIElement*> list;
    for (QObject * c : children()) {
        UIElement * ui = qobject_cast<UIElement*>(c);
        if (ui)
            list.Add(ui);
    }
    return list;
}

UIElement* UIElement::Parent()
{
    return fromItem(parentItem());
}

void UIElement::InvalidateVisual()
{
    update();
}


GeneralTransform UIElement::LayoutTransform()
{
    return GeneralTransform();
}

GeneralTransform UIElement::RenderTransform()
{
    return sceneTransform().toAffine();
}

GeneralTransform UIElement::TransformToVisual(UIElement* visual)
{
    return itemTransform(visual).toAffine();
}

Size UIElement::DesiredSize()
{
    return Size();
}

Size UIElement::RenderSize()
{
    return boundingRect().size();
}

void UIElement::SetRenderSize(Size const & size)
{
    Arrange(Rect(QPointF(0, 0), size));
    //rect.moveCenter(QPointF(0, 0));
}

void UIElement::Arrange(Rect const & rect)
{
    prepareGeometryChange();
    setData(ITEM_DATA_RECT, QRectF(rect));
//    for (QGraphicsItem * o : childItems()) {
//        UIElement* ue = fromItem(o);
//        if (ue) {
//            ue->Arrange(rect);
//        }
//    }
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
    return isUnderMouse();
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
    //grabMouse();
}

bool UIElement::IsMouseCaptured()
{
    return true;// Mouse::PrimaryDevice->Captured() == this;
}

void UIElement::ReleaseMouseCapture()
{
    //ungrabMouse();
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
    return true;
}

bool UIElement::IsAncestorOf(UIElement* child)
{
    return isAncestorOf(child);
}

void UIElement::ReleaseStylusCapture()
{
}


void UIElement::Measure(Size)
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

void UIElement::SetLimitInputPosition(bool active)
{
    privateFlags_.setFlag(LimitInputPosition, active);
}

bool UIElement::GetLimitInputPosition() const
{
    return privateFlags_.testFlag(LimitInputPosition);
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
    UIElement * parent = fromItem(VisualParent());
    if (parent)
        parent->RaiseEvent(args);
}

void UIElement::OnChildDesiredSizeChanged(UIElement* child)
{
    (void) child;
}

List<UIElement*> UIElement::CreateUIElementCollection(UIElement* logicalParent)
{
    (void) logicalParent;
    return {};
}

List<UIElement*> UIElement::LogicalChildren()
{
    return {};
}

List<UIElement*> UIElement::InternalChildren()
{
    return {};
}

Geometry* UIElement::GetLayoutClip(Size layoutSlotSize)
{
    (void) layoutSlotSize;
    return nullptr;
}

Size UIElement::ArrangeOverride(Size arrangeSize)
{
    (void) arrangeSize;
    return Size();
}

Size UIElement::MeasureOverride(Size availableSize)
{
    (void) availableSize;
    return Size();
}

void UIElement::OnPreApplyTemplate()
{
}

QVariant UIElement::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case QGraphicsItem::ItemVisibleHasChanged:
        //qDebug() << "itemChange " << change;
        emit IsVisibleChanged();
        break;
    case QGraphicsItem::ItemEnabledHasChanged:
        //qDebug() << "itemChange " << change;
        emit IsEnabledChanged();
        break;
    case QGraphicsItem::ItemParentHasChanged:
        //qDebug() << "itemChange " << change;
        break;
    case QGraphicsItem::ItemTransformHasChanged:
    {
        //qDebug() << "itemChange " << change;
        DependencyPropertyChangedEventArgs e(RenderTransformProperty, QVariant(), QVariant());
        OnPropertyChanged(e);
    }
        break;
    case QGraphicsItem::ItemChildAddedChange:
    {
        UIElement* ue = fromItem(value.value<QGraphicsItem*>());
        if (ue) {
            ue->SetRenderSize(RenderSize());
        }
    }
        break;
    default:
        break;
    }
    return value;
}

bool UIElement::sceneEvent(QEvent *event)
{
    if (privateFlags_.testFlag(HasPenContexts)) {
        GetPenContexts()->eventFilter(this, event);
    }
    bool handled = false;
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        RoutedEventStore* store = property("RoutedEventStore").value<RoutedEventStore*>();
        RoutedEventAndHandlers* rh = store->bytype.value(event->type(), nullptr);
        if (rh) {
            //if (event->type() != QEvent::GraphicsSceneHoverMove)
            //    qDebug() << "sceneEvent" << event->type() << static_cast<QObject*>(this);
            rh->route->handle(*event, rh->handlers);
            if (!event->isAccepted() && rh->route->type2()) {
                RoutedEventAndHandlers* rh2 = store->bytype.value(rh->route->type2(), nullptr);
                if (rh2) // if not handle touch event, not transfer to mouse event
                    rh->route->handle2(*event, rh2->handlers);
            }
            handled = event->isAccepted();
        }
    }
    if (privateFlags_.testFlag(HasPenContexts)) {
        GetPenContexts()->FireCustomData();
    }
    if (handled)
        return true;
    //qDebug() << "event reject" << event;
    return Visual::sceneEvent(event);
}

QRectF UIElement::boundingRect() const
{
    return data(ITEM_DATA_RECT).toRectF();
}

DependencyProperty const * const FrameworkElement::LayoutTransformProperty =
    new DependencyProperty(nullptr);

Rect FrameworkElement::Margin()
{
    return Rect();
}

double FrameworkElement::ActualWidth()
{
    return 0;
}

double FrameworkElement::ActualHeight()
{
    return 0;
}

void FrameworkElement::BringIntoView(Rect)
{
}

FlowDirection FrameworkElement::GetFlowDirection()
{
    return FlowDirection::LeftToRight;
}

int FrameworkElement::VisualChildrenCount()
{
    return childItems().size();
}

Visual* FrameworkElement::GetVisualChild(int index)
{
    return static_cast<Visual*>(childItems().at(index));
}

INKCANVAS_END_NAMESPACE
