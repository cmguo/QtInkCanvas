#ifndef WINDOWS_INPUT_MOUSEBUTTONSTATE_H
#define WINDOWS_INPUT_MOUSEBUTTONSTATE_H

#include "InkCanvas_global.h"

INKCANVAS_BEGIN_NAMESPACE

enum class MouseButtonState
{
    /// <summary>
    ///    The button is released.
    /// </summary>
    Released,

    /// <summary>
    ///    The button is pressed.
    /// </summary>
    Pressed,

    // Update the IsValid helper in RawMouseState.cs if this enum changes.
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INPUT_MOUSEBUTTONSTATE_H
