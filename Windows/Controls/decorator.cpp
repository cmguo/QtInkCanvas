#include "decorator.h"

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
    return nullptr;
}

void AdornerLayer::Add(Adorner *adorner)
{
    AddVisualChild(adorner);
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

