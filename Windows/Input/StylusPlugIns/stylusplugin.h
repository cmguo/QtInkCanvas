#ifndef STYLUSPLUGIN_H
#define STYLUSPLUGIN_H

#include "InkCanvas_global.h"

#include "Windows/Input/StylusPlugIns/rawstylusactions.h"

#include <QRectF>

// namespace System.Windows.Input.StylusPlugIns
INKCANVAS_BEGIN_NAMESPACE

class StylusPlugInCollection;
class UIElement;
class RawStylusInput;

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
class INKCANVAS_EXPORT StylusPlugIn
{
protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on Dispatcher
    /// </summary>
    StylusPlugIn();

    virtual ~StylusPlugIn();

public:
    /////////////////////////////////////////////////////////////////////
    // (in Dispatcher)
    void Added(StylusPlugInCollection* plugInCollection);

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on Dispatcher
    /// </summary>
    virtual void OnAdded();

public:
    /////////////////////////////////////////////////////////////////////
    // (in Dispatcher)
    void Removed();

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on Dispatcher
    /// </summary>
    virtual void OnRemoved();

public:
    /////////////////////////////////////////////////////////////////////
    // (in RTI Dispatcher)
    void StylusEnterLeave(bool isEnter, RawStylusInput& rawStylusInput, bool confirmed);

    /////////////////////////////////////////////////////////////////////
    // (in RTI Dispatcher)
    void RawStylusInput2(RawStylusInput& rawStylusInput);

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on RTI Dispatcher
    /// </summary>
    virtual void OnStylusEnter(RawStylusInput& rawStylusInput, bool confirmed);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on RTI Dispatcher
    /// </summary>
    virtual void OnStylusLeave(RawStylusInput& rawStylusInput, bool confirmed);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on RTI Dispatcher
    /// </summary>
    virtual void OnStylusDown(RawStylusInput& rawStylusInput);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on RTI Dispatcher
    /// </summary>
    virtual void OnStylusMove(RawStylusInput& rawStylusInput);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on RTI Dispatcher
    /// </summary>
    virtual void OnStylusUp(RawStylusInput& rawStylusInput);

public:
    /////////////////////////////////////////////////////////////////////
    // (on app Dispatcher)
    void FireCustomData(void* callbackData, RawStylusActions& action, bool targetVerified);

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on app Dispatcher
    /// </summary>
    virtual void OnStylusDownProcessed(void* callbackData, bool targetVerified);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on app Dispatcher
    /// </summary>
    virtual void OnStylusMoveProcessed(void* callbackData, bool targetVerified);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on app Dispatcher
    /// </summary>
    virtual void OnStylusUpProcessed(void* callbackData, bool targetVerified);

public:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - both Dispatchers
    /// </summary>
    UIElement* GetElement();
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - both Dispatchers
    /// </summary>
    QRectF ElementBounds();
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - get - both Dispatchers, set Dispatcher
    /// </summary>
    bool Enabled();
    void SetEnabled(bool value);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on app Dispatcher
    /// </summary>
    virtual void OnEnabledChanged();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - app Dispatcher
    /// </summary>
    void InvalidateIsActiveForInput();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - get - both Dispatchers
    /// </summary>
    bool IsActiveForInput();

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - on Dispatcher
    /// </summary>
    virtual void OnIsActiveForInputChanged();

private:
    // Enabled state is local to this plugin so we just use volatile versus creating a lock
    // around it since we just read it from multiple thread and write from one.
    volatile bool                  __enabled = true;
    bool                            _activeForInput = false;
    StylusPlugInCollection*  _pic = nullptr;

};

INKCANVAS_END_NAMESPACE

#endif // STYLUSPLUGIN_H
