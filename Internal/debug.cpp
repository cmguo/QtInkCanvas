#include "Internal/debug.h"

#include <assert.h>

INKCANVAS_BEGIN_NAMESPACE

void Debug::_Assert(bool condition, char const * message)
{
    assert(condition);
}

INKCANVAS_END_NAMESPACE
