#include "qtpainterdrawingcontext.h"
#include "Windows/Media/geometry.h"
#include "Windows/Media/drawing.h"

#include <QPainter>

QtPainterDrawingContext::QtPainterDrawingContext(QPainter &painter)
    : painter_(painter)
{
}

void QtPainterDrawingContext::DrawLine(QPen pen, const QPointF &point0, const QPointF &point1)
{
    DrawLine(pen, point0, nullptr, point1, nullptr);
}

void QtPainterDrawingContext::DrawLine(QPen pen, const QPointF &point0, AnimationClock *point0Animations, const QPointF &point1, AnimationClock *point1Animations)
{
    (void) point0Animations;
    (void) point1Animations;
    QPen oldPen = painter_.pen();
    painter_.setPen(pen);
    painter_.drawLine(point0, point1);
    painter_.setPen(oldPen);
}

void QtPainterDrawingContext::DrawRectangle(QBrush brush, QPen pen, const QRectF &rectangle)
{
    DrawRectangle(brush, pen, rectangle, nullptr);
}

void QtPainterDrawingContext::DrawRectangle(QBrush brush, QPen pen, const QRectF &rectangle, AnimationClock *rectangleAnimations)
{
    (void) rectangleAnimations;
    QBrush oldBrush = painter_.brush();
    QPen oldPen = painter_.pen();
    painter_.setBrush(brush);
    painter_.setPen(pen);
    painter_.drawRect(rectangle);
    painter_.setPen(oldPen);
    painter_.setBrush(oldBrush);
}

void QtPainterDrawingContext::DrawRoundedRectangle(QBrush brush, QPen pen, const QRectF &rectangle, double radiusX, double radiusY)
{
    DrawRoundedRectangle(brush, pen, rectangle, nullptr, radiusX, nullptr, radiusY, nullptr);
}

void QtPainterDrawingContext::DrawRoundedRectangle(QBrush brush, QPen pen, const QRectF &rectangle, AnimationClock *rectangleAnimations, double radiusX, AnimationClock *radiusXAnimations, double radiusY, AnimationClock *radiusYAnimations)
{
    (void) rectangleAnimations;
    (void) radiusXAnimations;
    (void) radiusYAnimations;
    QBrush oldBrush = painter_.brush();
    QPen oldPen = painter_.pen();
    painter_.setBrush(brush);
    painter_.setPen(pen);
    painter_.drawRoundedRect(rectangle, radiusX, radiusY);
    painter_.setPen(oldPen);
    painter_.setBrush(oldBrush);
}

void QtPainterDrawingContext::DrawEllipse(QBrush brush, QPen pen, const QPointF &center, double radiusX, double radiusY)
{
    DrawEllipse(brush, pen, center, nullptr, radiusX, nullptr, radiusY, nullptr);
}

void QtPainterDrawingContext::DrawEllipse(QBrush brush, QPen pen, const QPointF &center, AnimationClock *centerAnimations, double radiusX, AnimationClock *radiusXAnimations, double radiusY, AnimationClock *radiusYAnimations)
{
    (void) centerAnimations;
    (void) radiusXAnimations;
    (void) radiusYAnimations;
    QBrush oldBrush = painter_.brush();
    QPen oldPen = painter_.pen();
    painter_.setBrush(brush);
    painter_.setPen(pen);
    painter_.drawEllipse(center, radiusX, radiusY);
    painter_.setPen(oldPen);
    painter_.setBrush(oldBrush);
}

void QtPainterDrawingContext::DrawGeometry(QBrush brush, QPen pen, Geometry *geometry)
{
    QBrush oldBrush = painter_.brush();
    QPen oldPen = painter_.pen();
    painter_.setBrush(brush);
    painter_.setPen(pen);
    geometry->Draw(painter_);
    if (geometry->tryTakeOwn(this))
        delete geometry;
    painter_.setPen(oldPen);
    painter_.setBrush(oldBrush);
}

void QtPainterDrawingContext::DrawImage(QImage imageSource, const QRectF &rectangle)
{
    DrawImage(imageSource, rectangle, nullptr);
}

void QtPainterDrawingContext::DrawImage(QImage imageSource, const QRectF &rectangle, AnimationClock *rectangleAnimations)
{
    (void) rectangleAnimations;
    painter_.drawImage(rectangle, imageSource);
}

void QtPainterDrawingContext::DrawDrawing(Drawing *drawing)
{
    drawing->Draw(painter_);
}

void QtPainterDrawingContext::PushClip(Geometry *clipGeometry)
{
    (void) clipGeometry;
    //painter_.setClipPath(clipGeometry->Clip());
}

void QtPainterDrawingContext::PushOpacityMask(QBrush brush)
{
    (void) brush;
}

void QtPainterDrawingContext::PushOpacity(double opacity)
{
    PushOpacity(opacity, nullptr);
}

void QtPainterDrawingContext::PushOpacity(double opacity, AnimationClock *opacityAnimations)
{
    (void) opacityAnimations;
    painter_.setOpacity(opacity);
}

void QtPainterDrawingContext::PushTransform(QTransform transform)
{
    painter_.setTransform(transform);
}

void QtPainterDrawingContext::Pop()
{
}

void QtPainterDrawingContext::Close()
{
}
