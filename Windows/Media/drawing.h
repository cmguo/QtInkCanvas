#ifndef DRAWING_H
#define DRAWING_H

#include "Windows/Media/drawingdrawingcontext.h"

#include <QRectF>
#include <QList>

class DrawingContext;
class QPainter;

// namespace System.Windows.Media

class Drawing
{
public:
    Drawing();

    virtual ~Drawing() {}

    virtual QRectF Bounds() = 0;

    virtual void Draw(QPainter& painer) = 0;

};

class DrawingGroup : public Drawing
{
public:
    DrawingGroup();

    virtual ~DrawingGroup() override;

    DrawingContext* Open();

    QList<Drawing*>& Children();

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    friend class DrawingGroupDrawingContext;

    void Close(QList<Drawing*> rootDrawingGroupChildren);

private:
    QList<Drawing*> children_;
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

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    QBrush brush_;
    QPen pen_;
    Geometry * geometry_;
};

class ImageDrawing : public Drawing
{
public:
    ImageDrawing();

    virtual ~ImageDrawing() override;

    void SetImageSource(QPixmap);

    void SetRect(QRectF);

    virtual QRectF Bounds() override;

    virtual void Draw(QPainter& painer) override;

private:
    QPixmap image_;
    QRectF rect_;
};

class DrawingGroupDrawingContext : public DrawingDrawingContext
{
public:
    DrawingGroupDrawingContext(DrawingGroup* drawingGroup);
protected:
    void CloseCore(QList<Drawing*> rootDrawingGroupChildren);

private:
    DrawingGroup* drawingGroup_;
};


#endif // DRAWING_H
