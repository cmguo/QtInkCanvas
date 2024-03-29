#include "Windows/Input/pencontexts.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/StylusPlugIns/stylusplugin.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/uielement.h"
#include "Windows/Controls/inkcanvas.h"

#include <QTouchEvent>
#include <QGraphicsSceneMouseEvent>

INKCANVAS_BEGIN_NAMESPACE

PenContexts::PenContexts(UIElement * element)
    : mutex_(QMutex::Recursive)
    , element_(element)
{
    if (Mouse::PrimaryDevice == nullptr)
        Mouse::PrimaryDevice = new MouseDevice;
    element->installEventFilter(this);
}

QMutex& PenContexts::SyncRoot()
{
    return mutex_;
}

void PenContexts::AddStylusPlugInCollection(StylusPlugInCollection* pic)
{
    stylusPlugIns_.Add(pic);
}

void PenContexts::RemoveStylusPlugInCollection(StylusPlugInCollection* pic)
{
    stylusPlugIns_.Remove(pic);
}

void PenContexts::FireCustomData()
{
    for (RawStylusInputCustomData & cd : customDatas_) {
        cd.Owner->FireCustomData(cd.Data, action_, true);
    }
    customDatas_.Clear();
}

bool PenContexts::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
        if (qobject_cast<InkCanvas*>(element_)->ActiveEditingMode() == InkCanvasEditingMode::Ink) {
            QSizeF gs = qobject_cast<InkCanvas*>(element_)->property("StylusGroupSize").toSizeF();
            gs = element_->sceneTransform().inverted().mapRect(QRectF({0, 0}, gs)).size();
            Stylus::SetGroupSize(gs);
        } else {
            Stylus::SetGroupSize(QSizeF());
        }
        Q_FALLTHROUGH();
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        Stylus::SetLastInput(static_cast<QTouchEvent&>(*event));
        customDatas_.Clear();
        for (StylusPlugInCollection* pic : stylusPlugIns_) {
            RawStylusInput stylusInput(static_cast<QTouchEvent&>(*event), transform_, pic);
            pic->FireRawStylusInput(stylusInput);
            action_ = stylusInput.Actions();
            customDatas_.AddRange(stylusInput.CustomDataList());
        }
        break;
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        Mouse::SetLastInput(static_cast<QGraphicsSceneMouseEvent&>(*event));
        if (!element_->acceptTouchEvents() ||
                static_cast<QGraphicsSceneMouseEvent&>(*event).source() == Qt::MouseEventNotSynthesized) {
            customDatas_.Clear();
            for (StylusPlugInCollection* pic : stylusPlugIns_) {
                RawStylusInput stylusInput(static_cast<QGraphicsSceneMouseEvent&>(*event), transform_, pic);
                pic->FireRawStylusInput(stylusInput);
                action_ = stylusInput.Actions();
                customDatas_.AddRange(stylusInput.CustomDataList());
            }
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

INKCANVAS_END_NAMESPACE
