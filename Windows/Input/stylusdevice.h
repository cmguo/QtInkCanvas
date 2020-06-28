#ifndef STYLUSDEVICE_H
#define STYLUSDEVICE_H

#include "InkCanvas_global.h"

#include "Windows/Input/inputdevice.h"
#include "Windows/routedeventargs.h"
#include "Windows/point.h"
#include "sharedptr.h"

#include <QMap>

class QTouchDevice;
class QTouchEvent;
class QInputEvent;

INKCANVAS_BEGIN_NAMESPACE

class StylusPointCollection;
class StylusPointDescription;

class StylusDevice;
class Visual;

class StylusEvent : public RoutedEvent
{
public:
    StylusEvent(int type, int type2);
    virtual void handle(QEvent &event, QList<RoutedEventHandler> handlers) override;
    virtual void handle2(QEvent &event, QList<RoutedEventHandler> handlers) override;
};

class StylusPointPropertyInfo;

class INKCANVAS_EXPORT Stylus
{
public:
    static StylusDevice* CurrentStylusDevice();

    static StylusDevice* GetDevice(QTouchDevice* device);

    static StylusDevice* GetDevice(int id);

    static void SetGroupSize(QSizeF const & size);

    static QSizeF GetGroupSize();

    static void SetLastInput(QTouchEvent& input);

    static SharedPointer<StylusPointDescription> DefaultPointDescription();

    static StylusEvent StylusDownEvent;

    static StylusEvent StylusMoveEvent;

    static StylusEvent StylusUpEvent;

    static StylusPointPropertyInfo StylusPointIdPropertyInfo;

    static StylusDevice* CurrentDevice;

private:
    static QMap<QTouchDevice*, StylusDevice*> devices_;
    static QSizeF groupSize_;
};

class StylusGroup {
public:
    int groupId = 0;
    QRectF bound;
    QVector<int> allPointIds;
    QVector<int> pointIds;
    QVector<int> newPointIds;
};

class StylusDevice : public InputDevice
{
    Q_OBJECT
public:
    StylusDevice(QTouchDevice * device, int id);

    void SetLastPoints(QList<QTouchEvent::TouchPoint> const & points, bool reset);

    virtual int Id() override;

    bool InAir();

    bool Inverted();

    Point GetPosition(Visual* relativeTo);

    SharedPointer<StylusPointCollection> GetStylusPoints(Visual * visual);

    SharedPointer<StylusPointCollection> GetStylusPoints(Visual * visual, SharedPointer<StylusPointDescription> description);

    UIElement* Captured();

    virtual UIElement* Target() override;

    virtual PresentationSource* ActiveSource() override;

    virtual SharedPointer<StylusPointDescription> PointDescription() override;

    virtual Array<int> PacketData(QEvent& event) override;

    QMap<int, StylusGroup> const & StylusGroups() { return lastGroups_; }

private:
    int id_;
    QTouchDevice * device_;
    SharedPointer<StylusPointDescription> description_;
    QList<QTouchEvent::TouchPoint> lastPoints_;
    QMap<int, StylusGroup> lastGroups_;
    QMap<int, int> groupMap_;
};

INKCANVAS_END_NAMESPACE

#endif // STYLUSDEVICE_H
