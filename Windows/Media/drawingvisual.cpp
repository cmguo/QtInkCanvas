#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/drawing.h"

#include <QPainter>
#include <QDebug>

DrawingVisual::DrawingVisual()
{
    resize(0, 0);
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
    QRect bounds = drawing_->Bounds().toRect().adjusted(-1, -1, 1, 1);
    if (width() < bounds.width() || height() < bounds.height()) {
        //qDebug() << "DrawingVisual::RenderClose" << this << bounds;
        resize(bounds.size());
        move(bounds.topLeft());
    } else if (x() > bounds.left() || y() > bounds.top() || x() + width() < bounds.right()
               || y() + height() < bounds.bottom()) {
        move(bounds.topLeft());
    }
    update();
}

DrawingGroup * DrawingVisual::GetDrawing()
{
    return drawing_;
}

void DrawingVisual::paintEvent(QPaintEvent* event)
{
    //qDebug() << "DrawingVisual::paintEvent" << this << event->rect();
    if (drawing_) {
        QPainter painter(this);
        painter.translate(-QPointF(pos()));
        drawing_->Draw(painter);
    }
    //ContainerVisual::paintEvent(event);
}
