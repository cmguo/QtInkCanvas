#ifndef SINGLE_H
#define SINGLE_H

#include "InkCanvas_global.h"

#include <cmath>
#include <cstdint>

INKCANVAS_BEGIN_NAMESPACE

class Single
{
public:
    //
    // constants
    //
    static constexpr float MinValue = -3.40282346638528859e+38f;
    static constexpr float Epsilon = 1.4e-45f;
    static constexpr float MaxValue = 3.40282346638528859e+38f;
    static const float PositiveInfinity;
    static const float NegativeInfinity;
    static const float NaN;

    static bool IsInfinity(float f) {
        return (*reinterpret_cast<uint32_t*>(&f) & 0x7FFFFFFF) == 0x7F800000;
    }

    static bool IsPositiveInfinity(float f) {
        return *reinterpret_cast<uint32_t*>(&f) == 0x7F800000;
    }

    static bool IsNegativeInfinity(float f) {
        return *reinterpret_cast<uint32_t*>(&f) == 0xFF800000;
    }

    static bool IsNaN(float f) {
        return (*reinterpret_cast<uint32_t*>(&f) & 0x7FFFFFFF) > 0x7F800000;
    }

    // Compares this object to another object, returning an integer that
    // indicates the relationship.
    // Returns a value less than zero if this  object
    // null is considered to be less than any instance.
    // If object is not of type Single, this method throws an ArgumentException.
    //
    int Compare(float l, float r) {
        if (l < r) return -1;
        if (l > r) return 1;
        if (l == r) return 0;

        // At least one of the values is NaN.
        if (IsNaN(l))
            return (IsNaN(r) ? 0 : -1);
        else // f is NaN.
            return 1;
    }

    static bool Equals(float l, float r) {
        if (l == r) {
            return true;
        }
        return IsNaN(l) && IsNaN(r);
    }

};

INKCANVAS_END_NAMESPACE

#endif // SINGLE_H
