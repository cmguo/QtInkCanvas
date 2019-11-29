#ifndef DYNAMICRENDERER_H
#define DYNAMICRENDERER_H

#include "stylusplugin.h"

#include <QSharedPointer>

class StylusDevice;
class StylusPointCollection;
class DrawingAttributes;
class UIElement;

class DynamicRenderer : public StylusPlugIn
{
public:
    DynamicRenderer();

    bool Enabled();

    void SetEnabled(bool value);

    void Reset(StylusDevice*, QSharedPointer<StylusPointCollection>);

    void SetDrawingAttributes(QSharedPointer<DrawingAttributes>);

    UIElement* RootVisual();
};

#endif // DYNAMICRENDERER_H
