#include "decorator.h"
#include "qtpainterdrawingcontext.h"

#include <QPainter>

Decorator::Decorator()
{

}

UIElement *Decorator::Child()
{
    return child_;
}

void Decorator::SetChild(UIElement *value)
{
    if (child_ == value)
        return;
    if (child_)
         RemoveVisualChild(child_);
    child_ = value;
    if (child_)
        AddVisualChild(child_);
}

Adorner::Adorner(UIElement* adornedElement)
    : adornedElement_(adornedElement)
{
}

UIElement* Adorner::AdornedElement()
{
    return adornedElement_;
}

QTransform Adorner::GetDesiredTransform(const QTransform &transform)
{
    return transform;
}

void Adorner::paintEvent(QPaintEvent *event)
{
    (void) event;
    QPainter painter(this);
    QTransform transform(painter.transform());
    painter.setTransform(GetDesiredTransform(transform));
    QtPainterDrawingContext context(painter);
    OnRender(context);
    painter.setTransform(transform);
}

void AdornerLayer::Add(Adorner *adorner)
{
    AddVisualChild(adorner);
}

void AdornerLayer::Remove(Adorner *adorner)
{
    RemoveVisualChild(adorner);
}

AdornerLayer *AdornerLayer::GetAdornerLayer(Visual *visual)
{
    Visual * parent = visual->VisualParent();
    while (parent) {
        AdornerLayer* l = qobject_cast<AdornerLayer*>(parent);
        if (l)
            return l;
        AdornerDecorator* d = qobject_cast<AdornerDecorator*>(parent);
        if (d)
            return d->GetAdornerLayer();
        parent = parent->VisualParent();
    }
    return nullptr;
}

AdornerDecorator::AdornerDecorator()
{
    adornerLayer_ = new AdornerLayer;
}

void AdornerDecorator::SetChild(UIElement *value)
{
    Decorator::SetChild(value);
    if (value)
        AddVisualChild(adornerLayer_);
    else
        RemoveVisualChild(adornerLayer_);
}

AdornerLayer *AdornerDecorator::GetAdornerLayer()
{
    return adornerLayer_;
}

