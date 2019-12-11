#include "Internal/debug.h"

#include <assert.h>

void Debug::_Assert(bool condition, char const * message)
{
    assert(condition);
}
