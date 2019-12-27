#ifndef DRAWINGVISUAL_H
#define DRAWINGVISUAL_H

#include "containervisual.h"

class DrawingContext;
class DrawingGroup;

// namespace System.Windows.Media

class DrawingVisual : public ContainerVisual
{
    Q_OBJECT
public:
    DrawingVisual();

    virtual ~DrawingVisual() override;

    DrawingContext * RenderOpen();

    void RenderClose();

    DrawingGroup * GetDrawing();

public:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    DrawingGroup * drawing_ = nullptr;
};

#endif // DRAWINGVISUAL_H
