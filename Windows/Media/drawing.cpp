#include "Windows/Media/drawing.h"

#include <QPainter>

Drawing::Drawing()
{
}

DrawingGroup::DrawingGroup()
{
}

DrawingGroup::~DrawingGroup()
{
}


DrawingGroupDrawingContext::DrawingGroupDrawingContext(DrawingGroup* drawingGroup)
    : drawingGroup_(drawingGroup)
{
}

void DrawingGroupDrawingContext::CloseCore(QList<Drawing*> rootDrawingGroupChildren)
{
    drawingGroup_->Close(rootDrawingGroupChildren);
}


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

void DrawingGroup::Draw(QPainter &painer)
{
    for (Drawing * d : children_)
        d->Draw(painer);
}

void DrawingGroup::Close(QList<Drawing*> rootDrawingGroupChildren)
{
    children_.swap(rootDrawingGroupChildren);
}

GeometryDrawing::GeometryDrawing()
{
}

GeometryDrawing::~GeometryDrawing()
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

void GeometryDrawing::Draw(QPainter &painer)
{
    painer.save();
    painer.setPen(pen_);
    painer.setBrush(brush_);
    geometry_->Draw(painer);
    painer.restore();
}

ImageDrawing::ImageDrawing()
{
}

ImageDrawing::~ImageDrawing()
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

void ImageDrawing::Draw(QPainter &painer)
{

}

