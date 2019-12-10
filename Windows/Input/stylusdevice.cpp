#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluseventargs.h"
#include "Windows/routedeventargs.h"

#include <QTouchDevice>
#include <QTouchEvent>

StylusEvent::StylusEvent(int type)
    : RoutedEvent(type)
{
}

void StylusEvent::handle(QEvent &event, QList<RoutedEventHandler> handlers)
{
    StylusEventArgs args(static_cast<QTouchEvent&>(event));
    args.MarkAsUserInitiated();
    RoutedEvent::handle(event, args, handlers);
}

StylusDevice* Stylus::CurrentStylusDevice()
{
    return nullptr;
}

StylusDevice* Stylus::GetDevice(QTouchDevice* device)
{
    StylusDevice* sd = devices_.value(device);
    if (sd)
        return sd;
    sd = new StylusDevice(device, devices_.size() + 1);
    devices_.insert(device, sd);
    return sd;
}

QMap<QTouchDevice*, StylusDevice*> Stylus::devices_;

StylusEvent Stylus::StylusDownEvent(QEvent::TouchBegin);

StylusEvent Stylus::StylusMoveEvent(QEvent::TouchUpdate);

StylusEvent Stylus::StylusUpEvent(QEvent::TouchEnd);

StylusDevice::StylusDevice(QTouchDevice * device, int id)
    : id_(id)
    , device_(device)
{
    description_.reset(new StylusPointDescription());
}

int StylusDevice::Id()
{
    return id_;
}

bool StylusDevice::InAir()
{
    return false;
}

bool StylusDevice::Inverted()
{
    return false;
}

QPointF StylusDevice::GetPosition(Visual *relativeTo)
{
    return QPointF();
}

QSharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual * visual)
{
    return GetStylusPoints(visual, description_);
}

QSharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual * visual, QSharedPointer<StylusPointDescription> description)
{
    return nullptr;
}

UIElement* StylusDevice::Captured()
{
    return nullptr;
}

UIElement* StylusDevice::Target()
{
    return nullptr;
}

PresentationSource* StylusDevice::ActiveSource()
{
    return nullptr;
}


QSharedPointer<StylusPointDescription> StylusDevice::PointDescription()
{
    return description_;
}

QVector<int> StylusDevice::PacketData(QInputEvent& event)
{
    QTouchEvent& touchEvent(static_cast<QTouchEvent&>(event));
    QVector<int> data;
    for (QTouchEvent::TouchPoint const & pt : touchEvent.touchPoints()) {
        QPoint pt2 = pt.pos().toPoint();
        data.append(pt2.x());
        data.append(pt2.y());
        data.append(static_cast<int>(pt.pressure()));
    }
    return data;
}
