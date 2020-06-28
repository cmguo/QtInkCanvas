#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"

#include <QTouchEvent>
#include <QGraphicsSceneMouseEvent>

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
///     [TBS]
/// </summary>
/// <param name="report">[TBS]
/// <param name="tabletToElementTransform">[TBS]
/// <param name="targetPlugInCollection">[TBS]
RawStylusInput::RawStylusInput(
    QEvent & event,
    Matrix const & tabletToElementTransform,
    StylusPlugInCollection* targetPlugInCollection)
    : inputEvent_(event)
    , touchEvent_(nullptr)
    , mouseEvent_(nullptr)
    , device_(nullptr)
{
    //if (report == nullptr)
    //{
    //    throw std::runtime_error("report");
    //}
    if (!tabletToElementTransform.HasInverse())
    {
        throw std::runtime_error("tabletToElementTransform");
    }
    if (targetPlugInCollection == nullptr)
    {
        throw std::runtime_error("targetPlugInCollection");
    }

    switch (event.type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        touchEvent_ = static_cast<QTouchEvent*>(&event);
        device_ = Stylus::GetDevice(touchEvent_->device());
        break;
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        mouseEvent_ = static_cast<QGraphicsSceneMouseEvent*>(&event);
        device_ = Mouse::PrimaryDevice;
        break;
    default:
        break;
    }

    // We should always see this Transform is frozen since we access this from multiple threads.
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
int RawStylusInput::Timestamp() { return touchEvent_ ? static_cast<int>(touchEvent_->timestamp()) : Mouse::GetTimestamp(); }

RawStylusActions RawStylusInput::Actions()
{
    switch (inputEvent_.type()) {
    case QEvent::TouchBegin:
    case QEvent::GraphicsSceneMousePress:
        return RawStylusActions::Down;
    case QEvent::TouchUpdate:
    case QEvent::GraphicsSceneMouseMove:
        return RawStylusActions::Move;
    case QEvent::TouchEnd:
    case QEvent::GraphicsSceneMouseRelease:
        return RawStylusActions::Up;
    default:
        return RawStylusActions::None;
    }
}

/// <summary>
/// Returns a copy of the StylusPoints
/// </summary>
SharedPointer<StylusPointCollection> RawStylusInput::GetStylusPoints()
{
    return GetStylusPoints(Matrix());
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
SharedPointer<StylusPointCollection> RawStylusInput::GetStylusPoints(Matrix const & transform)
{
    if (_stylusPoints == nullptr)
    {
        //
        Matrix group;
        //if ( StylusDeviceId() == 0)
        //{
        //    // Only do this for the Mouse
        //    group.Children.Add(new MatrixTransform(_report->InputSource.CompositionTarget.TransformFromDevice));
        //}
        group = _tabletToElementTransform;
        group *= transform;
        return SharedPointer<StylusPointCollection>(
                    new StylusPointCollection(device_->PointDescription(), device_->PacketData(inputEvent_), group, Matrix()));
    }
    else
    {
        return _stylusPoints->Clone(transform, _stylusPoints->Description());
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
void RawStylusInput::SetStylusPoints(SharedPointer<StylusPointCollection> stylusPoints)
{
    // To modify the points we require Unmanaged code permission.
    //SecurityHelper.DemandUnmanagedCode();

    if (nullptr == stylusPoints)
    {
        throw std::runtime_error("stylusPoints");
    }

    if (!StylusPointDescription::AreCompatible(  stylusPoints->Description(),
                                                device_->PointDescription()))
    {
        throw std::runtime_error("stylusPoints");
    }
    if (stylusPoints->Count() == 0)
    {
        throw std::runtime_error("stylusPoints");
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
        throw std::runtime_error("SR.Get(SRID.Stylus_CanOnlyCallForDownMoveOrUp)");
    }
    //if (_customData == nullptr)
    //{
    //    _customData = new RawStylusInputCustomDataList();
    //}
    _customData.Add(RawStylusInputCustomData{_currentNotifyPlugIn, callbackData});
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
Matrix RawStylusInput::ElementTransform()
{
     return _tabletToElementTransform;
}
/// <summary>
/// Retrieves the RawStylusInputCustomDataList associated with this input.
/// </summary>
List<RawStylusInputCustomData> RawStylusInput::CustomDataList()
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

INKCANVAS_END_NAMESPACE
