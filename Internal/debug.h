#ifndef DEBUG_H
#define DEBUG_H

#include "InkCanvas_global.h"

// namespace System.Diagnostics
INKCANVAS_BEGIN_NAMESPACE

class Debug
{
public:
    static inline void Assert(bool condition)
    {
        (void) condition;
#ifdef QT_DEBUG
        _Assert(condition, "");
#endif
    }

    static inline void Assert(bool condition, char const * message)
    {
        (void) condition;
        (void) message;
#ifdef QT_DEBUG
        _Assert(condition, message);
#endif
    }

private:
    static void _Assert(bool condition, char const * message);
};

INKCANVAS_END_NAMESPACE

#endif // DEBUG_H
