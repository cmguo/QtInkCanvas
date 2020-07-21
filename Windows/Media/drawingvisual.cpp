#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/drawing.h"

#include <QPainter>
#include <QDebug>

INKCANVAS_BEGIN_NAMESPACE

DrawingVisual::DrawingVisual()
{
    setFlag(ItemHasNoContents, false);
}

DrawingVisual::~DrawingVisual()
{
    if (drawing_)
        delete drawing_;
}

class VisualDrawingContext : public DrawingGroupDrawingContext
{
public:
    VisualDrawingContext(DrawingVisual* visual)
        : DrawingGroupDrawingContext(visual->GetDrawing())
        , visual_(visual)
    {
    }

    virtual void CloseCore(List<Drawing*> rootDrawingGroupChildren) override
    {
        DrawingGroupDrawingContext::CloseCore(rootDrawingGroupChildren);
        visual_->RenderClose();
    }

private:
    DrawingVisual* visual_;
};

DrawingContext * DrawingVisual::RenderOpen()
{
    prepareGeometryChange();
    if (drawing_)
        delete drawing_;
    drawing_ = new DrawingGroup;
    return new VisualDrawingContext(this);
}

void DrawingVisual::RenderClose()
{
    prepareGeometryChange();
}

DrawingGroup * DrawingVisual::GetDrawing()
{
    return drawing_;
}

QRectF DrawingVisual::boundingRect() const
{
    return drawing_ ? QRectF(drawing_->Bounds()) : QRectF();
}

void DrawingVisual::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    //qDebug() << "DrawingVisual::paint" << this->boundingRect() << painter->clipRegion();
    if (drawing_) {
        drawing_->Draw(*painter);
    }
}

INKCANVAS_END_NAMESPACE
