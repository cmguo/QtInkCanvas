#include "debug.h"

#include <assert.h>

void Debug::Assert(bool condition)
{
    assert(condition);
}

void Debug::Assert(bool condition, char const * message)
{
    assert(condition);
}
