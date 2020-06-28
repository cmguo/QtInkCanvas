#include "rect.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

Rect Rect::s_empty = CreateEmptyRect();

bool Rect::IsEmpty() const
{
    // The funny width and height tests are to handle NaNs
    Debug::Assert((!(_width < 0) && !(_height < 0)) || (*this == Empty()));

    return _width < 0;
}

INKCANVAS_END_NAMESPACE
