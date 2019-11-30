#include "drawingvisual.h"
#include "drawing.h"

DrawingVisual::DrawingVisual()
{

}

DrawingContext * DrawingVisual::RenderOpen()
{
    return drawing_->Open();
}

