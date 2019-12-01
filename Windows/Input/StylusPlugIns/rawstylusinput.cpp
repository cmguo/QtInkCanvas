#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/styluspointdescription.h"

/// <summary>
///     [TBS]
/// </summary>
/// <param name="report">[TBS]
/// <param name="tabletToElementTransform">[TBS]
/// <param name="targetPlugInCollection">[TBS]
RawStylusInput::RawStylusInput(
    RawStylusInputReport*    report,
    QTransform        tabletToElementTransform,
    StylusPlugInCollection* targetPlugInCollection)
{
    if (report == nullptr)
    {
        throw std::exception("report");
    }
    if (!tabletToElementTransform.isInvertible())
    {
        throw std::exception("tabletToElementTransform");
    }
    if (targetPlugInCollection == nullptr)
    {
        throw std::exception("targetPlugInCollection");
    }

    // We should always see this QTransform is frozen since we access this from multiple threads.
    //System.Diagnostics.Debug.Assert(tabletToElementTransform.IsFrozen);
    _report                 = report;
    _tabletToElementTransform  = tabletToElementTransform;
    _targetPlugInCollection = targetPlugInCollection;
}

/// <summary>
///
/// </summary>
int RawStylusInput::StylusDeviceId() { return _report->StylusDeviceId; }

/// <summary>
///
/// </summary>
int RawStylusInput::TabletDeviceId() { return _report->TabletDeviceId; }

/// <summary>
///
/// </summary>
int RawStylusInput::Timestamp() { return _report->Timestamp; }

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
        QTransformGroup group = new QTransformGroup();
        if ( StylusDeviceId() == 0)
        {
            // Only do this for the Mouse
            group.Children.Add(new MatrixTransform(_report->InputSource.CompositionTarget.TransformFromDevice));
        }
        group.Children.Add(_tabletToElementTransform);
        group.Children.Add(transform);
        return new StylusPointCollection(_report->StylusPointDescription, _report->GetRawPacketData(), group, Matrix.Identity);
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

    if (!StylusPointDescription::AreCompatible(  stylusPoints.Description,
                                                _report->StylusPointDescription))
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
    if (_customData == nullptr)
    {
        _customData = new RawStylusInputCustomDataList();
    }
    _customData->Add(new RawStylusInputCustomData(_currentNotifyPlugIn, callbackData));
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
     return _report;
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
RawStylusInputCustomDataList* RawStylusInput::CustomDataList()
{
     if (_customData == nullptr)
     {
         _customData = new RawStylusInputCustomDataList();
     }
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
