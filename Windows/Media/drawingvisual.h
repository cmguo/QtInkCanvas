#ifndef WINDOWS_MEDIA_DRAWINGVISUAL_H
#define WINDOWS_MEDIA_DRAWINGVISUAL_H

#include "containervisual.h"

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class DrawingContext;
class DrawingGroup;

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

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_MEDIA_DRAWINGVISUAL_H
