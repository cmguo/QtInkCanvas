#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluseventargs.h"
#include "Windows/routedeventargs.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/mousebuttoneventargs.h"

#include <QTouchDevice>
#include <QTouchEvent>
#include <QDebug>

StylusEvent::StylusEvent(int type, int type2)
    : RoutedEvent(type, type2)
{
}

void StylusEvent::handle(QEvent &event, QList<RoutedEventHandler> handlers)
{
    StylusEventArgs args(static_cast<QTouchEvent&>(event));
    args.MarkAsUserInitiated();
    RoutedEvent::handle(event, args, handlers);
}

void StylusEvent::handle2(QEvent &event, QList<RoutedEventHandler> handlers)
{
    MouseButtonEventArgs args(static_cast<QTouchEvent&>(event));
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

StylusEvent Stylus::StylusDownEvent(QEvent::TouchBegin, QEvent::GraphicsSceneMousePress);

StylusEvent Stylus::StylusMoveEvent(QEvent::TouchUpdate, QEvent::GraphicsSceneMouseMove);

StylusEvent Stylus::StylusUpEvent(QEvent::TouchEnd, QEvent::GraphicsSceneMouseRelease);

StylusDevice::StylusDevice(QTouchDevice * device, int id)
    : id_(id)
    , device_(device)
{
    description_.reset(new StylusPointDescription());
    description_.reset(
                new StylusPointDescription(description_->GetStylusPointProperties(), -1));
}

void StylusDevice::SetLastPoints(const QList<QTouchEvent::TouchPoint> &points)
{
    lastPoints_ = points;
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
    (void) relativeTo;
    return lastPoints_.first().pos();
}

QSharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual * visual)
{
    return GetStylusPoints(visual, description_);
}

QSharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual * visual, QSharedPointer<StylusPointDescription> description)
{
    if (description == nullptr)
        description = description_;
    QSharedPointer<StylusPointCollection> points(new StylusPointCollection(description));
    StylusPoint point(GetPosition(visual));
    //qDebug() << "StylusDevice pressure" << lastPoints_.first().pressure();
    //point.SetPressureFactor(lastPoints_.first().pressure());
    points->AddItem(point);
    return points;
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

QVector<int> StylusDevice::PacketData(QEvent& event)
{
    QTouchEvent& touchEvent(static_cast<QTouchEvent&>(event));
    QVector<int> data;
    for (QTouchEvent::TouchPoint const & pt : touchEvent.touchPoints()) {
        QPoint pt2 = pt.pos().toPoint();
        data.append(pt2.x());
        data.append(pt2.y());
        //data.append(static_cast<int>(pt.pressure()));
        break; // single touch
    }
    return data;
}
