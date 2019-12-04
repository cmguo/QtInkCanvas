#ifndef DECORATOR_H
#define DECORATOR_H

#include "Windows/uielement.h"

class Decorator : public FrameworkElement
{
    Q_OBJECT
public:
    Decorator();

    virtual UIElement* Child();

    virtual void SetChild(UIElement * value);

private:
    UIElement* child_ = nullptr;
};

class Adorner : public FrameworkElement
{
    Q_OBJECT
public:
    Adorner(UIElement* adornedElement);

    UIElement* AdornedElement();

private:
    UIElement* adornedElement_;
};

class AdornerLayer : public FrameworkElement
{
    Q_OBJECT
public:
    void Add(Adorner* adorner);
};

class AdornerDecorator : public Decorator
{
    Q_OBJECT
public:
    AdornerDecorator();

    virtual void SetChild(UIElement * value);

    AdornerLayer* GetAdornerLayer();

private:
    AdornerLayer* adornerLayer_;
};


#endif // DECORATOR_H
