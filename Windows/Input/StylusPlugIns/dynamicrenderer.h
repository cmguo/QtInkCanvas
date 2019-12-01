#ifndef DYNAMICRENDERER_H
#define DYNAMICRENDERER_H

#include "Windows/Input/StylusPlugIns/stylusplugin.h"
#include "eventargs.h"

#include <QSharedPointer>
#include <QMutex>
#include <QQueue>

class StylusDevice;
class StylusPointCollection;
class DrawingAttributes;
class Visual;
class EventArgs;
class DrawingContext;
class Geometry;
class Dispatcher;
class ContainerVisual;
class DynamicRendererThreadManager;

/////////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
class DynamicRenderer : public StylusPlugIn
{

    /////////////////////////////////////////////////////////////////////

private:
    class StrokeInfo;

private:
    class DynamicRendererHostVisual;

    /////////////////////////////////////////////////////////////////////

    /// <summary>
    /// [TBS] - On UIContext
    /// </summary>
public:
    DynamicRenderer();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Reset will stop the current strokes being dynamically rendered
    /// and start a new stroke with the packets passed in.  Specified StylusDevice
    /// must be in down position when calling this method.
    /// Only call from application dispatcher.
    /// </summary>
    /// <param name="stylusDevice">
    /// <param name="stylusPoints">
    virtual void Reset(StylusDevice* stylusDevice, QSharedPointer<StylusPointCollection> stylusPoints);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On app Dispatcher
    /// </summary>
    Visual* RootVisual();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On app Dispatcher
    /// </summary>
protected:
    virtual void OnAdded();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On app dispatcher
    /// </summary>
    virtual void OnRemoved();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On UIContext
    /// </summary>
    virtual void OnIsActiveForInputChanged();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On pen threads or app thread
    /// </summary>
    virtual void OnStylusEnter(RawStylusInput& rawStylusInput, bool confirmed);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On pen threads or app thread
    /// </summary>
    virtual void OnStylusLeave(RawStylusInput& rawStylusInput, bool confirmed);

private:
    void HandleStylusEnterLeave(RawStylusInput& rawStylusInput, bool isEnter, bool isConfirmed);

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On UIContext
    /// </summary>
    virtual void OnEnabledChanged();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    virtual void OnStylusDown(RawStylusInput& rawStylusInput);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    virtual void OnStylusMove(RawStylusInput& rawStylusInput);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    virtual void OnStylusUp(RawStylusInput& rawStylusInput);

private:
    bool IsStylusUp(int stylusId);

    /// [TBS]
    ///
    void OnRenderComplete();

    void RemoveDynamicRendererVisualAndNotifyWhenDone(StrokeInfo* si);


    void NotifyAppOfDRThreadRenderComplete(StrokeInfo* si);


    void OnDRThreadRenderComplete(EventArgs& e);


    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    void OnStylusDownProcessed(bool targetVerified);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    virtual void OnStylusUpProcessed(bool targetVerified);

    void OnInternalRenderComplete(EventArgs& e);


    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    void NotifyOnNextRenderComplete();

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    virtual void OnDraw(  DrawingContext& drawingContext,
                                    QSharedPointer<StylusPointCollection> stylusPoints,
                                    Geometry& geometry,
                                    QBrush fillBrush);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS]
    /// </summary>
    virtual void OnDrawingAttributesReplaced();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Retrieves the Dispatcher for the thread used for rendering dynamic strokes
    /// when receiving data from the stylus input thread(s).
    /// </summary>
    Dispatcher* GetDispatcher();

    /////////////////////////////////////////////////////////////////////

    void RenderPackets(QSharedPointer<StylusPointCollection> stylusPoints,  StrokeInfo* si);

    /////////////////////////////////////////////////////////////////////

    void AbortAllStrokes();


    // The starting point for doing flicker free rendering when transitioning a real time
    // stroke from the DynamicRenderer thread to the application thread.
    //
    // There's a multi-step process to do this.  We now alternate between the two host visuals
    // to do the transtion work.  Only one HostVisual can be doing a full transition at one time.
    // When ones busy the other one reverts back to just removing the real time visual without
    // doing the full flicker free work.
    //
    // Here's the steps for a full transition using a Single DynamicRendererHostVisual:
    //
    // 1) [UI Thread] Set HostVisual.Clip = zero rect and then wait for render complete of that
    // 2) [UI Thread] On RenderComplete gets hit - Call over to DR thread to remove real time visual
    // 3) [DR Thread] Removed real time stroke visual and wait for rendercomplete of that
    // 4) [DR Thread] On RenderComplete of that call back over to UI thread to let it know that's done
    // 5) [UI Thread] Reset HostVisual.Clip = null and wait for render complete of that
    // 6) [UI Thread] On rendercomplete - we done.  Mark this HostVisual as free.
    //
    // In the case of another stroke coming through before a previous transition has completed
    // then basically instead of starting with step 1 we jump to step 2 and when then on step 5
    // we mark the HostVisual free and we are done.
    //
    void TransitionStrokeVisuals(StrokeInfo* si, bool abortStroke);

private:
    // Figures out the correct DynamicRenderHostVisual to use.
    DynamicRendererHostVisual* GetCurrentHostVisual();


    // Removes ref from DynamicRendererHostVisual.
    void TransitionComplete(StrokeInfo* si);

    void RemoveStrokeInfo(StrokeInfo* si);

    StrokeInfo* FindStrokeInfo(int timestamp);

    /////////////////////////////////////////////////////////////////////

public:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    /// [TBS] - On UIContext
    /// </summary>
    QSharedPointer<DrawingAttributes> GetDrawingAttributes();
    void SetDrawingAttributes(QSharedPointer<DrawingAttributes> value);

    void CreateInkingVisuals();

    /// <summary>
    /// Create the visual target
    /// This method is called from the application context
    /// </summary>
    void CreateRealTimeVisuals();

    /// <summary>
    /// Unhoot the visual target.
    /// This method is called from the application Dispatcher
    /// </summary>
    void DestroyRealTimeVisuals();

    /////////////////////////////////////////////////////////////////////
private:
    Dispatcher*          _applicationDispatcher;
    Geometry*            _zeroSizedFrozenRect;
    QSharedPointer<DrawingAttributes>   _drawAttrsSource;
    QList<StrokeInfo*>            _strokeInfoList;

    // Visuals layout:
    //
    //  _mainContainerVisual (root of inking tree - RootVisual [on app Dispatcher])
    //     |
    //     +-- _mainRawInkDispatcher (app dispatcher based stylus events renderer here [on app dispatcher])
    //     |
    //     +-- _rawInkHostVisual1 (HostVisual for inking on separate thread [on app dispatcher])
    //     |          |
    //     |          +-- VisualTarget ([on RealTimeInkingDispatcher thread])
    //     |
    //     +-- _rawInkHostVisual2 (HostVisual for inking on separate thread [on app dispatcher])
    //                |
    //                +-- VisualTarget ([on RealTimeInkingDispatcher thread])
    //
    ContainerVisual*              _mainContainerVisual;
    ContainerVisual*              _mainRawInkContainerVisual;
    DynamicRendererHostVisual*    _rawInkHostVisual1;
    DynamicRendererHostVisual*    _rawInkHostVisual2;

    DynamicRendererHostVisual*            _currentHostVisual; // Current HV.

    // For OnRenderComplete support (for UI Thread)
    EventHandler  _onRenderComplete;
    bool          _waitingForRenderComplete;
    QMutex        __siLock;
    StrokeInfo*  _renderCompleteStrokeInfo;

    // On internal real time ink rendering thread.
    DynamicRendererThreadManager* _renderingThread;

    // For OnRenderComplete support (for DynamicRenderer Thread)
    EventHandler  _onDRThreadRenderComplete;
    bool          _waitingForDRThreadRenderComplete;
    QQueue<StrokeInfo*>    _renderCompleteDRThreadStrokeInfoList;

};

#endif // DYNAMICRENDERER_H
