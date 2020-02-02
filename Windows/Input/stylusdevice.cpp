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

QSharedPointer<StylusPointDescription> Stylus::DefaultPointDescription()
{
    static QSharedPointer<StylusPointDescription> description;
    if (description)
        return description;
    description.reset(new StylusPointDescription());
    QVector<StylusPointPropertyInfo> props = description->GetStylusPointProperties();
    props.append(Stylus::StylusIdPropertyInfo);
    description.reset(new StylusPointDescription(props, -1));
    return description;
}

QMap<QTouchDevice*, StylusDevice*> Stylus::devices_;

StylusEvent Stylus::StylusDownEvent(QEvent::TouchBegin, QEvent::GraphicsSceneMousePress);

StylusEvent Stylus::StylusMoveEvent(QEvent::TouchUpdate, QEvent::GraphicsSceneMouseMove);

StylusEvent Stylus::StylusUpEvent(QEvent::TouchEnd, QEvent::GraphicsSceneMouseRelease);

StylusPointPropertyInfo Stylus::StylusIdPropertyInfo
        (StylusPointProperty(QUuid::createUuid(), false),
                                    0,
                                    1,
                                    StylusPointPropertyUnit::None,
                                    1.0f);

StylusDevice::StylusDevice(QTouchDevice * device, int id)
    : id_(id)
    , device_(device)
{
    description_ = Stylus::DefaultPointDescription();
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

QSharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual *, QSharedPointer<StylusPointDescription> description)
{
    if (description == nullptr)
        description = description_;
    QSharedPointer<StylusPointCollection> points(new StylusPointCollection(description));
    for (QTouchEvent::TouchPoint const & pt : lastPoints_) {
        bool hasTouchId = false;
        QVector<int> d;
        QVector<StylusPointPropertyInfo> props = description->GetStylusPointProperties();
        for (int i = StylusPointDescription::RequiredCountOfProperties; i < props.size(); ++i) {
            StylusPointPropertyInfo const & p = props.at(i);
            if (p.Id() == Stylus::StylusIdPropertyInfo.Id()) {
                d.append(pt.id());
                hasTouchId = true;
            }
        }
        StylusPoint point(pt.pos().x(), pt.pos().y(), StylusPoint::DefaultPressure, description, d);
        //qDebug() << "StylusDevice pressure" << lastPoints_.first().pressure();
        //point.SetPressureFactor(lastPoints_.first().pressure());
        points->AddItem(point);
        if (!hasTouchId)
            break;
    }
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
        data.append(static_cast<int>(pt.id()));
        //break; // single touch
    }
    return data;
}
