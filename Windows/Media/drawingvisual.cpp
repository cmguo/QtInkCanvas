#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/drawing.h"

DrawingVisual::DrawingVisual()
{

}

DrawingContext * DrawingVisual::RenderOpen()
{
    return drawing_->Open();
}

