#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"

#include <QTouchEvent>
#include <QMouseEvent>

/// <summary>
///     [TBS]
/// </summary>
/// <param name="report">[TBS]
/// <param name="tabletToElementTransform">[TBS]
/// <param name="targetPlugInCollection">[TBS]
RawStylusInput::RawStylusInput(
    QInputEvent&    event,
    QTransform        tabletToElementTransform,
    StylusPlugInCollection* targetPlugInCollection)
    : inputEvent_(event)
    , touchEvent_(nullptr)
    , mouseEvent_(nullptr)
    , device_(nullptr)
{
    //if (report == nullptr)
    //{
    //    throw std::exception("report");
    //}
    if (!tabletToElementTransform.isInvertible())
    {
        throw std::exception("tabletToElementTransform");
    }
    if (targetPlugInCollection == nullptr)
    {
        throw std::exception("targetPlugInCollection");
    }

    switch (event.type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        touchEvent_ = static_cast<QTouchEvent*>(&event);
        device_ = Stylus::GetDevice(touchEvent_->device());
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
        mouseEvent_ = static_cast<QMouseEvent*>(&event);
        device_ = Mouse::PrimaryDevice;
        break;
    default:
        break;
    }

    // We should always see this QTransform is frozen since we access this from multiple threads.
    //System.Diagnostics.Debug.Assert(tabletToElementTransform.IsFrozen);
    //_report                 = report;
    _tabletToElementTransform  = tabletToElementTransform;
    _targetPlugInCollection = targetPlugInCollection;
}

/// <summary>
///
/// </summary>
int RawStylusInput::StylusDeviceId()
{
    return device_->Id();
}

/// <summary>
///
/// </summary>
int RawStylusInput::TabletDeviceId() { return touchEvent_ ? touchEvent_->device()->type() : 0; }

/// <summary>
///
/// </summary>
int RawStylusInput::Timestamp() { return static_cast<int>(inputEvent_.timestamp()); }

RawStylusActions RawStylusInput::Actions()
{
    switch (inputEvent_.type()) {
    case QEvent::TouchBegin:
    case QEvent::MouseButtonPress:
        return RawStylusActions::Down;
    case QEvent::TouchUpdate:
    case QEvent::MouseMove:
        return RawStylusActions::Move;
    case QEvent::TouchEnd:
    case QEvent::MouseButtonRelease:
        return RawStylusActions::Up;
    default:
        return RawStylusActions::None;
    }
}

/// <summary>
/// Returns a copy of the StylusPoints
/// </summary>
QSharedPointer<StylusPointCollection> RawStylusInput::GetStylusPoints()
{
    return GetStylusPoints(QTransform());
}

/// <summary>
/// method called by StylusDevice to prevent two copies
/// </summary>
/// <securitynote>
///    Critical: This code accesses InputReport.InputSource
///    TreatAsSafe: The method only gets a transform from PresentationSource and returns StylusPointCollection.
///                 Those operations are considered safe.
///                 This method is called by:
///                                 RawStylusInput.GetStylusPoints
///                                 StylusDevice.UpdateEventStylusPoints(RawStylusInputReport, Boolean)
/// </securitynote>
//[SecurityCritical, SecurityTreatAsSafe]
QSharedPointer<StylusPointCollection> RawStylusInput::GetStylusPoints(QTransform transform)
{
    if (_stylusPoints == nullptr)
    {
        //
        QTransform group;
        //if ( StylusDeviceId() == 0)
        //{
        //    // Only do this for the Mouse
        //    group.Children.Add(new MatrixTransform(_report->InputSource.CompositionTarget.TransformFromDevice));
        //}
        group = _tabletToElementTransform;
        group *= transform;
        return QSharedPointer<StylusPointCollection>(
                    new StylusPointCollection(device_->PointDescription(), device_->PacketData(inputEvent_), group.toAffine(), QMatrix()));
    }
    else
    {
        return _stylusPoints->Clone(transform.toAffine(), _stylusPoints->Description());
    }
}

/// <summary>
/// Replaces the StylusPoints.
/// </summary>
/// <remarks>
///     Callers must have Unmanaged code permission to call this API.
/// </remarks>
/// <param name="stylusPoints">stylusPoints
/// <securitynote>
///     Callers must have Unmanaged code permission to call this API.
/// </securitynote>
void RawStylusInput::SetStylusPoints(QSharedPointer<StylusPointCollection> stylusPoints)
{
    // To modify the points we require Unmanaged code permission.
    //SecurityHelper.DemandUnmanagedCode();

    if (nullptr == stylusPoints)
    {
        throw std::exception("stylusPoints");
    }

    if (!StylusPointDescription::AreCompatible(  stylusPoints->Description(),
                                                device_->PointDescription()))
    {
        throw std::exception("stylusPoints");
    }
    if (stylusPoints->size() == 0)
    {
        throw std::exception("stylusPoints");
    }

    _stylusPoints = stylusPoints->Clone();
}

/// <summary>
/// Returns the RawStylusInputCustomDataList used to notify plugins before
/// PreviewStylus event has been processed by application.
/// </summary>
void RawStylusInput::NotifyWhenProcessed(void* callbackData)
{
    if (_currentNotifyPlugIn == nullptr)
    {
        throw std::exception("SR.Get(SRID.Stylus_CanOnlyCallForDownMoveOrUp)");
    }
    //if (_customData == nullptr)
    //{
    //    _customData = new RawStylusInputCustomDataList();
    //}
    _customData.append(RawStylusInputCustomData{_currentNotifyPlugIn, callbackData});
}

/// <summary>
/// True if a StylusPlugIn has modifiedthe StylusPoints.
/// </summary>
bool RawStylusInput::StylusPointsModified()
{
     return _stylusPoints != nullptr;
}
/// <summary>
/// Target StylusPlugInCollection that real time pen input sent to.
/// </summary>
StylusPlugInCollection* RawStylusInput::Target()
{
     return _targetPlugInCollection;
}
/// <summary>
/// Real RawStylusInputReport that this report is generated from.
/// </summary>
RawStylusInputReport* RawStylusInput::Report()
{
     return nullptr;
}
/// <summary>
/// Matrix that was used for rawstylusinput packets.
/// </summary>
QTransform RawStylusInput::ElementTransform()
{
     return _tabletToElementTransform;
}
/// <summary>
/// Retrieves the RawStylusInputCustomDataList associated with this input.
/// </summary>
QList<RawStylusInputCustomData> RawStylusInput::CustomDataList()
{
     // (_customData == nullptr)
     //{
     //    _customData = new RawStylusInputCustomDataList();
     //}
     return _customData;
}
/// <summary>
/// StylusPlugIn that is adding a notify event.
/// </summary>
StylusPlugIn* RawStylusInput::CurrentNotifyPlugIn()
{
     return _currentNotifyPlugIn;
}
void RawStylusInput::SetCurrentNotifyPlugIn(StylusPlugIn* value)
{
     _currentNotifyPlugIn = value;
}
