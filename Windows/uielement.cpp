#include "Windows/uielement.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/Input/pencontexts.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"

#include <QEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

RoutedEvent UIElement::LostMouseCaptureEvent(QEvent::UngrabMouse);

RoutedEvent UIElement::LostStylusCaptureEvent(QEvent::TouchCancel);

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

UIElement::UIElement()
{
    //qDebug() << "construct" << static_cast<QObject*>(this);
    setData(ITEM_DATA, QVariant::fromValue(this));
    setData(ITEM_DATA_RECT, QRectF());
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
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
    return fromItem(parentItem());
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
    return boundingRect().size();
}

void UIElement::SetRenderSize(QSizeF const & size)
{
    Arrange(QRectF(QPointF(0, 0), size));
    //rect.moveCenter(QPointF(0, 0));
}

void UIElement::Arrange(QRectF const & rect)
{
    prepareGeometryChange();
    setData(ITEM_DATA_RECT, rect);
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
    UIElement * parent = fromItem(VisualParent());
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
        //qDebug() << "itemChange " << change << qobject_cast<QObject*>(this) << value.value<QGraphicsItem*>();
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
    if (privateFlags_.testFlag(HasRoutedEventStore)) {
        RoutedEventStore* store = property("RoutedEventStore").value<RoutedEventStore*>();
        RoutedEventAndHandlers* rh = store->bytype.value(event->type(), nullptr);
        if (rh) {
            //if (event->type() != QEvent::GraphicsSceneHoverMove)
            //    qDebug() << "sceneEvent" << event->type() << static_cast<QObject*>(this);
            rh->route->handle(*event, rh->handlers);
            if (event->isAccepted())
                return true;
        }
    }
    if (privateFlags_.testFlag(HasPenContexts)) {
        GetPenContexts()->FireCustomData();
    }
    //qDebug() << "event reject" << event;
    return Visual::sceneEvent(event);
}

QRectF UIElement::boundingRect() const
{
    return data(ITEM_DATA_RECT).toRectF();
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

