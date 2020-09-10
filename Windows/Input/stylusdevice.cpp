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

INKCANVAS_BEGIN_NAMESPACE

StylusEvent::StylusEvent(int type, int type2)
    : RoutedEvent(type, type2)
{
}

void StylusEvent::handle(QEvent &event, QList<RoutedEventHandler> handlers)
{
    StylusEventArgs args(static_cast<QTouchEvent&>(event));
    RoutedEvent::handle(event, args, handlers);
}

void StylusEvent::handle2(QEvent &event, QList<RoutedEventHandler> handlers)
{
    MouseButtonEventArgs args(static_cast<QTouchEvent&>(event));
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
    groupSize_ = size * 10;
}

void Stylus::SetLastInput(QTouchEvent& input)
{
    CurrentDevice = GetDevice(input.device());
    CurrentDevice->SetLastPoints(input.touchPoints(), input.type() == QEvent::TouchBegin);
}

SharedPointer<StylusPointDescription> Stylus::DefaultPointDescription()
{
    static SharedPointer<StylusPointDescription> description;
    if (description)
        return description;
    description.reset(new StylusPointDescription());
    List<StylusPointPropertyInfo> props = description->GetStylusPointProperties();
    props.Add(Stylus::StylusPointIdPropertyInfo);
    description.reset(new StylusPointDescription(props, -1));
    return description;
}

QMap<QTouchDevice*, StylusDevice*> Stylus::devices_;
QSizeF Stylus::groupSize_;

StylusEvent Stylus::StylusDownEvent(QEvent::TouchBegin, QEvent::GraphicsSceneMousePress);

StylusEvent Stylus::StylusMoveEvent(QEvent::TouchUpdate, QEvent::GraphicsSceneMouseMove);

StylusEvent Stylus::StylusUpEvent(QEvent::TouchEnd, QEvent::GraphicsSceneMouseRelease);

StylusDevice* Stylus::CurrentDevice = nullptr;

// {17E84F40-D488-4EDD-969E-C121DBE1EC83}
static const Guid StylusPointPropertyId =
    { 0x17e84f40u, 0xd488, 0x4edd, 0x96, 0x9e, 0xc1, 0x21, 0xdb, 0xe1, 0xec, 0x83 };

static const StylusPointProperty StylusPointProperty =
    { StylusPointPropertyId, false };

StylusPointPropertyInfo Stylus::StylusPointIdPropertyInfo
        (StylusPointProperty,
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

#define ERASER_FINGER_MIN_COUNT 3

#ifdef QT_DEBUG
#define MULTIPLE_STROKES_IN_ONE_GROUP 1
#else
#define MULTIPLE_STROKES_IN_ONE_GROUP 0
#endif

inline QRectF United(QRectF const & rect, QPointF const & point)
{
    if (rect.isNull()) {
        if (rect.x() == 0.0 && rect.y() == 0.0)
            return QRectF(point, point);
        else
            return QRectF(rect.topLeft(), point).normalized();
    } else {
        qreal l = 0;
        qreal t = 0;
        qreal r = 0;
        qreal b = 0;
        if (point.x() < rect.left())
            l = point.x() - rect.left();
        if (point.y() < rect.top())
            t = point.y() - rect.top();
        if (point.x() > rect.right())
            r = point.x() - rect.right();
        if (point.y() > rect.bottom())
            b = point.y() - rect.bottom();
        return rect.adjusted(l, t, r, b);
    }
}

void StylusDevice::SetLastPoints(const QList<QTouchEvent::TouchPoint> &points, bool reset)
{
    //qDebug() << points;
    QSizeF gs = Stylus::GetGroupSize();
    //qDebug() << "group size" << gs;
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
    // recalc with old points
    for (QTouchEvent::TouchPoint const & tp : points) {
        int gid = groupMap_.value(tp.id(), 0);
        if (gid) {
            StylusGroup & g = lastGroups_[gid];
            g.bound = United(g.bound, tp.pos());
            g.pointIds.append(tp.id());
        }
    }
    // update with new points
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
            //qDebug() << "new point" << tp << bound;
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
        if (g.allPointIds.size() == (ERASER_FINGER_MIN_COUNT - 1) && g.newPointIds.empty()) {
            g.newPointIds.append(g.pointIds);
        }
        g.allPointIds.append(tp.id());
        g.pointIds.append(tp.id());
        g.newPointIds.append(tp.id());
        groupMap_.insert(tp.id(), gid);
    }
    // non-eraser group points
    lastPoints_.clear();
    for (QTouchEvent::TouchPoint const & tp : points) {
        int gid = groupMap_.value(tp.id(), 0);
        StylusGroup & g = lastGroups_[gid];
#if MULTIPLE_STROKES_IN_ONE_GROUP
        if (g.allPointIds.size() < ERASER_FINGER_MIN_COUNT) {
#else
        if (g.allPointIds.size() == 1) {
#endif
            lastPoints_.append(tp);
        }
    }
    //qDebug() << "----" << lastPoints_;
    // adjust bound of eraser-group
    for (StylusGroup & g : lastGroups_) {
        if (g.allPointIds.size() < ERASER_FINGER_MIN_COUNT) {
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

Point StylusDevice::GetPosition(Visual *relativeTo)
{
    (void) relativeTo;
    return lastPoints_.first().pos();
}

SharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual * visual)
{
    return GetStylusPoints(visual, description_);
}

SharedPointer<StylusPointCollection> StylusDevice::GetStylusPoints(Visual *, SharedPointer<StylusPointDescription> description)
{
    if (description == nullptr)
        description = description_;
    SharedPointer<StylusPointCollection> points(new StylusPointCollection(description));
    for (QTouchEvent::TouchPoint const & pt : lastPoints_) {
        bool hasTouchId = false;
        QVector<int> d;
        List<StylusPointPropertyInfo> props = description->GetStylusPointProperties();
        for (int i = StylusPointDescription::RequiredCountOfProperties; i < props.Count(); ++i) {
            StylusPointPropertyInfo const & p = props[i];
            if (p.Id() == Stylus::StylusPointIdPropertyInfo.Id()) {
                d.append(pt.id());
                hasTouchId = true;
            }
        }
        StylusPoint point(pt.pos().x(), pt.pos().y(), StylusPoint::DefaultPressure, description, d);
        //qDebug() << "StylusDevice pressure" << lastPoints_.first().pressure();
        //point.SetPressureFactor(lastPoints_.first().pressure());
        points->Add(point);
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


SharedPointer<StylusPointDescription> StylusDevice::PointDescription()
{
    return description_;
}

Array<int> StylusDevice::PacketData(QEvent&)
{
    Array<int> data(lastPoints_.size() * 3);
    int i = 0;
    for (QTouchEvent::TouchPoint const & pt : lastPoints_) {
        QPoint pt2 = pt.pos().toPoint();
        data[i++] = (pt2.x());
        data[i++] = (pt2.y());
        data[i++] = (static_cast<int>(pt.id()));
        //break; // single touch
    }
    return data;
}

INKCANVAS_END_NAMESPACE
