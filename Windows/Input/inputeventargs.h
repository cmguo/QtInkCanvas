#ifndef INPUTEVENTARGS_H
#define INPUTEVENTARGS_H

#include "Windows/routedeventargs.h"


class InputDevice;

// namespace System.Windows.Input

/// <summary>
///     The InputEventArgs class represents a type of RoutedEventArgs that
///     are relevant to all input events.
/// </summary>
class InputEventArgs : public RoutedEventArgs
{
public:
    /// <summary>
    ///     Initializes a new instance of the InputEventArgs class.
    /// </summary>
    /// <param name="inputDevice">
    ///     The input device to associate with this event.
    /// </param>
    /// <param name="timestamp">
    ///     The time when the input occured.
    /// </param>
    InputEventArgs(InputDevice* inputDevice, int timestamp);

    /// <summary>
    ///     Read-only access to the input device that initiated this
    ///     event.
    /// </summary>
    InputDevice* Device()
    {
        return _inputDevice;
    }
    void SetDevice(InputDevice* value)
    {
        _inputDevice = value;
    }

    /// <summary>
    ///     Read-only access to the input timestamp.
    /// </summary>
    int Timestamp()
    {
        return _timestamp;
    }


    /// <summary>
    ///     The mechanism used to call the type-specific handler on the
    ///     target.
    /// </summary>
    /// <param name="genericHandler">
    ///     The generic handler to call in a type-specific way.
    /// </param>
    /// <param name="genericTarget">
    ///     The target to call the handler on.
    /// </param>
    //virtual void InvokeEventHandler(Delegate genericHandler, QObject* genericTarget);

private:
    InputDevice* _inputDevice;
    static int _timestamp;
 };

class StylusDevice;

class StylusEventArgs : public InputEventArgs
{
public:
    StylusDevice* GetStylusDevice();

    bool Inverted();
};

#endif // INPUTEVENTARGS_H
