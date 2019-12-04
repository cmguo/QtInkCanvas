#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/drawing.h"

#include <QPainter>

DrawingVisual::DrawingVisual()
{
}

class VisualDrawingContext : public DrawingGroupDrawingContext
{
public:
    VisualDrawingContext(DrawingVisual* visual)
        : DrawingGroupDrawingContext(visual->GetDrawing())
        , visual_(visual)
    {
    }

    virtual void CloseCore(QList<Drawing*> rootDrawingGroupChildren) override
    {
        DrawingGroupDrawingContext::CloseCore(rootDrawingGroupChildren);
        visual_->RenderClose();
    }

private:
    DrawingVisual* visual_;
};

DrawingContext * DrawingVisual::RenderOpen()
{
    if (!drawing_)
        drawing_ = new DrawingGroup;
    return new VisualDrawingContext(this);
}

void DrawingVisual::RenderClose()
{
    update();
}

DrawingGroup * DrawingVisual::GetDrawing()
{
    return drawing_;
}

void DrawingVisual::paintEvent(QPaintEvent* event)
{
    if (drawing_) {
        QPainter painter(this);
        drawing_->Draw(painter);
    }
    //ContainerVisual::paintEvent(event);
}
