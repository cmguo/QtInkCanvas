#ifndef DRAWING_H
#define DRAWING_H

#include <QRectF>
#include <QList>

class DrawingContext;

class Drawing
{
public:
    Drawing();

    virtual QRectF Bounds() = 0;
};

class DrawingGroup : public Drawing
{
public:
    DrawingGroup();

    DrawingContext* Open();

    QList<Drawing*>& Children();

    virtual QRectF Bounds() override;

private:
    friend class DrawingGroupDrawingContext;

    void Close(QVector<Drawing*> rootDrawingGroupChildren);

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

    void SetBrush(QBrush);

    void SetPen(QPen);

    void SetGeometry(Geometry *);

    virtual QRectF Bounds() override;

private:
    QBrush brush_;
    QPen pen_;
    Geometry * geometry_;
};

class ImageDrawing : public Drawing
{
public:
    ImageDrawing();

    void SetImageSource(QPixmap);

    void SetRect(QRectF);

    virtual QRectF Bounds() override;

private:
    QPixmap image_;
    QRectF rect_;
};

#endif // DRAWING_H
