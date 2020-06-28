#ifndef DECORATOR_H
#define DECORATOR_H

#include "Windows/uielement.h"

INKCANVAS_BEGIN_NAMESPACE

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

class DrawingContext;

class Adorner : public FrameworkElement
{
    Q_OBJECT
public:
    Adorner(UIElement* adornedElement);

    UIElement* AdornedElement();

    virtual GeneralTransform GetDesiredTransform(GeneralTransform const& transform);

    virtual void OnRender(DrawingContext& drawingContext) = 0;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    UIElement* adornedElement_;
};

class AdornerLayer : public FrameworkElement
{
    Q_OBJECT
public:
    void Add(Adorner* adorner);

    void Remove(Adorner* adorner);

    static AdornerLayer *GetAdornerLayer(Visual* visual);
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

INKCANVAS_END_NAMESPACE

#endif // DECORATOR_H
