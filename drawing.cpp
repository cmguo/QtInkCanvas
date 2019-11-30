#include "drawing.h"
#include "drawingdrawingcontext.h"

Drawing::Drawing()
{

}

DrawingGroup::DrawingGroup()
{
}

class DrawingGroupDrawingContext : public DrawingDrawingContext
{
public:
    DrawingGroupDrawingContext(DrawingGroup* drawingGroup)
    {

    }
protected:
    void CloseCore(QVector<Drawing*> rootDrawingGroupChildren)
    {
        drawingGroup_->Close(rootDrawingGroupChildren);
    }

private:
    DrawingGroup* drawingGroup_;
};

DrawingContext* DrawingGroup::Open()
{
    return new DrawingGroupDrawingContext(this);
}

QList<Drawing*>& DrawingGroup::Children()
{
    return children_;
}

QRectF DrawingGroup::Bounds()
{
    return QRectF();
}

void DrawingGroup::Close(QVector<Drawing*> rootDrawingGroupChildren)
{

}

GeometryDrawing::GeometryDrawing()
{

}

void GeometryDrawing::SetBrush(QBrush brush)
{
    brush_ = brush;
}

void GeometryDrawing::SetPen(QPen pen)
{
    pen_ = pen;
}

void GeometryDrawing::SetGeometry(Geometry * g)
{
    geometry_ = g;
}

QRectF GeometryDrawing::Bounds()
{
    return QRectF();
}

ImageDrawing::ImageDrawing()
{

}

void ImageDrawing::SetImageSource(QPixmap pixmap)
{
    image_ = pixmap;
}

void ImageDrawing::SetRect(QRectF r)
{
    rect_ = r;
}

QRectF ImageDrawing::Bounds()
{
    return QRectF();
}

