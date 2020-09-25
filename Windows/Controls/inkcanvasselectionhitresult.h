#ifndef WINDOWS_CONTROLS_INKCANVASSELECTIONHITRESULT_H
#define WINDOWS_CONTROLS_INKCANVASSELECTIONHITRESULT_H

#include "InkCanvas_global.h"

// namespace System.Windows.Controls
INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// InkCanvas Selection Hit Result
/// </summary>
enum class InkCanvasSelectionHitResult
{
    /// <summary>
    /// None
    /// </summary>
    None =          0,
    /// <summary>
    /// TopLeft
    /// </summary>
    TopLeft =       1,
    /// <summary>
    /// Top
    /// </summary>
    Top =           2,
    /// <summary>
    /// TopRight
    /// </summary>
    TopRight =      3,
    /// <summary>
    /// Right
    /// </summary>
    Right =         4,
    /// <summary>
    /// BottomRight
    /// </summary>
    BottomRight =   5,
    /// <summary>
    /// Bottom
    /// </summary>
    Bottom =        6,
    /// <summary>
    /// BottomLeft
    /// </summary>
    BottomLeft =    7,
    /// <summary>
    /// Left
    /// </summary>
    Left =          8,
    /// <summary>
    /// Selection
    /// </summary>
    Selection =     9,
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_CONTROLS_INKCANVASSELECTIONHITRESULT_H
