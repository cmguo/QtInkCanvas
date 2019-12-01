#ifndef RAWSTYLUSINPUT_H
#define RAWSTYLUSINPUT_H

#include <QTransform>
#include <QSharedPointer>

class RawStylusInputReport;
class StylusPlugInCollection;
class RawStylusInputCustomDataList;
class StylusPointCollection;
class StylusPlugIn;

/////////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
class RawStylusInput
{
public:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     [TBS]
    /// </summary>
    /// <param name="report">[TBS]
    /// <param name="tabletToElementTransform">[TBS]
    /// <param name="targetPlugInCollection">[TBS]
    RawStylusInput(
        RawStylusInputReport*    report,
        QTransform        tabletToElementTransform,
        StylusPlugInCollection* targetPlugInCollection);

    /// <summary>
    ///
    /// </summary>
    int StylusDeviceId();

    /// <summary>
    ///
    /// </summary>
    int TabletDeviceId();

    /// <summary>
    ///
    /// </summary>
    int Timestamp();

    /// <summary>
    /// Returns a copy of the StylusPoints
    /// </summary>
    QSharedPointer<StylusPointCollection> GetStylusPoints();

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
    QSharedPointer<StylusPointCollection> GetStylusPoints(QTransform transform);

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
    void SetStylusPoints(QSharedPointer<StylusPointCollection> stylusPoints);

    /// <summary>
    /// Returns the RawStylusInputCustomDataList used to notify plugins before
    /// PreviewStylus event has been processed by application.
    /// </summary>
    void NotifyWhenProcessed(void* callbackData);

    /// <summary>
    /// True if a StylusPlugIn has modifiedthe StylusPoints.
    /// </summary>
    bool StylusPointsModified();
    /// <summary>
    /// Target StylusPlugInCollection that real time pen input sent to.
    /// </summary>
    StylusPlugInCollection* Target();
    /// <summary>
    /// Real RawStylusInputReport that this report is generated from.
    /// </summary>
    RawStylusInputReport* Report();
    /// <summary>
    /// Matrix that was used for rawstylusinput packets.
    /// </summary>
    QTransform ElementTransform();
    /// <summary>
    /// Retrieves the RawStylusInputCustomDataList associated with this input.
    /// </summary>
    RawStylusInputCustomDataList* CustomDataList();
    /// <summary>
    /// StylusPlugIn that is adding a notify event.
    /// </summary>
    StylusPlugIn* CurrentNotifyPlugIn();
    void SetCurrentNotifyPlugIn(StylusPlugIn* value);
    /////////////////////////////////////////////////////////////////////

    RawStylusInputReport*    _report;
    QTransform        _tabletToElementTransform;
    StylusPlugInCollection*  _targetPlugInCollection;
    QSharedPointer<StylusPointCollection>   _stylusPoints;
    StylusPlugIn*            _currentNotifyPlugIn;
    RawStylusInputCustomDataList*    _customData;

};

#endif // RAWSTYLUSINPUT_H
