#include "Windows/Media/drawing.h"

#include <QPainter>
#include <QDebug>

#include <QtSvg/QSvgRenderer>

INKCANVAS_BEGIN_NAMESPACE

//static int count = 0;

Drawing::Drawing()
{
    //qDebug() << "Drawing ++ " << ++count;
}

Drawing::~Drawing()
{
    //qDebug() << "Drawing -- " << --count;
}

DrawingGroup::DrawingGroup()
{
}

DrawingGroup::~DrawingGroup()
{
    for (Drawing * d : children_)
        delete d;
    children_.Clear();
}

DrawingGroupDrawingContext::DrawingGroupDrawingContext(DrawingGroup* drawingGroup)
    : drawingGroup_(drawingGroup)
{
}

void DrawingGroupDrawingContext::CloseCore(List<Drawing*> rootDrawingGroupChildren)
{
    drawingGroup_->Close(rootDrawingGroupChildren);
}


DrawingContext* DrawingGroup::Open()
{
    for (Drawing * d : children_)
        delete d;
    children_.Clear();
    return new DrawingGroupDrawingContext(this);
}

List<Drawing*>& DrawingGroup::Children()
{
    return children_;
}

Rect DrawingGroup::Bounds()
{
    Rect bounds = Rect::Empty();
    for (Drawing * d : children_)
        bounds.Union(d->Bounds());
    return bounds;
}

void DrawingGroup::Draw(QPainter &painer)
{
    for (Drawing * d : children_)
        d->Draw(painer);
}

void DrawingGroup::Close(List<Drawing*> rootDrawingGroupChildren)
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
    geometry_ = nullptr;
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

Rect GeometryDrawing::Bounds()
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

void ImageDrawing::SetImageSource(QImage image)
{
    image_ = image;
}

void ImageDrawing::SetRect(Rect r)
{
    rect_ = r;
}

Rect ImageDrawing::Bounds()
{
    return rect_;
}

void ImageDrawing::Draw(QPainter &painer)
{
    painer.drawImage(rect_, image_);
}



SvgImageDrawing::SvgImageDrawing()
{
}

SvgImageDrawing::~SvgImageDrawing()
{
}

void SvgImageDrawing::SetImageSource(QSvgRenderer *renderer)
{
    renderer_ = renderer;
}

void SvgImageDrawing::SetRect(Rect r)
{
    rect_ = r;
}

Rect SvgImageDrawing::Bounds()
{
    return rect_;
}

void SvgImageDrawing::Draw(QPainter &painer)
{
    renderer_->render(&painer, rect_);
}

INKCANVAS_END_NAMESPACE
