#ifndef EDITINGMODE_H
#define EDITINGMODE_H

enum InkCanvasEditingMode
{
    /// <summary>
    /// None
    /// </summary>
    None = 0,
    /// <summary>
    /// Ink
    /// </summary>
    Ink,
    /// <summary>
    /// GestureOnly
    /// </summary>
    GestureOnly,
    /// <summary>
    /// InkAndGesture
    /// </summary>
    InkAndGesture,
    /// <summary>
    /// Select
    /// </summary>
    Select,
    /// <summary>
    /// EraseByPoint
    /// </summary>
    EraseByPoint,
    /// <summary>
    /// EraseByStroke
    /// </summary>
    EraseByStroke,
};

// NOTICE-2004/10/13-WAYNEZEN,
// Whenever the InkCanvasEditingMode is modified, please update this EditingModeHelper.IsDefined.
class EditingModeHelper
{
public:
    // Helper like Enum.IsDefined,  for InkCanvasEditingMode::
    static bool IsDefined(InkCanvasEditingMode InkCanvasEditingMode)
    {
        return (InkCanvasEditingMode >= InkCanvasEditingMode::None && InkCanvasEditingMode <= InkCanvasEditingMode::EraseByStroke);
    }
};

#endif // EDITINGMODE_H
