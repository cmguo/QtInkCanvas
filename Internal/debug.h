#ifndef DEBUG_H
#define DEBUG_H

// namespace System.Diagnostics

#include <QtGlobal>

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

#endif // DEBUG_H
