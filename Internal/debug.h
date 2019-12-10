#ifndef DEBUG_H
#define DEBUG_H

// namespace System.Diagnostics

class Debug
{
public:
    static void Assert(bool condition);
    static void Assert(bool condition, char const * message);
};

#endif // DEBUG_H
