#ifndef STYLUSDEVICE_H
#define STYLUSDEVICE_H

#include "Windows/Input/inputdevice.h"
#include "Windows/routedeventargs.h"

#include <QMap>

class StylusPointCollection;
class StylusPointDescription;

class StylusDevice;
class QTouchDevice;
class Visual;

class StylusEvent : public RoutedEvent
{
public:
    StylusEvent(int type);
    virtual void handle(QEvent &event, QList<RoutedEventHandler> handlers) override;
};

class Stylus
{
public:
    static StylusDevice* CurrentStylusDevice();

    static StylusDevice* GetDevice(QTouchDevice* device);

    static StylusEvent StylusDownEvent;

    static StylusEvent StylusMoveEvent;

    static StylusEvent StylusUpEvent;

private:
    static QMap<QTouchDevice*, StylusDevice*> devices_;
};

class QTouchEvent;
class QInputEvent;

class StylusDevice : public InputDevice
{
    Q_OBJECT
public:
    StylusDevice(QTouchDevice * device, int id);

    virtual int Id() override;

    bool InAir();

    bool Inverted();

    QPointF GetPosition(Visual* relativeTo);

    QSharedPointer<StylusPointCollection> GetStylusPoints(Visual * visual);

    QSharedPointer<StylusPointCollection> GetStylusPoints(Visual * visual, QSharedPointer<StylusPointDescription> description);

    UIElement* Captured();

    virtual UIElement* Target();

    virtual PresentationSource* ActiveSource();

    virtual QSharedPointer<StylusPointDescription> PointDescription() override;

    virtual QVector<int> PacketData(QInputEvent& event) override;

private:
    int id_;
    QTouchDevice * device_;
    QSharedPointer<StylusPointDescription> description_;
};

#endif // STYLUSDEVICE_H
