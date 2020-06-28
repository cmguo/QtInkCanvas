#ifndef DRAWINGFLAGS_H
#define DRAWINGFLAGS_H

#include "InkCanvas_global.h"

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

enum class DrawingFlag
{
    /// <summary>The stroke should be drawn as a polyline</summary>
    Polyline = 0x00000000,
    /// <summary>The stroke should be fit to a curve, such as a bezier.</summary>
    FitToCurve = 0x00000001,
    /// <summary>The stroke should be rendered by subtracting its rendering values
    /// from those on the screen</summary>
    SubtractiveTransparency = 0x00000002,
    /// <summary>Ignore any stylus pressure information when rendering</summary>
    IgnorePressure = 0x00000004,
    /// <summary>The stroke should be rendered with anti-aliased edges</summary>
    AntiAliased = 0x00000010,
    /// <summary>Ignore any stylus rotation information when rendering</summary>
    IgnoreRotation = 0x00000020,
    /// <summary>Ignore any stylus angle information when rendering</summary>
    IgnoreAngle = 0x00000040,
};

#ifdef INKCANVAS_CORE

typedef  DrawingFlag DrawingFlags;

inline DrawingFlag operator ~(DrawingFlag f)
{
    return static_cast<DrawingFlag>(~static_cast<int>(f));
}

inline DrawingFlag operator |(DrawingFlag l, DrawingFlag r)
{
    return static_cast<DrawingFlag>(static_cast<int>(l) | static_cast<int>(r));
}

inline DrawingFlag operator &(DrawingFlag l, DrawingFlag r)
{
    return static_cast<DrawingFlag>(static_cast<int>(l) & static_cast<int>(r));
}

inline DrawingFlag operator ^(DrawingFlag l, DrawingFlag r)
{
    return static_cast<DrawingFlag>(static_cast<int>(l) ^ static_cast<int>(r));
}

inline bool operator==(DrawingFlag l, int r)
{
    return static_cast<int>(l) == r;
}

inline bool operator!=(DrawingFlag l, int r)
{
    return static_cast<int>(l) != r;
}

inline bool operator==(int l, DrawingFlag r)
{
    return static_cast<DrawingFlag>(l) == r;
}

inline bool operator!=(int l, DrawingFlag r)
{
    return static_cast<DrawingFlag>(l) != r;
}

INKCANVAS_END_NAMESPACE

#else

INKCANVAS_END_NAMESPACE

#include <QFlags>
#include <QMetaType>

INKCANVAS_BEGIN_NAMESPACE

inline DrawingFlag operator ~(DrawingFlag f)
{
    return static_cast<DrawingFlag>(~static_cast<int>(f));
}

Q_DECLARE_FLAGS(DrawingFlags, DrawingFlag)

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingFlags)

INKCANVAS_END_NAMESPACE

Q_DECLARE_METATYPE(INKCANVAS_PREPEND_NAMESPACE(DrawingFlags))

#endif

#endif // DRAWINGFLAGS_H
