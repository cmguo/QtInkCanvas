#ifndef DOUBLE_H
#define DOUBLE_H

#include "InkCanvas_global.h"

#include <cmath>

INKCANVAS_BEGIN_NAMESPACE

class Double
{
public:
    static constexpr double MinValue = -1.7976931348623157E+308;
    static constexpr double MaxValue = 1.7976931348623157E+308;

    // Note Epsilon should be a double whose hex representation is 0x1
    // on little endian machines.
    static constexpr double Epsilon  = 4.9406564584124654E-324;
//    static constexpr double NegativeInfinity = (double)-1.0 / (double)(0.0);
//    static constexpr double PositiveInfinity = (double)1.0 / (double)(0.0);
//    static constexpr double NaN = (double)0.0 / (double)0.0;
    static const double NegativeInfinity;
    static const double PositiveInfinity;
    static const double NaN;

    static bool IsInfinity(double d) {
        return (*(long*)(&d) & 0x7FFFFFFFFFFFFFFF) == 0x7FF0000000000000;
    }

    static bool IsPositiveInfinity(double d) {
        //Jit will generate inlineable code with this
        if (d == PositiveInfinity)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool IsNegativeInfinity(double d) {
        //Jit will generate inlineable code with this
        if (d == NegativeInfinity)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool IsNegative(double d) {
        return (*(uintptr_t*)(&d) & 0x8000000000000000) == 0x8000000000000000;
    }

    static bool IsNaN(double d)
    {
        return (*(uintptr_t*)(&d) & 0x7FFFFFFFFFFFFFFFL) > 0x7FF0000000000000L;
    }

    static bool IsNaN(float d)
    {
        return (*(uintptr_t*)(&d) & 0x7FFFFFFFFFFFFFFFL) > 0x7FF0000000000000L;
    }

    static int Compare(double l, double r) {
        if (l < r) return -1;
        if (l > r) return 1;
        if (l == r) return 0;

        // At least one of the values is NaN.
        if (IsNaN(l))
            return (IsNaN(r) ? 0 : -1);
        else
            return 1;
    }

    static bool Equals(double l, double r) {
        // This code below is written this way for performance reasons i.e the != and == check is intentional.
        if (l == r) {
            return true;
        }
        return IsNaN(l) && IsNaN(r);
    }
};

inline double Mod (double l, int r)
{
    return l - floor(l / r) * r;
}

INKCANVAS_END_NAMESPACE

#endif // DOUBLE_H
