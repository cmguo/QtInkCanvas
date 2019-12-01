#ifndef DRAWINGFLAGS_H
#define DRAWINGFLAGS_H

#include <QFlags>
#include <QMetaType>

// namespace System.Windows.Ink

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

Q_DECLARE_FLAGS(DrawingFlags, DrawingFlag)

Q_DECLARE_METATYPE(DrawingFlags)

#endif // DRAWINGFLAGS_H
