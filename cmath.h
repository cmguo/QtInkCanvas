#ifndef CMATH_H
#define CMATH_H

#include "InkCanvas_global.h"

#include <cmath>
#include <algorithm>

INKCANVAS_BEGIN_NAMESPACE

class Math
{
public:
    static constexpr double PI = 3.14159265358979323846;

    static double Sqrt(double v)
    {
        return sqrt(v);
    }

    static double Abs(double v)
    {
        return abs(v);
    }

    static double Max(double v1, double v2)
    {
        return std::max(v1, v2);
    }

    static double Min(double v1, double v2)
    {
        return std::min(v1, v2);
    }

    static double Round(double v)
    {
        return std::round(v);
    }

    static double Sin(double v)
    {
        return sin(v);
    }

    static double Cos(double v)
    {
        return cos(v);
    }

    static double Tan(double v)
    {
        return tan(v);
    }

    static double Atan2(double y, double x)
    {
        return atan2(y, x);
    }
};

INKCANVAS_END_NAMESPACE

#endif // CMATH_H
