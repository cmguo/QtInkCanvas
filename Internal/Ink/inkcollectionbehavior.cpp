#include "Internal/Ink/inkcollectionbehavior.h"
#include "Windows/Controls/inkcanvas.h"
#include "Windows/routedeventargs.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/StylusPlugIns/dynamicrenderer.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Controls/inkevents.h"
#include "finallyhelper.h"

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors

/// <SecurityNote>
///     Critical: Touches critical member _stylusPoints, which is passed to
///         SecurityCritical method GetInkCanvas().RaiseGestureOrStrokeCollected
///
///     TreatAsSafe: only initializes _stylusPoints to nullptr , _userInitialize to false
///
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
InkCollectionBehavior::InkCollectionBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : StylusEditingBehavior(editingCoordinator, inkCanvas)
{
    //_stylusPoints = nullptr ;
    _userInitiated = false;
}

//#endregion Constructors

/// <summary>
/// A method which flags InkCollectionBehavior when it needs to reset the dynamic renderer.
///    Called By:
///         GetEditingCoordinator().OnInkCanvasDeviceDown
/// </summary>
void InkCollectionBehavior::ResetDynamicRenderer()
{
    _resetDynamicRenderer = true;
}

//-------------------------------------------------------------------------------
//
// Protected Methods
//
//-------------------------------------------------------------------------------

//#region Protected Methods

/// <summary>
/// Overrides SwitchToMode
/// As the following expected results
///  1. From Ink To InkAndGesture
///     Packets between StylusDown and StylusUp are sent to the gesture reco. On StylusUp gesture event fires. If it’s not a gesture, StrokeCollected event fires.
///  2. From Ink To GestureOnly
///     Packets between StylusDown and StylusUp are send to the gesture reco. On StylusUp gesture event fires. Stroke gets removed on StylusUp even if it’s not a gesture.
///  3. From Ink To EraseByPoint
///     Stroke is discarded. PointErasing is performed after changing the mode.
///  4. From Ink To EraseByStroke
///      Stroke is discarded. StrokeErasing is performed after changing the mode.
///  5. From Ink To Select
///     Stroke is discarded. Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
///  6. From Ink To None
///     Stroke is discarded.
///  7. From InkAndGesture To Ink
///     Stroke is collected for all packets between StylusDown and StylusUp. Gesture event does not fire.
///  8. From InkAndGesture To GestureOnly
///     Packets between StylusDown and StylusUp are sent to the gesture reco. Stroke gets removed on StylusUp even if it’s not a gesture.
///  9. From InkAndGesture To EraseByPoint
///     Stroke is discarded. PointErasing is performed after changing the mode, gesture event does not fire.
/// 10. From InkAndGesture To EraseByStroke
///     Stroke is discarded. StrokeErasing is performed after changing the mode, gesture event does not fire.
/// 11. From InkAndGesture To Select
///     Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
/// 12. From InkAndGesture To None
///     Stroke is discarded, no gesture is recognized.
/// 13. From GestureOnly To InkAndGesture
///     Packets between StylusDown and StylusUp are sent to the gesture reco. On StylusUp gesture event fires. If it’s not a gesture, StrokeCollected event fires.
/// 14. From GestureOnly To Ink
///     Stroke is collected. Gesture event does not fire.
/// 15. From GestureOnly To EraseByPoint
///     Stroke is discarded PointErasing is performed after changing the mode, gesture event does not fire
/// 16. From GestureOnly To EraseByStroke
///     Stroke is discarded. StrokeErasing is performed after changing the mode, gesture event does not fire.
/// 17. From GestureOnly To Select
///     Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
/// 18. From GestureOnly To None
///     Stroke is discarded. Gesture event does not fire.
/// </summary>
/// <param name="mode"></param>
/// <SecurityNote>
///     Critical: Clones SecurityCritical member _stylusPoints, which is only critical
///         as an argument in StylusInputEnd to critical method GetInkCanvas().RaiseGestureOrStrokeCollected
///
///     TreatAsSafe: This method simply clones critical member _stylusPoints and passes that to transparent code
///         _stylusPoints is only critical when passed to GetInkCanvas().RaiseGestureOrStrokeCollected.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
void InkCollectionBehavior::OnSwitchToMode(InkCanvasEditingMode mode)
{
    //Debug.Assert(GetEditingCoordinator().IsInMidStroke, "SwitchToMode should only be called in a mid-stroke");

    switch ( mode )
    {
        case InkCanvasEditingMode::Ink:
        case InkCanvasEditingMode::InkAndGesture:
        case InkCanvasEditingMode::GestureOnly:
            {
                // We are under one of those Ink modes now. Nothing to change here except raising the mode change event.
                RoutedEventArgs e(InkCanvas::ActiveEditingModeChangedEvent, &GetInkCanvas());
                GetInkCanvas().RaiseActiveEditingModeChanged(e);
                break;
            }
        case InkCanvasEditingMode::EraseByPoint:
        case InkCanvasEditingMode::EraseByStroke:
            {
                // Discard the collected ink.
                Commit(false);

                // Change the Erase mode
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);
                break;
            }
        case InkCanvasEditingMode::Select:
            {
                // Make a copy of the current cached points.
                QSharedPointer<StylusPointCollection> cachedPoints = _stylusPoints != nullptr ?
                                                        _stylusPoints->Clone() : nullptr ;

                // Discard the collected ink.
                Commit(false);

                // Change the Select mode
                IStylusEditing* newBehavior = GetEditingCoordinator().ChangeStylusEditingMode(this, mode);

                if ( cachedPoints != nullptr
                    // NOTICE-2006/04/27-WAYNEZEN,
                    // GetEditingCoordinator().ChangeStylusEditingMode raises external event.
                    // The user code could take any arbitrary action for instance calling GetInkCanvas().ReleaseMouseCapture()
                    // So there is no guarantee that we could receive the newBehavior.
                    && newBehavior != nullptr )
                {
                    // Now add the previous points to the lasso behavior
                    // The SelectionBehavior doesn't check userInitiated, pass false
                    // even if our _userInitiated flag is true
                    newBehavior->AddStylusPoints(cachedPoints, false/*userInitiated*/);
                }

                break;
            }
        case InkCanvasEditingMode::None:
            {
                // Discard the collected ink.
                Commit(false);

                // Change to the None mode
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);
                break;
            }
        default:
            //Debug.Assert(false, "Unknown InkCanvasEditingMode!");
            break;
    }
}

/// <summary>
/// OnActivate
/// </summary>
void InkCollectionBehavior::OnActivate()
{
    StylusEditingBehavior::OnActivate();

    // Enable RealTimeInking.
    if (GetInkCanvas().InternalDynamicRenderer() != nullptr )
    {
        GetInkCanvas().InternalDynamicRenderer()->SetEnabled(true);
        GetInkCanvas().UpdateDynamicRenderer(); // Kick DynamicRenderer to be hooked up to renderer.
    }

    //if we're activated in PreviewStylusDown or in mid-stroke, the DynamicRenderer will miss the down
    //and will not ink.  If that is the case, flag InkCollectionBehavior to reset RTI.
    _resetDynamicRenderer = GetEditingCoordinator().StylusOrMouseIsDown();
}

/// <summary>
/// OnDeactivate
/// </summary>
void InkCollectionBehavior::OnDeactivate()
{
    StylusEditingBehavior::OnDeactivate();

    // Disable RealTimeInking.
    if (GetInkCanvas().InternalDynamicRenderer() != nullptr )
    {
        GetInkCanvas().InternalDynamicRenderer()->SetEnabled(false);
        GetInkCanvas().UpdateDynamicRenderer();  // Kick DynamicRenderer to be removed from renderer.
    }
}

/// <summary>
/// Get Pen Cursor
/// </summary>
/// <returns></returns>
QCursor InkCollectionBehavior::GetCurrentCursor()
{
    if ( GetEditingCoordinator().UserIsEditing() == true )
    {
        return QCursor();
    }
    else
    {
        return PenCursor();
    }
}


/// <summary>
/// StylusInputBegin
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
/// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
/// <SecurityNote>
///     Critical: Constructs SecurityCritical member _stylusPoints, which is passed
///         as an argument in StylusInputEnd to critical method GetInkCanvas().RaiseGestureOrStrokeCollected
///
///         Also accesses critical member _userInitiated, which is used to determine if all of the
///         stylusPoints were user initiated
/// </SecurityNote>
//[SecurityCritical]
void InkCollectionBehavior::StylusInputBegin(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    _userInitiated = false;

    //we only initialize to true if the first stylusPoints were user initiated
    if (userInitiated)
    {
        _userInitiated = true;
    }

    _stylusPoints.reset(new StylusPointCollection(stylusPoints->Description(), 100));
    _stylusPoints->Add(*stylusPoints);

    _strokeDrawingAttributes = GetInkCanvas().DefaultDrawingAttributes()->Clone();

    // Reset the dynamic renderer if it's been flagged.
    if ( _resetDynamicRenderer )
    {
        InputDevice* inputDevice = GetEditingCoordinator().GetInputDeviceForReset();
        if ( GetInkCanvas().InternalDynamicRenderer() != nullptr && inputDevice != nullptr )
        {
            StylusDevice* stylusDevice = static_cast<StylusDevice*>(inputDevice);
            // If the input device is MouseDevice, nullptr will be passed in Reset Method.
            GetInkCanvas().InternalDynamicRenderer()->Reset(stylusDevice, stylusPoints);
        }

        _resetDynamicRenderer = false;
    }

    // Call InvalidateBehaviorCursor at the end of the routine. The method will cause an external event fired.
    // So it should be invoked after we set up our states.
    GetEditingCoordinator().InvalidateBehaviorCursor(this);
}

/// <summary>
/// StylusInputContinue
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
/// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
/// <SecurityNote>
///     Critical: Adds to SecurityCritical member _stylusPoints, which are passed
///         as an argument in StylusInputEnd to critical method GetInkCanvas().RaiseGestureOrStrokeCollected
///
///         Also accesses critical member _userInitiated, which is used to determine if all of the
///         stylusPoints were user initiated
/// </SecurityNote>
//[SecurityCritical]
void InkCollectionBehavior::StylusInputContinue(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    //we never set _userInitated to true after it is initialized, only to false
    if (!userInitiated)
    {
        _userInitiated = false;
    }

    _stylusPoints->Add(*stylusPoints);
}

/// <summary>
/// StylusInputEnd
/// </summary>
/// <param name="commit">commit</param>
/// <SecurityNote>
///     Critical: Calls SecurityCritical method GetInkCanvas().RaiseGestureOrStrokeCollected with
///         critical data _stylusPoints
///
///         Also accesses critical member _userInitiated, which is used to determine if all of the
///         stylusPoints were user initiated
///
///     TreatAsSafe: The critical methods that build up the critical argument _stylusPoints which
///         is passed to critical method GetInkCanvas().RaiseGestureOrStrokeCollected are already marked
///         critical.  No critical data is passed in this method.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
void InkCollectionBehavior::StylusInputEnd(bool commit)
{
    // The follow code raises Gesture and/or StrokeCollected event
    // The out-side code could throw exception in the their handlers. We use try/finally block to protect our status.

    //try
    {
        FinallyHelper final([this](){
            _stylusPoints = nullptr ;
            _strokeDrawingAttributes = nullptr ;
            _userInitiated = false;
            GetEditingCoordinator().InvalidateBehaviorCursor(this);
        });

        if ( commit )
        {
            // NTRAID:WINDOWS#1613731-2006/04/27-WAYNEZEN,
            // It's possible that the input may end up without any StylusPoint being collected since the behavior can be deactivated by
            // the user code in the any event handler.
            if ( _stylusPoints != nullptr )
            {
                //Debug.Assert(_strokeDrawingAttributes != nullptr , "_strokeDrawingAttributes can not be nullptr , did we not see a down?");

                QSharedPointer<Stroke>  stroke(new Stroke(_stylusPoints, _strokeDrawingAttributes));

                //we don't add the stroke to the InkCanvas stroke collection until RaiseStrokeCollected
                //since this might be a gesture and in some modes, gestures don't get added
                InkCanvasStrokeCollectedEventArgs argsStroke(stroke);
                GetInkCanvas().RaiseGestureOrStrokeCollected(argsStroke, _userInitiated);
            }
        }
    }
    //finally
    //{
    //    _stylusPoints = nullptr ;
    //    _strokeDrawingAttributes = nullptr ;
    //    _userInitiated = false;
    //    GetEditingCoordinator().InvalidateBehaviorCursor(this);
    //}
}

/// <summary>
/// ApplyTransformToCursor
/// </summary>
void InkCollectionBehavior::OnTransformChanged()
{
    // Drop the cached pen cursor.
    //_cachedPenCursor = nullptr ;
}

//#endregion Protected Methods

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

QCursor InkCollectionBehavior::PenCursor()
{
    // We only update our cache cursor when DefaultDrawingAttributes has changed or
    // there are animated transforms being applied to GetInkCanvas().
    if ( /*_cachedPenCursor == nullptr ||*/ _cursorDrawingAttributes != GetInkCanvas().DefaultDrawingAttributes() )
    {
        //adjust the DA for any Layout/Render transforms.
        QMatrix xf = GetElementTransformMatrix();

        QSharedPointer<DrawingAttributes> da = GetInkCanvas().DefaultDrawingAttributes();
        if ( !xf.isIdentity() )
        {
            //scale the DA, zero the offsets.
            xf *= da->StylusTipTransform();
            //xf.OffsetX = 0;
            //xf.OffsetY = 0;
            xf = QMatrix(xf.m11(), xf.m12(), xf.m21(), xf.m22(), 0, 0);
            if ( xf.isInvertible() )
            {
                da = da->Clone();
                da->SetStylusTipTransform(xf);
            }
        }

        _cursorDrawingAttributes = GetInkCanvas().DefaultDrawingAttributes()->Clone();
        //DpiScale dpi = GetInkCanvas().GetDpi();
        //_cachedPenCursor = PenCursorManager.GetPenCursor(da, false, (GetInkCanvas().FlowDirection == FlowDirection.RightToLeft), dpi.DpiScaleX, dpi.DpiScaleY);
    }

    return _cachedPenCursor;
}

//#endregion Methods
