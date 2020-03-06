#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluseventargs.h"
#include "Windows/routedeventargs.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/mousebuttoneventargs.h"
#include "Internal/doubleutil.h"

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
    return CurrentDevice;
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

StylusDevice *Stylus::GetDevice(int id)
{
    for (StylusDevice* sd : devices_) {
        if (sd->Id() == id)
            return sd;
    }
    return nullptr;
}

QSizeF Stylus::GetGroupSize()
{
    return groupSize_;
}

void Stylus::SetGroupSize(const QSizeF &size)
{
    groupSize_ = size;
}

void Stylus::SetLastInput(QTouchEvent& input)
{
    CurrentDevice = GetDevice(input.device());
    CurrentDevice->SetLastPoints(input.touchPoints(), input.type() == QEvent::TouchBegin);
}

QSharedPointer<StylusPointDescription> Stylus::DefaultPointDescription()
{
    static QSharedPointer<StylusPointDescription> description;
    if (description)
        return description;
    description.reset(new StylusPointDescription());
    QVector<StylusPointPropertyInfo> props = description->GetStylusPointProperties();
    props.append(Stylus::StylusPointIdPropertyInfo);
    description.reset(new StylusPointDescription(props, -1));
    return description;
}

QMap<QTouchDevice*, StylusDevice*> Stylus::devices_;
QSizeF Stylus::groupSize_;

StylusEvent Stylus::StylusDownEvent(QEvent::TouchBegin, QEvent::GraphicsSceneMousePress);

StylusEvent Stylus::StylusMoveEvent(QEvent::TouchUpdate, QEvent::GraphicsSceneMouseMove);

StylusEvent Stylus::StylusUpEvent(QEvent::TouchEnd, QEvent::GraphicsSceneMouseRelease);

StylusDevice* Stylus::CurrentDevice = nullptr;

StylusPointPropertyInfo Stylus::StylusPointIdPropertyInfo
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

void StylusDevice::SetLastPoints(const QList<QTouchEvent::TouchPoint> &points, bool reset)
{
    //qDebug() << points;
    QSizeF gs = Stylus::GetGroupSize();
    if (gs.isEmpty()) {
        lastPoints_ = points;
        return;
    }
    if (reset) {
        lastGroups_.clear();
        groupMap_.clear();
    }
    for (StylusGroup & g : lastGroups_) {
        g.bound = QRectF();
        g.pointIds.clear();
        g.newPointIds.clear();
    }
    for (QTouchEvent::TouchPoint const & tp : points) {
        int gid = groupMap_.value(tp.id(), 0);
        if (gid) {
            StylusGroup & g = lastGroups_[gid];
            g.bound = United(g.bound, tp.pos());
            g.pointIds.append(tp.id());
        }
    }
    for (QTouchEvent::TouchPoint const & tp : points) {
        int gid = groupMap_.value(tp.id(), 0);
        if (gid) continue;
        for (StylusGroup & g : lastGroups_) {
            if(g.pointIds.isEmpty()) continue;
            if (g.bound.contains(tp.pos())) {
                gid = g.groupId;
                break;
            }
            QRectF bound = United(g.bound, tp.pos());
            if (bound.width() <= gs.width() && bound.height() <= gs.height()) {
                gid = g.groupId;
                g.bound = bound;
                break;
            }
        }
        if (!gid) {
            gid = tp.id();
            StylusGroup & g = lastGroups_[gid];
            g.groupId = gid;
            g.bound = QRectF(tp.pos(), tp.pos());
        }
        StylusGroup & g = lastGroups_[gid];
        if (g.allPointIds.size() == 2 && g.newPointIds.empty()) {
            g.newPointIds.append(g.pointIds);
        }
        g.allPointIds.append(tp.id());
        g.pointIds.append(tp.id());
        g.newPointIds.append(tp.id());
        groupMap_.insert(tp.id(), gid);
    }
    lastPoints_.clear();
    for (QTouchEvent::TouchPoint const & tp : points) {
        int gid = groupMap_.value(tp.id(), 0);
        StylusGroup & g = lastGroups_[gid];
        if (g.allPointIds.size() <= 2) {
            lastPoints_.append(tp);
        }
    }
    //qDebug() << "----" << lastPoints_;
    for (StylusGroup & g : lastGroups_) {
        if (g.allPointIds.size() <= 2) {
            g.newPointIds.clear();
        } else {
            QPointF c = g.bound.center();
            QSizeF s = g.bound.size();
            if (s.width() > gs.width() || s.height() > gs.height()) {
                s = gs;
            } else if (s.width() < gs.width() / 5 && s.height() < gs.height() / 5) {
                s = gs / 5;
            } else if (s.width() * gs.height() < s.height() * gs.width()) {
                s.setWidth(s.height() * gs.width() / gs.height());
            } else {
                s.setHeight(s.width() * gs.height() / gs.width());
            }
            g.bound.setSize(s);
            g.bound.moveCenter(c);
        }
    }
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
            if (p.Id() == Stylus::StylusPointIdPropertyInfo.Id()) {
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

QVector<int> StylusDevice::PacketData(QEvent&)
{
    QVector<int> data;
    for (QTouchEvent::TouchPoint const & pt : lastPoints_) {
        QPoint pt2 = pt.pos().toPoint();
        data.append(pt2.x());
        data.append(pt2.y());
        data.append(static_cast<int>(pt.id()));
        //break; // single touch
    }
    return data;
}
