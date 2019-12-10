#include "Windows/Input/StylusPlugIns/dynamicrenderer.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Internal/Ink/strokenodeiterator.h"
#include "Internal/Ink/strokenodeoperations.h"
#include "Internal/Ink/strokerenderer.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Media/geometry.h"
#include "Windows/Media/drawingcontext.h"
#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/containervisual.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/uielement.h"
#include "Windows/dispatcher.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QBrush>
#include <QThread>

#include <Windows.h>
#include <sysinfoapi.h>

class HostVisual;

class VisualTarget
{
public:
    VisualTarget(HostVisual* host)
        : host_(host)
        , root_(nullptr)
    {
    }

    Visual* RootVisual()
    {
        return root_;
    }

    void SetRootVisual(Visual* root)
    {
        root_ = root;
    }

private:
    HostVisual* host_;
    Visual * root_;
};

class HostVisual : public ContainerVisual
{
public:
    Visual* GetVisualTarget();
};

class DynamicRenderer::DynamicRendererHostVisual : public HostVisual
{
public:
    bool InUse()
    {
        return _strokeInfoList.size() > 0;
    }
    bool HasSingleReference()
    {
        return _strokeInfoList.size() == 1;
    }
    void AddStrokeInfoRef(StrokeInfo* si)
    {
        _strokeInfoList.append(si);
    }
    void RemoveStrokeInfoRef(StrokeInfo* si)
    {
        _strokeInfoList.removeOne(si);
    }
    /// <securitynote>
    /// Critical - Calls SecurityCritical method with LinkDemand (CompositionTarget.RootVisual).
    /// TreatAsSafe: You can't set the RootVisual to an arbitrary value.
    /// </securitynote>
    VisualTarget* GetVisualTarget()
    {
         if (_visualTarget == nullptr)
         {
             _visualTarget = new VisualTarget(this);
             _visualTarget->SetRootVisual(new ContainerVisual());
         }
         return _visualTarget;
    }

private:
    VisualTarget*       _visualTarget = nullptr;
    QList<StrokeInfo*>   _strokeInfoList;
};

class DynamicRenderer::StrokeInfo
{
    int _stylusId;
    int _startTime;
    int _lastTime;
    ContainerVisual* _strokeCV = nullptr;  // App thread rendering CV
    ContainerVisual* _strokeRTICV = nullptr; // real time input CV
    bool _seenUp = false; // Have we seen the stylusUp event yet?
    bool _isReset = false; // Was reset used to create this StrokeInfo?
    QBrush _fillBrush; // app thread based brushed
    QSharedPointer<DrawingAttributes> _drawingAttributes;
    StrokeNodeIterator _strokeNodeIterator;
    double _opacity;
    DynamicRendererHostVisual*   _strokeHV = nullptr;  // App thread rendering HostVisual

public:
    StrokeInfo(QSharedPointer<DrawingAttributes> drawingAttributes, int stylusDeviceId, int startTimestamp, DynamicRendererHostVisual* hostVisual)
        : _drawingAttributes(drawingAttributes->Clone())
        , _strokeNodeIterator(*_drawingAttributes)
    {
        _stylusId = stylusDeviceId;
        _startTime = startTimestamp;
        _lastTime = _startTime;
        //_drawingAttributes = drawingAttributes.Clone(); // stroke copy for duration of stroke.
        //_strokeNodeIterator = new StrokeNodeIterator(_drawingAttributes);
        QColor color = _drawingAttributes->Color();
        _opacity = _drawingAttributes->IsHighlighter() ? 0 : color.alpha() / StrokeRenderer::SolidStrokeAlpha;
        color.setAlpha(StrokeRenderer::SolidStrokeAlpha);

        // Set the brush to be used with this new stroke too (since frozen can be shared by threads)
        QBrush brush(color);
        //brush.Freeze();
        _fillBrush = brush;
        _strokeHV = hostVisual;
        hostVisual->AddStrokeInfoRef(this); // Add ourselves as reference.
    }

public:
    // Public props to access info
    int StylusId()
    {
        return _stylusId;
    }
    void SetStylusId(int value)
    {
        _stylusId = value;
    }
    int StartTime()
    {
        return _startTime;
    }
    int LastTime()
    {
        return _lastTime;
    }
    void SetLastTime(int value)
    {
        _lastTime = value;
    }
    ContainerVisual* StrokeCV()
    {
        return _strokeCV;
    }
    void SetStrokeCV(ContainerVisual* value)
    {
        if (value == nullptr) {
            if (_strokeCV)
                delete _strokeCV;
        } else {
            value->setObjectName("StrokeInfo::StrokeCV");
        }
        _strokeCV = value;
    }
    ContainerVisual* StrokeRTICV()
    {
        return _strokeRTICV;
    }
    void SetStrokeRTICV(ContainerVisual* value)
    {
        value->setObjectName("StrokeInfo::StrokeRTICV");
        _strokeRTICV = value;
    }
    bool SeenUp()
    {
        return _seenUp;
    }
    void SetSeenUp(bool value)
    {
        _seenUp = value;
    }
    bool IsReset()
    {
        return _isReset;
    }
    void SetIsReset(bool value)
    {
        _isReset = value;
    }
    StrokeNodeIterator& GetStrokeNodeIterator()
    {
        return _strokeNodeIterator;
    }
    void SetStrokeNodeIterator(StrokeNodeIterator && value)
    {
         //if (value == nullptr)
         //{
         //    throw std::exception();
         //}
         _strokeNodeIterator = std::move(value);
    }
    QBrush FillBrush()
    {
        return _fillBrush;
    }
    void SetFillBrush(QBrush value)
    {
        _fillBrush = value;
    }
    QSharedPointer<DrawingAttributes> GetDrawingAttributes()
    {
        return _drawingAttributes;
    }
    double Opacity()
    {
        return _opacity;
    }
    DynamicRendererHostVisual* StrokeHV()
    {
        return _strokeHV;
    }
    bool IsTimestampWithin(int timestamp)
    {
        // If we've seen up use the start and end to figure out if timestamp
        // is between start and last.  Note that we need to deal with the
        // times wrapping back to 0.
        if (SeenUp())
        {
            if (StartTime() < LastTime()) // wrapping check
            {
                return ((timestamp >= StartTime()) && (timestamp <= LastTime()));
            }
            else // The timestamp wrapped back to zero
            {
                return ((timestamp >= StartTime()) || (timestamp <= LastTime()));
            }
        }
        else
        {
            return true; // everything is consider part of an open StrokeInfo.
        }
    }
    bool IsTimestampAfter(int timestamp)
    {
        // If we've seen up then timestamp can't be after, otherwise do the check.
        // Note that we need to deal with the times wrapping (goes negative).
        if (!SeenUp())
        {
            if (LastTime() >= StartTime())
            {
                if (timestamp >= LastTime())
                {
                    return true;
                }
                else
                {
                    return ((LastTime() > 0) && (timestamp < 0));  // true if we wrapped
                }
            }
            else // The timestamp may have wrapped, see if greater than last time and less than start time
            {
                return timestamp >= LastTime() && timestamp <= StartTime();
            }
        }
        else
        {
            return false; // Nothing can be after a closed StrokeInfo (see up).
        }
    }
};

class VisualTarget;

DynamicRenderer::DynamicRenderer()
    : StylusPlugIn()
    , __siLock(QMutex::Recursive)
{
    _zeroSizedFrozenRect = new RectangleGeometry(QRectF(0,0,0,0));
    //_zeroSizedFrozenRect.Freeze();
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// Reset will stop the current strokes being dynamically rendered
/// and start a new stroke with the packets passed in.  Specified StylusDevice
/// must be in down position when calling this method.
/// Only call from application dispatcher.
/// </summary>
/// <param name="stylusDevice">
/// <param name="stylusPoints">
void DynamicRenderer::Reset(StylusDevice* stylusDevice, QSharedPointer<StylusPointCollection> stylusPoints)
{
    // NOTE: stylusDevice == nullptr means the mouse device.

    // Nothing to do if root visual not queried or not hookup up to element yet.
    if (_mainContainerVisual == nullptr || _applicationDispatcher == nullptr || !IsActiveForInput())
        return;

    // Ensure on UIContext.
    _applicationDispatcher->VerifyAccess();

    // Make sure the stylusdevice specified (or mouse if nullptr stylusdevice) is currently in
    // down state!
    bool inAir = (stylusDevice != nullptr) ?
                    stylusDevice->InAir() :
                    Mouse::PrimaryDevice->LeftButton() == MouseButtonState::Released;

    if (inAir)
    {
        throw std::exception("stylusDevice");
    }

    // Avoid reentrancy due to lock() call.
    //using(_applicationDispatcher->DisableProcessing())
    {
        //QMutexLocker l(&__siLock)
        {
            AbortAllStrokes(); // stop any current inking strokes

            // Now create new si and insert it in the list.
            StrokeInfo* si = new StrokeInfo(GetDrawingAttributes(),
                                           (stylusDevice != nullptr) ? stylusDevice->Id() : 0,
                                           ::GetTickCount(), GetCurrentHostVisual());
            _strokeInfoList.append(si);
            si->SetIsReset(true);

            if (stylusPoints != nullptr)
            {
                RenderPackets(stylusPoints, si); // do this inside of lock to make sure this renders first.
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On app Dispatcher
/// </summary>
Visual* DynamicRenderer::RootVisual()
{
    // NOTE: We don't create any visuals (real time or non real time) until someone
    //  queries for this property since we can't display anything until this is done and
    // they hook the returned visual up to their visual tree.
    if (_mainContainerVisual == nullptr)
    {
        CreateInkingVisuals(); // ensure at least the app dispatcher visuals are created.
    }
    return _mainContainerVisual;
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On app Dispatcher
/// </summary>

void DynamicRenderer::OnAdded()
{
    // Grab the dispatcher we're hookup up to.
    _applicationDispatcher = GetElement()->GetDispatcher();

    // If we are active for input, make sure we create the real time inking thread
    // and visuals if needed.
    if (IsActiveForInput())
    {
        CreateRealTimeVisuals();  // Transitions to inking thread.
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On app dispatcher
/// </summary>
void DynamicRenderer::OnRemoved()
{
    // Make sure we destroy any real time visuals and thread when removed.
    DestroyRealTimeVisuals();
    _applicationDispatcher = nullptr; // removed from tree.
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On UIContext
/// </summary>
void DynamicRenderer::OnIsActiveForInputChanged()
{
    // We only want to keep our real time inking thread references around only
    // when we need them.  If not enabled for input then we don't need them.
    if (IsActiveForInput())
    {
        // Make sure we create the real time inking visuals if we in tree.
        CreateRealTimeVisuals();  // Transitions to inking thread.
    }
    else
    {
        DestroyRealTimeVisuals();
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On pen threads or app thread
/// </summary>
void DynamicRenderer::OnStylusEnter(RawStylusInput& rawStylusInput, bool confirmed)
{
    HandleStylusEnterLeave(rawStylusInput, true, confirmed);
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On pen threads or app thread
/// </summary>
void DynamicRenderer::OnStylusLeave(RawStylusInput& rawStylusInput, bool confirmed)
{
    HandleStylusEnterLeave(rawStylusInput, false, confirmed);
}

void DynamicRenderer::HandleStylusEnterLeave(RawStylusInput& rawStylusInput, bool isEnter, bool isConfirmed)
{
    // See if we need to abort a stroke due to entering or leaving within a stroke.
    if (isConfirmed)
    {
        StrokeInfo* si = FindStrokeInfo(rawStylusInput.Timestamp());

        if (si != nullptr)
        {
            if (rawStylusInput.StylusDeviceId() == si->StylusId())
            {
                if ((isEnter && (rawStylusInput.Timestamp() > si->StartTime())) ||
                    (!isEnter && !si->SeenUp()))
                {
                    // abort this stroke.
                    TransitionStrokeVisuals(si, true);
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On UIContext
/// </summary>
void DynamicRenderer::OnEnabledChanged()
{
    // If going disabled cancel all real time strokes.  We won't be getting any more
    // events.
    if (!Enabled())
    {
        AbortAllStrokes();
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnStylusDown(RawStylusInput& rawStylusInput)
{
    // Only allow inking if someone has queried our RootVisual.
    if (_mainContainerVisual != nullptr)
    {
        StrokeInfo* si;

        {
            QMutexLocker l(&__siLock);
            si = FindStrokeInfo(rawStylusInput.Timestamp());

            // If we find we are already in the middle of stroke then bail out.
            // Can only ink with one stylus at a time.
            if (si != nullptr)
            {
                return;
            }

            si = new StrokeInfo(GetDrawingAttributes(), rawStylusInput.StylusDeviceId(), rawStylusInput.Timestamp(), GetCurrentHostVisual());
            _strokeInfoList.append(si);
        }

        rawStylusInput.NotifyWhenProcessed(si);
        RenderPackets(rawStylusInput.GetStylusPoints(), si);
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnStylusMove(RawStylusInput& rawStylusInput)
{
    // Only allow inking if someone has queried our RootVisual.
    if (_mainContainerVisual != nullptr)
    {
        StrokeInfo* si = FindStrokeInfo(rawStylusInput.Timestamp());

        if (si != nullptr && (si->StylusId() == rawStylusInput.StylusDeviceId()))
        {
            // We only render packets that are in the proper order due to
            // how our incremental rendering uses the last point to continue
            // the path geometry from.
            // NOTE: We also update the LastTime value here too
            if (si->IsTimestampAfter(rawStylusInput.Timestamp()))
            {
                si->SetLastTime(rawStylusInput.Timestamp());
                RenderPackets(rawStylusInput.GetStylusPoints(), si);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnStylusUp(RawStylusInput& rawStylusInput)
{
    // Only allow inking if someone has queried our RootVisual.
    if (_mainContainerVisual != nullptr)
    {
        StrokeInfo* si = FindStrokeInfo(rawStylusInput.Timestamp());

        if (si != nullptr &&
            ((si->StylusId() == rawStylusInput.StylusDeviceId()) ||
             (rawStylusInput.StylusDeviceId() == 0 &&
              (si->IsReset() ||
              (si->IsTimestampAfter(rawStylusInput.Timestamp()) && IsStylusUp(si->StylusId()))))))
        {
            si->SetSeenUp(true);
            si->SetLastTime(rawStylusInput.Timestamp());
            rawStylusInput.NotifyWhenProcessed(si);
        }
    }
}

bool DynamicRenderer::IsStylusUp(int stylusId)
{
    //TabletDeviceCollection tabletDevices = Tablet.TabletDevices;
    //for (int i=0; i<tabletDevices.Count; i++)
    //{
    //    TabletDevice tabletDevice = tabletDevices[i];
    //    for (int j=0; j<tabletDevice.StylusDevices.Count; j++)
    //    {
    //        StylusDevice stylusDevice = tabletDevice.StylusDevices[j];
    //        if (stylusId == stylusDevice.Id)
    //            return stylusDevice.InAir;
    //    }
    //}

    return true; // not found so must be up.
}

/// [TBS]
///
void DynamicRenderer::OnRenderComplete()
{
    StrokeInfo* si = _renderCompleteStrokeInfo;
    Debug::Assert(si!=nullptr);  // should never get here unless we are transitioning a stroke.

    if (si != nullptr)
    {
        // See if we are done transitioning this stroke!!
        if (si->StrokeHV()->Clip() == nullptr)
        {
            TransitionComplete(si);
            _renderCompleteStrokeInfo = nullptr;
        }
        else
        {
            // Wait for real time visual to be removed and updated.
            RemoveDynamicRendererVisualAndNotifyWhenDone(si);
        }
    }
}

void DynamicRenderer::RemoveDynamicRendererVisualAndNotifyWhenDone(StrokeInfo* si)
{
    if (si != nullptr)
    {
        QThread * renderingThread = _renderingThread; // Keep it alive
        if (renderingThread != nullptr)
        {
            // We are being called by the main UI thread, so marshal over to
            // the inking thread before cleaning up the stroke visual.
            Dispatcher::from(renderingThread)->BeginInvoke([this, si](void* unused)
            {
                if (si->StrokeRTICV ()!= nullptr)
                {
                    // Now wait till this is rendered and then notify UI thread.
                    //if (_onDRThreadRenderComplete == nullptr)
                    //{
                    //    _onDRThreadRenderComplete = new EventHandler(OnDRThreadRenderComplete);
                    //}

                    // Add to list to transact.
                    _renderCompleteDRThreadStrokeInfoList.enqueue(si);

                    // See if we are already waiting for a removed stroke to be rendered.
                    // If we aren't then remove visuals and wait for it to be rendered.
                    // Otherwise we'll do the work when the current stroke has been removed.
                    if (!_waitingForDRThreadRenderComplete)
                    {
                        ((ContainerVisual*)si->StrokeHV()->GetVisualTarget()->RootVisual())->Children().Remove(si->StrokeRTICV());
                        si->SetStrokeRTICV(nullptr);

                        // hook up render complete notification for one time then unhook.
                        //MediaContext.From(renderingThread->ThreadDispatcher).RenderComplete += _onDRThreadRenderComplete;
                        _waitingForDRThreadRenderComplete = true;
                    }
                }
                else
                {
                    // Nothing to transition so just say we're done!
                    NotifyAppOfDRThreadRenderComplete(si);
                }

                return nullptr;
            },
            nullptr);
        }
    }
}


void DynamicRenderer::NotifyAppOfDRThreadRenderComplete(StrokeInfo* si)
{
    Dispatcher* dispatcher = _applicationDispatcher;
    if (dispatcher != nullptr)
    {
        // We are being called by the inking thread, so marshal over to
        // the UI thread before handling the StrokeInfos that are done rendering.
        dispatcher->BeginInvoke([this, si](void* unused)
        {
            // See if this is the one we are doing a full transition for.
            if (si == _renderCompleteStrokeInfo)
            {
                if (si->StrokeHV()->Clip() != nullptr)
                {
                    si->StrokeHV()->SetClip(nullptr);
                    NotifyOnNextRenderComplete();
                }
                else
                {
                    Debug::Assert(_waitingForRenderComplete, "We were expecting to be waiting for a RenderComplete to call our OnRenderComplete, we might never reset and get flashing strokes from here on out");
                    TransitionComplete(si); // We're done
                }
            }
            else
            {
                TransitionComplete(si); // We're done
            }
            return nullptr;
        },
        nullptr);
    }
}


void DynamicRenderer::OnDRThreadRenderComplete(EventArgs& e)
{
    QThread * drThread = _renderingThread;
    Dispatcher* drDispatcher = nullptr;

    // Remove RenderComplete hook.
    if (drThread != nullptr)
    {
        drDispatcher = Dispatcher::from(drThread);

        if (drDispatcher != nullptr)
        {
            if (_renderCompleteDRThreadStrokeInfoList.size() > 0)
            {
                StrokeInfo* si = _renderCompleteDRThreadStrokeInfoList.dequeue();
                NotifyAppOfDRThreadRenderComplete(si);
            }

            // If no other queued up transitions, then remove event listener.
            if (_renderCompleteDRThreadStrokeInfoList.size() == 0)
            {
                // First unhook event handler
                //MediaContext::From(drDispatcher).RenderComplete -= _onDRThreadRenderComplete;
                _waitingForDRThreadRenderComplete = false;
            }
            else
            {
                // Process next waiting one.  Note we don't remove till removed processed.
                StrokeInfo* siNext = _renderCompleteDRThreadStrokeInfoList.front();
                if (siNext->StrokeRTICV() != nullptr)
                {
                    // Post this back to our thread to make sure we return from the
                    // this render complete call first before queuing up the next.
                    drDispatcher->BeginInvoke([siNext](void* unused) {
                            ((ContainerVisual*)siNext->StrokeHV()->GetVisualTarget()->RootVisual())->Children().Remove(siNext->StrokeRTICV());
                            siNext->SetStrokeRTICV(nullptr);
                            return nullptr;
                    },
                    nullptr);
                }
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnStylusDownProcessed(void* callbackData, bool targetVerified)
{
    StrokeInfo* si = reinterpret_cast<StrokeInfo*>(callbackData);

    if (si == nullptr)
        return;

    // See if we need to abort this stroke or reset the HostVisual clipping rect to nullptr.
    if (!targetVerified)
    {
        TransitionStrokeVisuals(si, true);
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnStylusUpProcessed(void* callbackData, bool targetVerified)
{
    StrokeInfo* si = reinterpret_cast<StrokeInfo*>(callbackData);

    if (si == nullptr)
        return;

    // clean up stroke visuals (and move to transitional VisualTarget as needed)
    TransitionStrokeVisuals(si, !targetVerified);
}

void DynamicRenderer::OnInternalRenderComplete(EventArgs& e)
{
    // First unhook event handler
    //MediaContext.From(_applicationDispatcher).RenderComplete -= _onRenderComplete;
    _waitingForRenderComplete = false;

    // Make sure lock() doesn't cause reentrancy.
    //using(_applicationDispatcher->DisableProcessing())
    {
        // Now notify event happened.
        OnRenderComplete();
    }
}


/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::NotifyOnNextRenderComplete()
{
    // Nothing to do if not hooked up to plugin collection.
    if (_applicationDispatcher == nullptr)
        return;

    // Ensure on application Dispatcher.
    _applicationDispatcher->VerifyAccess();

    //if (_onRenderComplete == nullptr)
    //{
    //    _onRenderComplete = new EventHandler(OnInternalRenderComplete);
    //}

    if (!_waitingForRenderComplete)
    {
        // hook up render complete notification for one time then unhook.
        //MediaContext.From(_applicationDispatcher).RenderComplete += _onRenderComplete;
        _waitingForRenderComplete = true;
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnDraw(  DrawingContext& drawingContext,
                                QSharedPointer<StylusPointCollection> stylusPoints,
                                Geometry* geometry,
                                QBrush fillBrush)
{
    //if (drawingContext == nullptr)
    //{
    //    throw std::exception("drawingContext");
    //}
    QPen pen;//(Qt::NoPen);
    //pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
    drawingContext.DrawGeometry(fillBrush, pen, geometry);
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS]
/// </summary>
void DynamicRenderer::OnDrawingAttributesReplaced()
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// Retrieves the Dispatcher for the thread used for rendering dynamic strokes
/// when receiving data from the stylus input thread(s).
/// </summary>
Dispatcher* DynamicRenderer::GetDispatcher()
{
    return _renderingThread != nullptr ? Dispatcher::from(_renderingThread) : nullptr;
}

/////////////////////////////////////////////////////////////////////

void DynamicRenderer::RenderPackets(QSharedPointer<StylusPointCollection> stylusPoints,  StrokeInfo* si)
{
    // If no points or not hooked up to element then do nothing.
    if (stylusPoints->size() == 0 || _applicationDispatcher == nullptr)
        return;

    // Get a collection of ink nodes built from the new stylusPoints.
    si->SetStrokeNodeIterator(si->GetStrokeNodeIterator().GetIteratorForNextSegment(stylusPoints));
    if (si->GetStrokeNodeIterator() != nullptr)
    {
        // Create a PathGeometry representing the contour of the ink increment
        Geometry* strokeGeometry = nullptr;
        QRectF bounds;
        StrokeRenderer::CalcGeometryAndBounds(si->GetStrokeNodeIterator(),
                                             *si->GetDrawingAttributes(),
#if DEBUG_RENDERING_FEEDBACK
                                             nullptr, //debug dc
                                             0d,   //debug feedback size
                                             false,//render debug feedback
#endif
                                             false, //calc bounds
                                             strokeGeometry,
                                             bounds);

        // If we are called from the app thread we can just stay on it and render to that
        // visual tree.  Otherwise we need to marshal over to our inking thread to do our work.
        if (_applicationDispatcher->CheckAccess())
        {
            // See if we need to create a new container visual for the stroke.
            if (si->StrokeCV() == nullptr)
            {
                // Create new container visual for this stroke and add our incremental rendering visual to it.
                si->SetStrokeCV(new ContainerVisual());

                //




                if (!si->GetDrawingAttributes()->IsHighlighter())
                {
                    si->StrokeCV()->SetOpacity(si->Opacity());
                }
                _mainRawInkContainerVisual->Children().Add(si->StrokeCV());
            }

            // Create new visual and render the geometry into it
            DrawingVisual* visual = new DrawingVisual();
            std::unique_ptr<DrawingContext> drawingContext(visual->RenderOpen());
            //try
            {
                FinallyHelper final([&drawingContext]() {
                    drawingContext->Close();
                });
                OnDraw(*drawingContext, stylusPoints, strokeGeometry, si->FillBrush());
            }
            //finally
            //{
            //    drawingContext.Close();
            //}

            // Now add it to the visual tree (making sure we still have StrokeCV after
            // onDraw called above).
            if (si->StrokeCV() != nullptr)
            {
                si->StrokeCV()->Children().Add(visual);
            }
        }
        else
        {
            /*
            QThread* renderingThread = _renderingThread; // keep it alive
            Dispatcher* drDispatcher = renderingThread != nullptr ? renderingThread->ThreadDispatcher() : nullptr;

            // Only try to render if we get a ref on the rendering thread.
            if (drDispatcher != nullptr)
            {
                // We are on a pen thread so marshal this call to our inking thread.
                drDispatcher.BeginInvoke(DispatcherPriority.Send,
                (DispatcherOperationCallback) delegate(object unused)
                {
                    QBrush fillBrush = si->FillBrush;

                    // Make sure this stroke is not aborted
                    if (fillBrush != nullptr)
                    {
                        // See if we need to create a new container visual for the stroke.
                        if (si->StrokeRTICV == nullptr)
                        {
                            // Create new container visual for this stroke and add our incremental rendering visual to it.
                            si->StrokeRTICV = new ContainerVisual();

                            //




                            if (!si->DrawingAttributes.IsHighlighter)
                            {
                                si->StrokeRTICV.Opacity = si->Opacity;
                            }
                            ((ContainerVisual)si->StrokeHV.VisualTarget.RootVisual).Children.Add(si->StrokeRTICV);
                        }

                        // Create new visual and render the geometry into it
                        DrawingVisual visual = new DrawingVisual();
                        DrawingContext drawingContext = visual.RenderOpen();
                        try
                        {
                            OnDraw(drawingContext, stylusPoints, strokeGeometry, fillBrush);
                        }
                        finally
                        {
                            drawingContext.Close();
                        }
                        // Add it to the visual tree
                        si->StrokeRTICV.Children.Add(visual);
                    }

                    return nullptr;
                },
                nullptr);
            }*/
        }
    }
}

/////////////////////////////////////////////////////////////////////

void DynamicRenderer::AbortAllStrokes()
{
    {
        QMutexLocker l(&__siLock);
        while (_strokeInfoList.size() > 0)
        {
            TransitionStrokeVisuals(_strokeInfoList[0], true);
        }
    }
}


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
// 5) [UI Thread] Reset HostVisual.Clip = nullptr and wait for render complete of that
// 6) [UI Thread] On rendercomplete - we done.  Mark this HostVisual as free.
//
// In the case of another stroke coming through before a previous transition has completed
// then basically instead of starting with step 1 we jump to step 2 and when then on step 5
// we mark the HostVisual free and we are done.
//
void DynamicRenderer::TransitionStrokeVisuals(StrokeInfo* si, bool abortStroke)
{
    // Make sure we don't get any more input for this stroke.
    RemoveStrokeInfo(si);

    // remove si visuals and this si
    if (si->StrokeCV() != nullptr)
    {
        if (_mainRawInkContainerVisual != nullptr)
        {
            _mainRawInkContainerVisual->Children().Remove(si->StrokeCV());
        }
        si->SetStrokeCV(nullptr);
    }

    si->SetFillBrush(QBrush());

    // Nothing to do if we've destroyed our host visuals.
    if (_rawInkHostVisual1 == nullptr)
        return;

    bool doRenderComplete = false;

    // See if we can do full transition (only when none in progress and not abort)
    if (!abortStroke && _renderCompleteStrokeInfo == nullptr)
    {
        // make sure lock does not cause reentrancy on application thread!
        //using (_applicationDispatcher->DisableProcessing())
        {
            {
                QMutexLocker l(&__siLock);
                // We can transition the host visual only if a single reference is on it.
                if (si->StrokeHV()->HasSingleReference())
                {
                    Debug::Assert(si->StrokeHV()->Clip() == nullptr);
                    si->StrokeHV()->SetClip(_zeroSizedFrozenRect);
                    Debug::Assert(_renderCompleteStrokeInfo == nullptr);
                    _renderCompleteStrokeInfo = si;
                    doRenderComplete = true;
                }
            }
        }
    }

    if (doRenderComplete)
    {
        NotifyOnNextRenderComplete();
    }
    else
    {
        // Just wait to dynamic rendering thread is updated then we're done.
        RemoveDynamicRendererVisualAndNotifyWhenDone(si);
    }
}

// Figures out the correct DynamicRenderHostVisual to use.
DynamicRenderer::DynamicRendererHostVisual* DynamicRenderer::GetCurrentHostVisual()
{
    // Find which of the two host visuals to use as current.
    if (_currentHostVisual == nullptr)
    {
        _currentHostVisual = _rawInkHostVisual1;
    }
    else
    {
        HostVisual* transitioningHostVisual = _renderCompleteStrokeInfo != nullptr ?
                                                _renderCompleteStrokeInfo->StrokeHV() : nullptr;

        if (_currentHostVisual->InUse())
        {
            if (_currentHostVisual == _rawInkHostVisual1)
            {
                if (!_rawInkHostVisual2->InUse() || _rawInkHostVisual1 == transitioningHostVisual)
                {
                    _currentHostVisual = _rawInkHostVisual2;
                }
            }
            else
            {
                if (!_rawInkHostVisual1->InUse() || _rawInkHostVisual2 == transitioningHostVisual)
                {
                    _currentHostVisual = _rawInkHostVisual1;
                }
            }
        }
    }
    return _currentHostVisual;
}


// Removes ref from DynamicRendererHostVisual.
void DynamicRenderer::TransitionComplete(StrokeInfo* si)
{
    // make sure lock does not cause reentrancy on application thread!
    //using(_applicationDispatcher->DisableProcessing())
    {
        {
            QMutexLocker l(&__siLock);
            si->StrokeHV()->RemoveStrokeInfoRef(si);
        }
    }
}

void DynamicRenderer::RemoveStrokeInfo(StrokeInfo* si)
{
    {
        QMutexLocker l(&__siLock);
        _strokeInfoList.removeOne(si);
    }
}

DynamicRenderer::StrokeInfo* DynamicRenderer::FindStrokeInfo(int timestamp)
{
    {
        QMutexLocker l(&__siLock);
        for (int i=0; i < _strokeInfoList.size(); i++)
        {
            StrokeInfo* siCur = _strokeInfoList[i];

            if (siCur->IsTimestampWithin(timestamp))
            {
                return siCur;
            }
        }
    }

    return nullptr;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - On UIContext
/// </summary>
QSharedPointer<DrawingAttributes> DynamicRenderer::GetDrawingAttributes()
{
     return _drawAttrsSource;
}
void DynamicRenderer::SetDrawingAttributes(QSharedPointer<DrawingAttributes> value)
{
     if (value == nullptr)
         throw std::exception("value");
     _drawAttrsSource = value;
     OnDrawingAttributesReplaced();
}

void DynamicRenderer::CreateInkingVisuals()
{
    if (_mainContainerVisual == nullptr)
    {
        _mainContainerVisual = new ContainerVisual();
        _mainContainerVisual->setObjectName("DynamicRenderer::MainContainerVisual");
        _mainRawInkContainerVisual = new ContainerVisual();
        _mainRawInkContainerVisual->setObjectName("DynamicRenderer::MainRawInkContainerVisual");
        _mainContainerVisual->Children().Add(_mainRawInkContainerVisual);
    }

    if (IsActiveForInput())
    {
        // Make sure lock() doesn't cause reentrancy.
        //using (Element.Dispatcher.DisableProcessing())
        {
            CreateRealTimeVisuals();
        }
    }
}

/// <summary>
/// Create the visual target
/// This method is called from the application context
/// </summary>
void DynamicRenderer::CreateRealTimeVisuals()
{
    // Only create if we have a root visual and have not already created them.
    if (_mainContainerVisual != nullptr && _rawInkHostVisual1 == nullptr)
    {
        // Create new VisualTarget and hook up in apps visuals under element.
        _rawInkHostVisual1 = new DynamicRendererHostVisual();
        _rawInkHostVisual2 = new DynamicRendererHostVisual();
        _currentHostVisual = nullptr;  // Pick a new current HostVisual on first stylus input.
        _mainContainerVisual->Children().Add(_rawInkHostVisual1);
        _mainContainerVisual->Children().Add(_rawInkHostVisual2);
        // NOTE: Do the work later if perf is bad hooking up VisualTargets on StylusDown...

        // Guarentee that objects are valid when on the DR thread below.
        //DynamicRendererHostVisual[] myArgs = new DynamicRendererHostVisual[2] { _rawInkHostVisual1, _rawInkHostVisual2 };

        // Do this last since we can be reentrant on this call and we want to set
        // things up so we are all set except for the real time thread visuals which
        // we set up on first usage.
        _renderingThread = QThread::currentThread();

        /*
        // We are being called by the main UI thread, so invoke a call over to
        // the inking thread to create the visual targets.
        // NOTE: Since input rendering uses the same priority we are guanenteed
        //       that this will be processed before any input will try to be rendererd.
        _renderingThread.ThreadDispatcher.BeginInvoke(DispatcherPriority.Send,
        (DispatcherOperationCallback)delegate(object args)
        {
            DynamicRendererHostVisual[] hostVisuals = (DynamicRendererHostVisual[])args;
            VisualTarget vt;
            // Query the VisualTarget properties to initialize them.
            vt = hostVisuals[0].VisualTarget;
            vt = hostVisuals[1].VisualTarget;

            return nullptr;
        },
        myArgs);
        */
    }
}

/// <summary>
/// Unhoot the visual target.
/// This method is called from the application Dispatcher
/// </summary>
void DynamicRenderer::DestroyRealTimeVisuals()
{
    // Only need to handle if already created visuals.
    if (_mainContainerVisual != nullptr && _rawInkHostVisual1 != nullptr)
    {
        // Make sure we unhook the rendercomplete event.
        if (_waitingForRenderComplete)
        {
            //MediaContext.From(_applicationDispatcher).RenderComplete -= _onRenderComplete;
            _waitingForRenderComplete = false;
        }

        _mainContainerVisual->Children().Remove(_rawInkHostVisual1);
        _mainContainerVisual->Children().Remove(_rawInkHostVisual2);

        _renderCompleteStrokeInfo = nullptr;

        QThread* renderingThread = _renderingThread; // keep ref to keep it alive in this routine
        Dispatcher* drDispatcher = renderingThread != nullptr ? Dispatcher::from(renderingThread) : nullptr;

        if (drDispatcher != nullptr)
        {
            drDispatcher->BeginInvoke([this, renderingThread](void* unused)
            {
                _renderCompleteDRThreadStrokeInfoList.clear();

                Dispatcher* drDispatcher = Dispatcher::from(renderingThread);

                if (drDispatcher != nullptr && _waitingForDRThreadRenderComplete)
                {
                    //MediaContext.From(drDispatcher).RenderComplete -= _onDRThreadRenderComplete;
                }
                _waitingForDRThreadRenderComplete = false;

                return nullptr;
            },
            nullptr);
        }

        // Make sure to free up inking thread ref to ensure thread shuts down properly.
        _renderingThread = nullptr;

        _rawInkHostVisual1 = nullptr;
        _rawInkHostVisual2 = nullptr;
        _currentHostVisual = nullptr;  // We create new HostVisuals next time we're enabled.

        AbortAllStrokes(); // Doing this here avoids doing a begininvoke to enter the rendering thread (avoid reentrancy).
    }
}
