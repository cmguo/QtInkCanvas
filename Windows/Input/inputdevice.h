#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include "InkCanvas_global.h"
#include "Collections/Generic/array.h"
#include "sharedptr.h"

#include <QObject>

class QInputEvent;

// namespace System.Windows.Input
INKCANVAS_BEGIN_NAMESPACE

class UIElement;
class Visual;
class PresentationSource;
class RoutedEvent;
class StylusPointDescription;

/// <summary>
///     Provides the base class for all input devices.
/// </summary>
class InputDevice : public QObject
{
    Q_OBJECT
protected:
    /// <summary>
    ///     Constructs an instance of the InputDevice class.
    /// </summary>
    InputDevice()
    {
        // Only we can create these.
        //
    }

public:
    /// <summary>
    ///     Returns the element that input from this device is sent to.
    /// </summary>
    virtual UIElement* Target() = 0;

    /// <summary>
    ///     Returns the PresentationSource that is reporting input for this device.
    /// </summary>
    virtual PresentationSource* ActiveSource() = 0;

    virtual int Id() = 0;

    virtual SharedPointer<StylusPointDescription> PointDescription() = 0;

    virtual Array<int> PacketData(QEvent& event) = 0;
};

INKCANVAS_END_NAMESPACE

#endif // INPUTDEVICE_H
