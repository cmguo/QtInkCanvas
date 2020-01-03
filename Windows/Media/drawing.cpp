#include "Windows/Media/drawing.h"

#include <QPainter>
#include <QDebug>

static int count = 0;

Drawing::Drawing()
{
    qDebug() << "Drawing ++ " << ++count;
}

Drawing::~Drawing()
{
    qDebug() << "Drawing -- " << --count;
}

DrawingGroup::DrawingGroup()
{
}

DrawingGroup::~DrawingGroup()
{
    for (Drawing * d : children_)
        delete d;
    children_.clear();
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
    for (Drawing * d : children_)
        delete d;
    children_.clear();
    return new DrawingGroupDrawingContext(this);
}

QList<Drawing*>& DrawingGroup::Children()
{
    return children_;
}

QRectF DrawingGroup::Bounds()
{
    QRectF bounds;
    for (Drawing * d : children_)
        bounds |= d->Bounds();
    return bounds;
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
    if (geometry_ && geometry_->tryTakeOwn(this))
        delete geometry_;
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
    return geometry_->Bounds();
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


