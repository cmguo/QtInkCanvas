#ifndef DRAWINGVISUAL_H
#define DRAWINGVISUAL_H

#include "containervisual.h"

class DrawingContext;
class DrawingGroup;

// namespace System.Windows.Media

class DrawingVisual : public ContainerVisual
{
public:
    DrawingVisual();

    DrawingContext * RenderOpen();

private:
    DrawingGroup * drawing_;
};

#endif // DRAWINGVISUAL_H
