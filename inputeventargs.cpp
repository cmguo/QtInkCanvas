#include "inputeventargs.h"

/// <summary>
///     Initializes a new instance of the InputEventArgs class.
/// </summary>
/// <param name="inputDevice">
///     The input device to associate with this event.
/// </param>
/// <param name="timestamp">
///     The time when the input occured.
/// </param>
InputEventArgs::InputEventArgs(InputDevice* inputDevice, int timestamp)
{
    /* inputDevice parameter being null is valid*/
/* timestamp parameter is valuetype, need not be checked */
    _inputDevice = inputDevice;
    _timestamp = timestamp;
}

int InputEventArgs::_timestamp = 0;

StylusDevice* StylusEventArgs::GetStylusDevice()
{
    return (StylusDevice*) Device();
}

bool StylusEventArgs::Inverted()
{
    return false;
}
