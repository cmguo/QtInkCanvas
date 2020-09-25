#ifndef WINDOWS_MEDIA_DRAWING_H
#define WINDOWS_MEDIA_DRAWING_H

#include "Windows/Media/drawingdrawingcontext.h"
#include "Windows/rect.h"

class QPainter;
class QSvgRenderer;

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class DrawingContext;
class Drawing
{
public:
    Drawing();

    virtual ~Drawing();

    virtual Rect Bounds() = 0;

    virtual void Draw(QPainter& painer) = 0;

};

class DrawingGroup : public Drawing
{
public:
    DrawingGroup();

    virtual ~DrawingGroup() override;

    DrawingContext* Open();

    List<Drawing*>& Children();

    virtual Rect Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    friend class DrawingGroupDrawingContext;

    void Close(List<Drawing*> rootDrawingGroupChildren);

private:
    List<Drawing*> children_;
};

#include <QBrush>
#include <QPen>

class Geometry;

class GeometryDrawing : public Drawing
{
public:
    GeometryDrawing();

    virtual ~GeometryDrawing() override;

    void SetBrush(QBrush);

    void SetPen(QPen);

    void SetGeometry(Geometry *);

    virtual Rect Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    QBrush brush_;
    QPen pen_;
    Geometry * geometry_ = nullptr;
};

class ImageDrawing : public Drawing
{
public:
    ImageDrawing();

    virtual ~ImageDrawing() override;

    void SetImageSource(QImage);

    void SetRect(Rect);

    virtual Rect Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    QImage image_;
    Rect rect_ = Rect::Empty();
};

class SvgImageDrawing : public Drawing
{
public:
    SvgImageDrawing();

    virtual ~SvgImageDrawing() override;

    void SetImageSource(QSvgRenderer * renderer);

    void SetRect(Rect);

    virtual Rect Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    QSvgRenderer * renderer_;
    Rect rect_ = Rect::Empty();
};

class DrawingGroupDrawingContext : public DrawingDrawingContext
{
public:
    DrawingGroupDrawingContext(DrawingGroup* drawingGroup);
protected:
    void CloseCore(List<Drawing*> rootDrawingGroupChildren);

private:
    DrawingGroup* drawingGroup_ = nullptr;
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_MEDIA_DRAWING_H
