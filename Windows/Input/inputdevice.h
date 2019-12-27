#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QPointF>
#include <QSharedPointer>
#include <QObject>

class UIElement;
class Visual;
class PresentationSource;
class RoutedEvent;
class QInputEvent;
class StylusPointDescription;

// namespace System.Windows.Input

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

    virtual QSharedPointer<StylusPointDescription> PointDescription() = 0;

    virtual QVector<int> PacketData(QEvent& event) = 0;
};

#endif // INPUTDEVICE_H
