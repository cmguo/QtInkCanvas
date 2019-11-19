#ifndef INKCANVAS_H
#define INKCANVAS_H

#include "InkCanvas_global.h"

class StylusPointCollection;

class INKCANVAS_EXPORT InkCanvas
{
public:
    InkCanvas();

    StylusPointCollection GetBezierStylusPoints();
};

#endif // INKCANVAS_H
