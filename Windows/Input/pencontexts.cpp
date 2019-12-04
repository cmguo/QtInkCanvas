#include "Windows/Input/pencontexts.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/StylusPlugIns/stylusplugin.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/uielement.h"

#include <QTouchEvent>
#include <QMouseEvent>

PenContexts::PenContexts(UIElement * element)
    : mutex_(QMutex::Recursive)
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
    stylusPlugIns_.append(pic);
}

void PenContexts::RemoveStylusPlugInCollection(StylusPlugInCollection* pic)
{
    stylusPlugIns_.removeOne(pic);
}

void PenContexts::FireCustomData()
{
    for (RawStylusInputCustomData & cd : customDatas_) {
        cd.Owner->FireCustomData(cd.Data, action_, true);
    }
    customDatas_.clear();
}

bool PenContexts::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        customDatas_.clear();
        for (StylusPlugInCollection* pic : stylusPlugIns_) {
            RawStylusInput stylusInput(static_cast<QTouchEvent&>(*event), transform_, pic);
            pic->FireRawStylusInput(stylusInput);
            action_ = stylusInput.Actions();
            customDatas_.append(stylusInput.CustomDataList());
        }
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
        customDatas_.clear();
        for (StylusPlugInCollection* pic : stylusPlugIns_) {
            RawStylusInput stylusInput(static_cast<QMouseEvent&>(*event), transform_, pic);
            pic->FireRawStylusInput(stylusInput);
            action_ = stylusInput.Actions();
            customDatas_.append(stylusInput.CustomDataList());
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

