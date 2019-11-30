#include "dynamicrenderer.h"

DynamicRenderer::DynamicRenderer()
{

}

bool DynamicRenderer::Enabled()
{
    return false;
}

void DynamicRenderer::SetEnabled(bool value)
{
}

void DynamicRenderer::Reset(StylusDevice*, QSharedPointer<StylusPointCollection>)
{
}

void DynamicRenderer::SetDrawingAttributes(QSharedPointer<DrawingAttributes>)
{
}

UIElement* DynamicRenderer::RootVisual()
{
    return nullptr;
}
