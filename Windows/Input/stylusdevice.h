#ifndef STYLUSDEVICE_H
#define STYLUSDEVICE_H

#include "Windows/Input/inputdevice.h"
#include "Windows/routedeventargs.h"

#include <QMap>
#include <QList>

class StylusPointCollection;
class StylusPointDescription;

class StylusDevice;
class QTouchDevice;
class Visual;

class StylusEvent : public RoutedEvent
{
public:
    StylusEvent(int type, int type2);
    virtual void handle(QEvent &event, QList<RoutedEventHandler> handlers) override;
    virtual void handle2(QEvent &event, QList<RoutedEventHandler> handlers) override;
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

    void SetLastPoints(QList<QTouchEvent::TouchPoint> const & points);

    virtual int Id() override;

    bool InAir();

    bool Inverted();

    QPointF GetPosition(Visual* relativeTo);

    QSharedPointer<StylusPointCollection> GetStylusPoints(Visual * visual);

    QSharedPointer<StylusPointCollection> GetStylusPoints(Visual * visual, QSharedPointer<StylusPointDescription> description);

    UIElement* Captured();

    virtual UIElement* Target() override;

    virtual PresentationSource* ActiveSource() override;

    virtual QSharedPointer<StylusPointDescription> PointDescription() override;

    virtual QVector<int> PacketData(QEvent& event) override;

private:
    int id_;
    QTouchDevice * device_;
    QSharedPointer<StylusPointDescription> description_;
    QList<QTouchEvent::TouchPoint> lastPoints_;
};

#endif // STYLUSDEVICE_H
