#ifndef QTPAINTERDRAWINGCONTEXT_H
#define QTPAINTERDRAWINGCONTEXT_H

#include "Windows/Media/drawingcontext.h"

class QPainter;

INKCANVAS_BEGIN_NAMESPACE

class QtPainterDrawingContext : public DrawingContext
{
public:
    QtPainterDrawingContext(QPainter& painter);

public:
    virtual void DrawLine(
        QPen pen,
        Point const & point0,
        Point const & point1) override;

    virtual void DrawLine(
        QPen pen,
        Point const & point0,
        AnimationClock * point0Animations,
        Point const & point1,
        AnimationClock * point1Animations) override;

    virtual void DrawRectangle(
        QBrush brush,
        QPen pen,
        Rect const & rectangle) override;

    virtual void DrawRectangle(
        QBrush brush,
        QPen pen,
        Rect const & rectangle,
        AnimationClock * rectangleAnimations) override;

    virtual void DrawRoundedRectangle(
        QBrush brush,
        QPen pen,
        Rect const & rectangle,
        double radiusX,
        double radiusY) override;

    virtual void DrawRoundedRectangle(
        QBrush brush,
        QPen pen,
        Rect const & rectangle,
        AnimationClock * rectangleAnimations,
        double radiusX,
        AnimationClock * radiusXAnimations,
        double radiusY,
        AnimationClock * radiusYAnimations) override;

    virtual void DrawEllipse(
        QBrush brush,
        QPen pen,
        Point const & center,
        double radiusX,
        double radiusY) override;

    virtual void DrawEllipse(
        QBrush brush,
        QPen pen,
        Point const & center,
        AnimationClock * centerAnimations,
        double radiusX,
        AnimationClock * radiusXAnimations,
        double radiusY,
        AnimationClock * radiusYAnimations) override;

    virtual void DrawGeometry(
        QBrush brush,
        QPen pen,
        Geometry * geometry) override;

    virtual void DrawImage(
        QImage imageSource,
        Rect const & rectangle) override;

    virtual void DrawImage(
        QImage imageSource,
        Rect const & rectangle,
        AnimationClock * rectangleAnimations) override;

    virtual void DrawDrawing(
        Drawing * drawing) override;

    virtual void PushClip(
        Geometry * clipGeometry) override;

    virtual void PushOpacityMask(QBrush brush) override;

    virtual void PushOpacity(
        double opacity
        ) override;

    virtual void PushOpacity(
        double opacity,
        AnimationClock * opacityAnimations) override;

    virtual void PushTransform(
        Matrix const & transform) override;

    virtual void Pop() override;

    virtual void Close() override;

private:
    QPainter& painter_;
};

INKCANVAS_END_NAMESPACE

#endif // QTPAINTERDRAWINGCONTEXT_H
