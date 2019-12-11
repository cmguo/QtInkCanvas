#ifndef DEBUG_H
#define DEBUG_H

// namespace System.Diagnostics

class Debug
{
public:
    static inline void Assert(bool condition)
    {
#ifdef _DEBUG
        _Assert(condition, "");
#endif
    }

    static inline void Assert(bool condition, char const * message)
    {
#ifdef _DEBUG
        _Assert(condition, message);
#endif
    }

private:
    static void _Assert(bool condition, char const * message);
};

#endif // DEBUG_H
