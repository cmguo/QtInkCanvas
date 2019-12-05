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

class DrawingContext;

class Adorner : public FrameworkElement
{
    Q_OBJECT
public:
    Adorner(UIElement* adornedElement);

    UIElement* AdornedElement();

    virtual QTransform GetDesiredTransform(QTransform const& transform);

    virtual void OnRender(DrawingContext& drawingContext) = 0;

protected:
    virtual void paintEvent(QPaintEvent* event) override;

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


#endif // DECORATOR_H
