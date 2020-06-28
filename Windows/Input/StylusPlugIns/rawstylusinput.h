#ifndef RAWSTYLUSINPUT_H
#define RAWSTYLUSINPUT_H

#include "Windows/Input/StylusPlugIns/rawstylusactions.h"
#include "Windows/Media/matrix.h"
#include "sharedptr.h"
#include "Collections/Generic/list.h"

class QEvent;
class QTouchEvent;
class QGraphicsSceneMouseEvent;

INKCANVAS_BEGIN_NAMESPACE

class RawStylusInputReport;
class StylusPlugInCollection;
class StylusPointCollection;
class StylusPlugIn;
class InputDevice;

class RawStylusInputCustomData
{
public:
    StylusPlugIn* Owner;
    void* Data;
};

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
        QEvent& event,
        Matrix const & tabletToElementTransform,
        StylusPlugInCollection* targetPlugInCollection);

    QEvent& inputEvent() { return inputEvent_; }

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

    RawStylusActions Actions();

    /// <summary>
    /// Returns a copy of the StylusPoints
    /// </summary>
    SharedPointer<StylusPointCollection> GetStylusPoints();

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
    SharedPointer<StylusPointCollection> GetStylusPoints(Matrix const & transform);

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
    void SetStylusPoints(SharedPointer<StylusPointCollection> stylusPoints);

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
    Matrix ElementTransform();
    /// <summary>
    /// Retrieves the RawStylusInputCustomDataList associated with this input.
    /// </summary>
    List<RawStylusInputCustomData> CustomDataList();
    /// <summary>
    /// StylusPlugIn that is adding a notify event.
    /// </summary>
    StylusPlugIn* CurrentNotifyPlugIn();
    void SetCurrentNotifyPlugIn(StylusPlugIn* value);
    /////////////////////////////////////////////////////////////////////

private:
    QEvent& inputEvent_;
    QTouchEvent* touchEvent_;
    QGraphicsSceneMouseEvent* mouseEvent_;

    //RawStylusInputReport*    _report;
    InputDevice* device_;
    Matrix _tabletToElementTransform;
    StylusPlugInCollection*  _targetPlugInCollection;
    SharedPointer<StylusPointCollection>   _stylusPoints;
    StylusPlugIn*            _currentNotifyPlugIn;
    List<RawStylusInputCustomData>    _customData;

};

INKCANVAS_END_NAMESPACE

#endif // RAWSTYLUSINPUT_H
