#ifndef DRAWINGVISUAL_H
#define DRAWINGVISUAL_H

#include "containervisual.h"

class DrawingContext;
class DrawingGroup;

// namespace System.Windows.Media

class DrawingVisual : public ContainerVisual
{
    Q_OBJECT
public:
    DrawingVisual();

    DrawingContext * RenderOpen();

    void RenderClose();

    DrawingGroup * GetDrawing();

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    DrawingGroup * drawing_ = nullptr;
};

#endif // DRAWINGVISUAL_H
