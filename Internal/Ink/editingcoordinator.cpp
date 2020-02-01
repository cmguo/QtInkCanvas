#include "Internal/Ink/editingcoordinator.h"
#include "Internal/Ink/selectioneditor.h"
#include "Windows/Controls/inkcanvas.h"
#include "Windows/Input/styluspointdescription.h"
#include "Internal/Ink/inkcollectionbehavior.h"
#include "Internal/Ink/lassoselectionbehavior.h"
#include "Internal/Ink/selectioneditingbehavior.h"
#include "Internal/Ink/eraserbehavior.h"
#include "Windows/Controls/editingmode.h"
#include "Windows/routedeventargs.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Input/StylusPlugIns/dynamicrenderer.h"
#include "Windows/Input/stylusdevice.h"
#include "Windows/Input/mousebuttoneventargs.h"
#include "Windows/Input/styluseventargs.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

/// <summary>
/// Constructors
/// </summary>
/// <param name="inkCanvas">InkCanvas instance</param>
EditingCoordinator::EditingCoordinator(InkCanvas& inkCanvas)
    : _inkCanvas(inkCanvas)
{
    //if (inkCanvas == nullptr)
    //{
    //    throw std::exception("inkCanvas");
    //}

    //_inkCanvas = inkCanvas;

    // Create a stack for tracking the behavior
    //_activationStack = new QStack<EditingBehavior>(2);
    _activationStack.reserve(2);

    // NTRAID#WINDOWS-1104477-2005/02/26-waynezen,
    // listen to in-air move so that we could get notified when Stylus is inverted.
    //
    //_inkCanvas.AddHandler(Stylus.StylusInRangeEvent, new StylusEventHandler(OnInkCanvasStylusInAirOrInRangeMove));
    //_inkCanvas.AddHandler(Stylus.StylusInAirMoveEvent, new StylusEventHandler(OnInkCanvasStylusInAirOrInRangeMove));
    //_inkCanvas.AddHandler(Stylus.StylusOutOfRangeEvent, new StylusEventHandler(OnInkCanvasStylusOutOfRange));
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// Activate a dynamic behavior
/// Called from:
///     SelectionEditor.OnCanvasMouseMove
/// </summary>
/// <param name="dynamicBehavior"></param>
/// <param name="inputDevice"></param>
/// <SecurityNote>
///     Critical: Calls critical methods AddStylusPoints and InitializeCapture
/// </SecurityNote>
//[SecurityCritical]
void EditingCoordinator::ActivateDynamicBehavior(EditingBehavior* dynamicBehavior, InputDevice* inputDevice)
{
    // Only SelectionEditor should be enable to initiate dynamic behavior
    Debug::Assert(ActiveEditingBehavior() == GetSelectionEditor(),
                    "Only SelectionEditor should be able to initiate dynamic behavior");

    DebugCheckDynamicBehavior(dynamicBehavior);

    // Push the dynamic behavior.
    PushEditingBehavior(dynamicBehavior);

    // If this is LassoSelectionBehavior, we should capture the current stylus and feed the cached
    if ( dynamicBehavior == GetLassoSelectionBehavior() )
    {
        bool fSucceeded = false;

        // The below code might call out StrokeErasing or StrokeErased event.
        // The out-side code could throw exception.
        // We use try/finally block to protect our status.
        //try
        {
            FinallyHelper final([this, &fSucceeded] () {
                if ( !fSucceeded )
                {
                    // Abort the current editing.
                    ActiveEditingBehavior()->Commit(false);

                    // Release capture and do clean-up.
                    ReleaseCapture(true);
                }
            });
            //we pass true for userInitiated because we've simply consulted the InputDevice
            //(and only StylusDevice or MouseDevice) for the current position of the device
            InitializeCapture(inputDevice, static_cast<StylusEditingBehavior*>(dynamicBehavior),
                true /*userInitiated*/, false/*Don't reset the RTI*/);
            fSucceeded = true;
        }
        //finally
        //{
        //    if ( !fSucceeded )
        //    {
                // Abort the current editing.
        //        ActiveEditingBehavior()->Commit(false);

                // Release capture and do clean-up.
        //        ReleaseCapture(true);
        //    }
        //}
    }

    RoutedEventArgs e(InkCanvas::ActiveEditingModeChangedEvent, &_inkCanvas);
    _inkCanvas.RaiseActiveEditingModeChanged(e);
}

/// <summary>
/// Deactivate a dynamic behavior
/// Called from:
///     EditingBehavior.Commit
/// </summary>
void EditingCoordinator::DeactivateDynamicBehavior()
{
    // Make sure we are under correct state:
    // ActiveEditingBehavior() has to be either GetLassoSelectionBehavior() or SelectionEditingBehavior.
    DebugCheckDynamicBehavior(ActiveEditingBehavior());

    // Pop the dynamic behavior.
    PopEditingBehavior();
}

/// <summary>
/// Update the current active EditingMode
/// Called from:
///     EditingCoordinator.UpdateInvertedState
///     InkCanvas.Initialize()
/// </summary>
void EditingCoordinator::UpdateActiveEditingState()
{
    UpdateEditingState(_stylusIsInverted);
}

/// <summary>
/// Update the EditingMode
/// Called from:
///     InkCanvas.RaiseEditingModeChanged
///     InkCanvas.RaiseEditingModeInvertedChanged
/// </summary>
/// <param name="inverted">A flage which indicates whether the new mode is for the Inverted state</param>
void EditingCoordinator::UpdateEditingState(bool inverted)
{
    // If the mode is inactive currently, we should skip the update.
    if ( inverted != _stylusIsInverted )
    {
        return;
    }

    // Get the current behavior and the new behavior.
    EditingBehavior* currentBehavior = ActiveEditingBehavior();
    EditingBehavior* newBehavior = GetBehavior(ActiveEditingMode());

    // Check whether the user is editing.
    if ( UserIsEditing() )
    {
        // Check if we are in the mid-stroke.
        if ( IsInMidStroke() )
        {
            // We are in mid-stroke now.

            Debug::Assert(currentBehavior->metaObject()->inherits(&StylusEditingBehavior::staticMetaObject),
                            "The current behavoir has to be one of StylusEditingBehaviors");
            static_cast<StylusEditingBehavior*>( currentBehavior )->SwitchToMode(ActiveEditingMode());
        }
        else
        {
            if ( currentBehavior == GetSelectionEditingBehavior() )
            {
                // Commit the current moving/resizing behavior
                currentBehavior->Commit(true);
            }

            ChangeEditingBehavior(newBehavior);
        }
    }
    else
    {
        // Check if we are in the mid-stroke.
        if ( IsInMidStroke() )
        {
            // We are in mid-stroke now.

            Debug::Assert(currentBehavior->metaObject()->inherits(&StylusEditingBehavior::staticMetaObject),
                            "The current behavoir has to be one of StylusEditingBehaviors");
            static_cast<StylusEditingBehavior*>( currentBehavior )->SwitchToMode(ActiveEditingMode());
        }
        else
        {
            // Make sure we are under correct state:
            // currentBehavior cannot be any of Dynamic Behavior.
            DebugCheckNonDynamicBehavior(currentBehavior);
            ChangeEditingBehavior(newBehavior);
        }
    }

    _inkCanvas.UpdateCursor();
}

/// <summary>
/// Update the PointEraerCursor
/// Called from:
///     InkCanvas.set_EraserShape
/// </summary>
void EditingCoordinator::UpdatePointEraserCursor()
{
    // We only have to update the point eraser when the active mode is EraseByPoint
    // In other case, EraseBehavior will update cursor properly in its OnActivate routine.
    if ( ActiveEditingMode() == InkCanvasEditingMode::EraseByPoint )
    {
        InvalidateBehaviorCursor(GetEraserBehavior());
    }
}

/// <summary>
/// InvalidateTransform
///     Called by: InkCanvas.OnPropertyChanged
/// </summary>
void EditingCoordinator::InvalidateTransform()
{
    // We only have to invalidate transform flags for GetInkCollectionBehavior() and EraserBehavior for now.
    SetTransformValid(GetInkCollectionBehavior(), false);
    SetTransformValid(GetEraserBehavior(), false);
}

/// <summary>
/// Invalidate the behavior cursor
/// Call from:
///     EditingCoordinator.UpdatePointEraserCursor
///     EraserBehavior.OnActivate
///     InkCollectionBehavior.OnInkCanvasDefaultDrawingAttributesReplaced
///     InkCollectionBehavior.OnInkCanvasDefaultDrawingAttributesChanged
///     SelectionEditingBehavior.OnActivate
///     SelectionEditor.UpdateCursor(InkCanvasSelectionHandle handle)
/// </summary>
/// <param name="behavior">the behavior which its cursor needs to be updated.</param>
void EditingCoordinator::InvalidateBehaviorCursor(EditingBehavior* behavior)
{
    // Should never be null
    Debug::Assert(behavior != nullptr);

    // InvalidateCursor first
    SetCursorValid(behavior, false);

    if ( !GetTransformValid(behavior) )
    {
        behavior->UpdateTransform();
        SetTransformValid(behavior, true);
    }

    // If the behavior is active, we have to update cursor right now.
    if ( behavior == ActiveEditingBehavior() )
    {
        _inkCanvas.UpdateCursor();
    }
}

/// <summary>
/// Check whether the cursor of the specified behavior is valid
/// </summary>
/// <param name="behavior">EditingBehavior</param>
/// <returns>True if the cursor doesn't need to be updated</returns>
bool EditingCoordinator::IsCursorValid(EditingBehavior* behavior)
{
    return GetCursorValid(behavior);
}

/// <summary>
/// Check whether the cursor of the specified behavior is valid
/// </summary>
/// <param name="behavior">EditingBehavior</param>
/// <returns>True if the cursor doesn't need to be updated</returns>
bool EditingCoordinator::EditingCoordinator::IsTransformValid(EditingBehavior* behavior)
{
    return GetTransformValid(behavior);
}

/// <summary>
/// Change to another StylusEditing mode or None mode.
/// </summary>
/// <param name="sourceBehavior"></param>
/// <param name="newMode"></param>
/// <returns></returns>
IStylusEditing* EditingCoordinator::ChangeStylusEditingMode(StylusEditingBehavior* sourceBehavior, InkCanvasEditingMode newMode)
{
    // NOTICE-2006/04/27-WAYNEZEN,
    // Before the caller invokes the method, the external event could have been fired.
    // The user code may interrupt the Mid-Stroke by releasing capture or switching to another mode.
    // So we should check if we still is under mid-stroke and the source behavior still is active.
    // If not, we just no-op.
    if ( IsInMidStroke() &&
        ( ( // We expect that either GetInkCollectionBehavior() or EraseBehavior is active.
            sourceBehavior != GetLassoSelectionBehavior() && sourceBehavior == ActiveEditingBehavior() )
            // Or We expect SelectionEditor is active here since
            // GetLassoSelectionBehavior() will be deactivated once it gets committed.
            || ( sourceBehavior == GetLassoSelectionBehavior() && GetSelectionEditor() == ActiveEditingBehavior() ) ) )
    {
        Debug::Assert(_activationStack.size() == 1, "The behavior stack has to contain one behavior.");

        // Pop up the old behavior
        PopEditingBehavior();

        // Get the new behavior
        EditingBehavior* newBehavior = GetBehavior(ActiveEditingMode());

        if ( newBehavior != nullptr )
        {
            // Push the new behavior
            PushEditingBehavior(newBehavior);

            // If the new mode is Select, we should push the GetLassoSelectionBehavior() now.
            if ( newMode == InkCanvasEditingMode::Select
                // NOTICE-2006/04/27-WAYNEZEN,
                // Make sure the newBehavior is SelectionEditor since it could be changed by the user event handling code.
                && newBehavior == GetSelectionEditor())
            {
                PushEditingBehavior(GetLassoSelectionBehavior());
            }
        }
        else
        {
            // None-mode now. We stop collecting the packets.
            ReleaseCapture(true);
        }

        RoutedEventArgs e(InkCanvas::ActiveEditingModeChangedEvent, &_inkCanvas);
        _inkCanvas.RaiseActiveEditingModeChanged(e);

        return qobject_cast<StylusEditingBehavior*>(ActiveEditingBehavior());
    }
    else
    {
        // No-op
        return nullptr;
    }
}

/// <summary>
/// Debug Checker
/// </summary>
/// <param name="behavior"></param>
//[Conditional("DEBUG")]
void EditingCoordinator::DebugCheckActiveBehavior(EditingBehavior* behavior)
{
    (void) behavior;
    Debug::Assert(behavior == ActiveEditingBehavior());
}

/// <summary>
/// Debug check for the dynamic behavior
/// </summary>
/// <param name="behavior"></param>
//[Conditional("DEBUG")]
void EditingCoordinator::DebugCheckDynamicBehavior(EditingBehavior* behavior)
{
    (void) behavior;
    Debug::Assert(behavior == GetLassoSelectionBehavior() || behavior == GetSelectionEditingBehavior(),
        "Only GetLassoSelectionBehavior() or GetSelectionEditingBehavior() is dynamic behavior");
}

/// <summary>
/// Debug check for the non dynamic behavior
/// </summary>
/// <param name="behavior"></param>
//[Conditional("DEBUG")]
void EditingCoordinator::DebugCheckNonDynamicBehavior(EditingBehavior* behavior)
{
    (void) behavior;
    Debug::Assert(behavior != GetLassoSelectionBehavior() && behavior != GetSelectionEditingBehavior(),
        "behavior cannot be GetLassoSelectionBehavior() or SelectionEditingBehavior");
}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------

//#region Properties


/// <summary>
/// Helper flag that tells if we're between a preview down and an up.
/// </summary>
bool EditingCoordinator::StylusOrMouseIsDown()
{
     bool stylusDown = false;
     StylusDevice* stylusDevice = Stylus::CurrentStylusDevice();
     if (stylusDevice != nullptr && _inkCanvas.IsStylusOver() && !stylusDevice->InAir())
     {
         stylusDown = true;
     }
     bool mouseDown = (_inkCanvas.IsMouseOver() && Mouse::PrimaryDevice->LeftButton() == MouseButtonState::Pressed);
     if (stylusDown || mouseDown)
     {
         return true;
     }
     return false;
}

/// <summary>
/// Returns the StylusDevice to call DynamicRenderer.Reset with.  Stylus or Mouse
/// must be in a down state.  If the down device is a Mouse, nullptr is returned, since
/// that is what DynamicRenderer.Reset expects for the Mouse.
/// </summary>
/// <returns></returns>
InputDevice* EditingCoordinator::GetInputDeviceForReset()
{
    Debug::Assert((_capturedStylus != nullptr && _capturedMouse == nullptr)
                    || (_capturedStylus == nullptr && _capturedMouse != nullptr),
                    "There must be one and at most one device being captured.");

    if ( _capturedStylus != nullptr && !_capturedStylus->InAir() )
    {
        return _capturedStylus;
    }
    else if ( _capturedMouse != nullptr && _capturedMouse->LeftButton() == MouseButtonState::Pressed )
    {
        return _capturedMouse;
    }

    return nullptr;
}


/// <summary>
/// Lazy init access to the LassoSelectionBehavior
/// </summary>
/// <value></value>
LassoSelectionBehavior* EditingCoordinator::GetLassoSelectionBehavior()
{
     if ( _lassoSelectionBehavior == nullptr )
     {
         _lassoSelectionBehavior = new LassoSelectionBehavior(*this, _inkCanvas);
     }
     return _lassoSelectionBehavior;
}

/// <summary>
/// Lazy init access to the SelectionEditingBehavior
/// </summary>
/// <value></value>
SelectionEditingBehavior* EditingCoordinator::GetSelectionEditingBehavior()
{
     if ( _selectionEditingBehavior == nullptr )
     {
         _selectionEditingBehavior = new SelectionEditingBehavior(*this, _inkCanvas);
     }
     return _selectionEditingBehavior;
}

/// <summary>
/// helper prop to indicate the active editing mode
/// </summary>
InkCanvasEditingMode EditingCoordinator::ActiveEditingMode()
{
     if ( _stylusIsInverted )
     {
         return _inkCanvas.EditingModeInverted();
     }
     return _inkCanvas.EditingMode();
}

/// <summary>
/// Lazy init access to the SelectionEditor
/// </summary>
/// <value></value>
SelectionEditor* EditingCoordinator::GetSelectionEditor()
{
     if ( _selectionEditor == nullptr )
     {
         _selectionEditor = new SelectionEditor(*this, _inkCanvas);
     }
     return _selectionEditor;
}

/// <summary>
/// Gets the mid-stroke state
/// </summary>
bool EditingCoordinator::IsInMidStroke()
{
     return _capturedStylus != nullptr || _capturedMouse != nullptr;
}



//#endregion Properties

//-------------------------------------------------------------------------------
//
// Private Methods
//
//-------------------------------------------------------------------------------

//#region Private Methods

/// <summary>
/// Retrieve the behavior instance based on the EditingMode
/// </summary>
/// <param name="editingMode">EditingMode</param>
/// <returns></returns>
EditingBehavior* EditingCoordinator::GetBehavior(InkCanvasEditingMode editingMode)
{
    EditingBehavior* newBehavior;

    switch ( editingMode )
    {
        case InkCanvasEditingMode::Ink:
        case InkCanvasEditingMode::GestureOnly:
        case InkCanvasEditingMode::InkAndGesture:
            newBehavior = GetInkCollectionBehavior();
            break;
        case InkCanvasEditingMode::Select:
            newBehavior = GetSelectionEditor();
            break;
        case InkCanvasEditingMode::EraseByPoint:
        case InkCanvasEditingMode::EraseByStroke:
            newBehavior = GetEraserBehavior();
            break;
        default:
            // Treat as InkCanvasEditingMode::None. Return nullptr value
            newBehavior = nullptr;
            break;

    }

    return newBehavior;
}


/// <summary>
/// Pushes an EditingBehavior onto our stack, disables any current ones
/// </summary>
/// <param name="newEditingBehavior">The EditingBehavior to activate</param>
void EditingCoordinator::PushEditingBehavior(EditingBehavior* newEditingBehavior)
{
    Debug::Assert(newEditingBehavior != nullptr);

    EditingBehavior* behavior = ActiveEditingBehavior();

    // Deactivate the previous behavior
    if ( behavior != nullptr )
    {
        behavior->Deactivate();
    }

    // Activate the new behavior.
    _activationStack.push_back(newEditingBehavior);
    newEditingBehavior->Activate();
}

/// <summary>
/// Pops an EditingBehavior onto our stack, disables any current ones
/// </summary>
void EditingCoordinator::PopEditingBehavior()
{
    EditingBehavior* behavior = ActiveEditingBehavior();

    if ( behavior != nullptr )
    {
        behavior->Deactivate();
        _activationStack.pop_back();

        behavior = ActiveEditingBehavior();
        if ( ActiveEditingBehavior() != nullptr )
        {
            behavior->Activate();
        }
    }

    return;
}

/// <summary>
/// Handles in-air stylus movements
/// </summary>
void EditingCoordinator::OnInkCanvasStylusInAirOrInRangeMove(StylusEventArgs& args)
{
    // If the current capture is mouse, we should reset the capture.
    if ( _capturedMouse != nullptr )
    {
        if (ActiveEditingBehavior() == GetInkCollectionBehavior() && _inkCanvas.InternalDynamicRenderer() != nullptr)
        {
            // NTRAID#WINDOWS-1378904-2005/11/17-WAYNEZEN
            // When InkCanvas loses the current capture, we should stop the RTI since the App thread are no longer collecting ink.
            // By flipping the Enabled property, the RTI will be reset.
            _inkCanvas.InternalDynamicRenderer()->SetEnabled(false);
            _inkCanvas.InternalDynamicRenderer()->SetEnabled(true);
        }

        // Call ActiveEditingBehavior()->Commit at the end of the routine. The method will cause an external event fired.
        // So it should be invoked after we set up our states.
        ActiveEditingBehavior()->Commit(true);

        // Release capture and do clean-up.
        ReleaseCapture(true);
    }

    UpdateInvertedState(args.GetStylusDevice(), args.Inverted());
}

/// <summary>
/// Handle StylusOutofRange event
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void EditingCoordinator::OnInkCanvasStylusOutOfRange(StylusEventArgs& args)
{
    // Reset the inverted state once OutOfRange has been received.
    UpdateInvertedState(args.GetStylusDevice(), false);
}

/// <summary>
/// InkCanvas.StylusDown handler
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
/// <SecurityNote>
///     Critical:
///             Calls SecurityCritcal method: InitializeCapture and AddStylusPoints
///             Eventually calls SecurityCritical method InkCanvas.RaiseGestureOrStrokeCollected
///
///     TreatAsSafe: This method is called by the input system, from security transparent
///                 code, so it can not be marked critical.  We check the eventArgs.UserInitiated
///                 to verify that the input was user initiated and pass this flag to
///                 InkCanvas.RaiseGestureOrStrokeCollected and use it to decide if we should
///                 perform gesture recognition
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
void EditingCoordinator::OnInkCanvasDeviceDown(InputEventArgs& args)
{
    MouseButtonEventArgs& mouseButtonEventArgs = static_cast<MouseButtonEventArgs&>(args);
    bool resetDynamicRenderer = false;
    //if ( mouseButtonEventArgs != nullptr )
    if (args.Device() == Mouse::PrimaryDevice)
    {
        // NTRADI:WINDOWSOS#1563896-2006/03/20-WAYNEZEN,
        // Note we don't mark Handled for the None EditingMode.
        // Set focus to InkCanvas.
        if ( _inkCanvas.Focus() && ActiveEditingMode() != InkCanvasEditingMode::None )
        {
            mouseButtonEventArgs.SetHandled(true);
        }

        // ISSUE-2005/09/20-WAYNEZEN,
        // We will reevaluate whether we should allow the right-button down for the modes other than the Ink mode.
        // Skip collecting for the non-left buttons.
        if ( mouseButtonEventArgs.ChangedButton() != MouseButton::Left )
        {
            return;
        }

        // NTRAID-WINDOWS#1607286-2006/04/14-WAYNEZEN,
        // If the mouse down event is from a Stylus, make sure we have a correct inverted state.
        if ( mouseButtonEventArgs.GetStylusDevice() != nullptr )
        {
            UpdateInvertedState(mouseButtonEventArgs.GetStylusDevice(), mouseButtonEventArgs.GetStylusDevice()->Inverted());
        }
    }
    else
    {
        // NTRAID-WINDOWS#1395155-2005/12/15-WAYNEZEN,
        // When StylusDown is received, We should check the Invert state again.
        // If there is any change, the ActiveEditingMode() will be updated.
        // The dynamic renderer will be reset in InkCollectionBehavior.OnActivated since the device is under down state.
        StylusEventArgs& stylusEventArgs = static_cast<StylusEventArgs&>(args);
        UpdateInvertedState(stylusEventArgs.GetStylusDevice(), stylusEventArgs.Inverted());
    }

    // If the active behavior is not one of StylusEditingBehavior, don't even bother here.
    IStylusEditing* stylusEditingBehavior = qobject_cast<StylusEditingBehavior*>(ActiveEditingBehavior());

    // We might receive StylusDown from a different device meanwhile we have already captured one.
    // Make sure that we are starting from a fresh state.
    if ( !IsInMidStroke() && stylusEditingBehavior != nullptr )
    {

        bool fSucceeded = false;

        //try
        {
            FinallyHelper final([this, &fSucceeded](){
                if ( !fSucceeded )
                {
                    // Abort the current editing.
                    ActiveEditingBehavior()->Commit(false);

                    // Release capture and do clean-up.
                    ReleaseCapture(IsInMidStroke());
                }
            });
            InputDevice* capturedDevice = nullptr;
            // Capture the stylus (if mouse event make sure to use stylus if generated by a stylus)
            if ( args.Device() == Mouse::PrimaryDevice && mouseButtonEventArgs.GetStylusDevice() != nullptr )
            {
                capturedDevice = mouseButtonEventArgs.GetStylusDevice();
                resetDynamicRenderer = true;
            }
            else
            {
                capturedDevice = args.Device();
            }

            InitializeCapture(capturedDevice, stylusEditingBehavior, args.UserInitiated(), resetDynamicRenderer);

            // The below code might call out StrokeErasing or StrokeErased event.
            // The out-side code could throw exception.
            // We use try/finally block to protect our status.
            fSucceeded = true;
        }
        //finally
        //{
        //    if ( !fSucceeded )
        //    {
                // Abort the current editing.
        //        ActiveEditingBehavior()->Commit(false);

                // Release capture and do clean-up.
        //        ReleaseCapture(IsInMidStroke);
        //    }
        //}
    }
}

/// <summary>
/// InkCanvas.StylusMove handler
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
/// <SecurityNote>
///     Critical:
///             Calls SecurityCritcal method:
///             Eventually calls SecurityCritical method InkCanvas.RaiseGestureOrStrokeCollected
///
///     TreatAsSafe: This method is called by the input system, from security transparent
///                 code, so it can not be marked critical.  We check the eventArgs.UserInitiated
///                 to verify that the input was user initiated and pass this flag to
///                 InkCanvas.RaiseGestureOrStrokeCollected and use it to decide if we should
///                 perform gesture recognition
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
void EditingCoordinator::OnInkCanvasDeviceMove(InputEventArgs& args)
{
    // Make sure that the stylus is the one we captured previously.
    if ( IsInputDeviceCaptured(args.Device()) )
    {
        IStylusEditing* stylusEditingBehavior = qobject_cast<StylusEditingBehavior*>(ActiveEditingBehavior());
        Debug::Assert(stylusEditingBehavior != nullptr || ActiveEditingBehavior() == nullptr,
            "The ActiveEditingBehavior() should be either nullptr (The None mode) or type of IStylusEditing.");

        if ( stylusEditingBehavior != nullptr )
        {
            QSharedPointer<StylusPointCollection> stylusPoints;
            if ( _capturedStylus != nullptr )
            {
                stylusPoints = _capturedStylus->GetStylusPoints(&_inkCanvas, _commonDescription);
            }
            else
            {
                // Make sure we ignore stylus generated mouse events.
                MouseEventArgs& mouseEventArgs = static_cast<MouseEventArgs&>(args);
                if ( args.Device() == Mouse::PrimaryDevice && mouseEventArgs.GetStylusDevice() != nullptr )
                {
                    return;
                }

                stylusPoints.reset(new StylusPointCollection(QVector<QPointF>{ _capturedMouse->GetPosition(&_inkCanvas) }));
            }

            bool fSucceeded = false;

            // The below code might call out StrokeErasing or StrokeErased event.
            // The out-side code could throw exception.
            // We use try/finally block to protect our status.
            //try
            {
                FinallyHelper final([this, &fSucceeded](){
                    if ( !fSucceeded )
                    {
                        // Abort the current editing.
                        ActiveEditingBehavior()->Commit(false);

                        // Release capture and do clean-up.
                        ReleaseCapture(true);
                    }
                });
                stylusEditingBehavior->AddStylusPoints(stylusPoints, args.UserInitiated());
                fSucceeded = true;
            }
            //finally
            //{
            //    if ( !fSucceeded )
            //    {
                    // Abort the current editing.
            //        ActiveEditingBehavior()->Commit(false);

                    // Release capture and do clean-up.
            //        ReleaseCapture(true);
            //    }
            //}
        }
    }
}

/// <summary>
/// InkCanvas.StylusUp handler
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void EditingCoordinator::OnInkCanvasDeviceUp(InputEventArgs& args)
{
    MouseButtonEventArgs& mouseButtonEventArgs = static_cast<MouseButtonEventArgs&>(args);

    StylusDevice* stylusDevice = nullptr;

    // If this is a mouse event, store the StylusDevice
    //if(mouseButtonEventArgs != nullptr)
    if (args.Device() == Mouse::PrimaryDevice)
    {
        stylusDevice = mouseButtonEventArgs.GetStylusDevice();
    }

    // DDVSO:290949
    // In the new WM_POINTER based touch stack, we can have a mouse down promotion
    // that fires after the stylus up (due to how WM_POINTER promotes).  In this case,
    // we would capture the stylus on the down (since it understands mouse promotion)
    // but we would never release capture here since this only checked args.Device.
    // To ensure that we properly release capture, we need to test both the args.Device
    // (which is a Win32MouseDevice on promoted messages) and the StylusDevice present in
    // the mouse event (if it is one).  That allows us to release properly for post-stylus
    // up promoted mouse events.  Note that we do not do this for Update handling as we don't
    // want promoted moves to change state there.
    //
    // WISP COMPAT NOTE:
    // This does not affect WISP as it will release capture on the actual StylusUp, so no capture
    // will exist to allow this to proceed when the promoted mouse up is received.

    // Make sure that the stylus is the one we captured previously.
    if (IsInputDeviceCaptured(args.Device())
        || (stylusDevice != nullptr && IsInputDeviceCaptured(stylusDevice)))
    {
        Debug::Assert(ActiveEditingBehavior() == nullptr || ActiveEditingBehavior()->metaObject() == &StylusEditingBehavior::staticMetaObject,
            "The ActiveEditingBehavior() should be either nullptr (The None mode) or type of IStylusEditing.");

        // Make sure we only look at mouse left button events if watching mouse events.
        if ( _capturedMouse != nullptr )
        {
            if ( args.Device() == Mouse::PrimaryDevice )
            {
                if ( mouseButtonEventArgs.ChangedButton() != MouseButton::Left )
                {
                    return;
                }
            }
        }
        //try
        {
            FinallyHelper final([this]() {
                ReleaseCapture(true);
            });
            // The follow code raises variety editing events.
            // The out-side code could throw exception in the their handlers. We use try/finally block to protect our status.
            if ( ActiveEditingBehavior() != nullptr )
            {
                // Commit the current editing.
                ActiveEditingBehavior()->Commit(true);
            }
        }
        //finally
        //{
            // Call ReleaseCapture(true) at the end of the routine. The method will cause an external event fired.
            // So it should be invoked after we set up our states.
        //    ReleaseCapture(true);
        //}

    }
}

/// <summary>
/// InkCanvas.LostStylusCapture handler
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void EditingCoordinator::OnInkCanvasLostDeviceCapture(EventArgs& args)
{
    (void) args;
    // If user is editing, we have to commit the current operation and reset our state.
    if ( UserIsEditing() )
    {
        // Note ReleaseCapture(false) won't raise any external events. It only reset the states.
        ReleaseCapture(false);

        if ( ActiveEditingBehavior() == GetInkCollectionBehavior() && _inkCanvas.InternalDynamicRenderer() != nullptr )
        {
            // NTRAID#WINDOWS-1378904-2005/11/17-WAYNEZEN
            // When InkCanvas loses the current capture, we should stop the RTI since the App thread are no longer collecting ink.
            // By flipping the Enabled property, the RTI will be reset.
            _inkCanvas.InternalDynamicRenderer()->SetEnabled(false);
            _inkCanvas.InternalDynamicRenderer()->SetEnabled(true);
        }

        // Call ActiveEditingBehavior()->Commit at the end of the routine. The method will cause an external event fired.
        // So it should be invoked after we set up our states.
        ActiveEditingBehavior()->Commit(true);
    }
}

/// <summary>
/// Initialize the capture state
/// </summary>
/// <param name="inputDevice"></param>
/// <param name="stylusEditingBehavior"></param>
/// <param name="userInitiated"></param>
/// <param name="resetDynamicRenderer"></param>
/// <SecurityNote>
///     Critical: Returns critical data from the inputDevice, StylusPointCollection.
/// </SecurityNote>
//[SecurityCritical]
void EditingCoordinator::InitializeCapture(InputDevice* inputDevice, IStylusEditing* stylusEditingBehavior, bool userInitiated, bool resetDynamicRenderer)
{
    Debug::Assert(inputDevice != nullptr, "A nullptr device is passed in.");
    Debug::Assert(stylusEditingBehavior != nullptr, "stylusEditingBehavior cannot be null.");
    Debug::Assert(!IsInMidStroke(), "The previous device hasn't been released yet.");

    QSharedPointer<StylusPointCollection> stylusPoints;

    _capturedStylus = qobject_cast<StylusDevice*>(inputDevice);
    _capturedMouse = qobject_cast<MouseDevice*>(inputDevice);

    // NOTICE-2005/09/15-WAYNEZEN,
    // We assume that the StylusDown always happens before the MouseDown. So, we could safely add the listeners of
    // XXXMove and XXXUp as the below which branchs out the coming mouse or stylus events.
    if ( _capturedStylus != nullptr )
    {

        QSharedPointer<StylusPointCollection> newPoints = _capturedStylus->GetStylusPoints(&_inkCanvas, nullptr);

        _commonDescription =
            StylusPointDescription::GetCommonDescription(_inkCanvas.DefaultStylusPointDescription(),
                newPoints->Description());
        stylusPoints = newPoints->Reformat(_commonDescription);

        if ( resetDynamicRenderer )
        {
            // Reset the dynamic renderer for InkCollectionBehavior
            InkCollectionBehavior* inkCollectionBehavior = static_cast<InkCollectionBehavior*>(stylusEditingBehavior);
            if ( GetInkCollectionBehavior() != nullptr )
            {
                inkCollectionBehavior->ResetDynamicRenderer();
            }
        }

        stylusEditingBehavior->AddStylusPoints(stylusPoints, userInitiated);

        // If the current down device is stylus, we should only listen to StylusMove, StylusUp and LostStylusCapture
        // events.

        _inkCanvas.CaptureStylus();

        if ( _inkCanvas.IsStylusCaptured() && ActiveEditingMode() != InkCanvasEditingMode::None )
        {
            _inkCanvas.AddHandler(Stylus::StylusMoveEvent,
                                  RoutedEventHandlerT<EditingCoordinator, InputEventArgs, &EditingCoordinator::OnInkCanvasDeviceMove>(this));
            _inkCanvas.AddHandler(UIElement::LostStylusCaptureEvent,
                                  RoutedEventHandlerT<EditingCoordinator, EventArgs, &EditingCoordinator::OnInkCanvasLostDeviceCapture>(this));
        }
        else
        {
            _capturedStylus = nullptr;
        }

    }
    else
    {
        Debug::Assert(!resetDynamicRenderer, "The dynamic renderer shouldn't be reset for Mouse");

        _commonDescription = nullptr;

        QVector<QPointF> points = { _capturedMouse->GetPosition(&_inkCanvas) };
        stylusPoints.reset(new StylusPointCollection(points));
        stylusEditingBehavior->AddStylusPoints(stylusPoints, userInitiated);

        // NTRAID:WINDOWSOS#1536974-2006/03/02-WAYNEZEN,
        // CaptureMouse triggers MouseDevice.Synchronize which sends a simulated MouseMove event.
        // So we have to call CaptureMouse after at the end of the initialization.
        // Otherwise, the MouseMove handlers will be executed in the middle of the initialization.
        _inkCanvas.CaptureMouse();

        // The user code could change mode or call release capture when the simulated Move event is received.
        // So we have to check the capture still is on and the mode is correct before hooking up our handlers.
        // If the current down device is mouse, we should only listen to MouseMove, MouseUp and LostMouseCapture
        // events.
        if ( _inkCanvas.IsMouseCaptured() && ActiveEditingMode() != InkCanvasEditingMode::None )
        {
            _inkCanvas.AddHandler(Mouse::MouseMoveEvent,
                                  RoutedEventHandlerT<EditingCoordinator, InputEventArgs, &EditingCoordinator::OnInkCanvasDeviceMove>(this));
            _inkCanvas.AddHandler(UIElement::LostMouseCaptureEvent,
                                  RoutedEventHandlerT<EditingCoordinator, EventArgs, &EditingCoordinator::OnInkCanvasLostDeviceCapture>(this));
        }
        else
        {
            _capturedMouse = nullptr;
        }
    }
}

/// <summary>
/// Clean up the capture state
/// </summary>
/// <param name="releaseDevice"></param>
void EditingCoordinator::ReleaseCapture(bool releaseDevice)
{
    Debug::Assert(IsInMidStroke() || !releaseDevice, "The captured device has been release unexpectly.");

    if ( _capturedStylus != nullptr )
    {
        // The Stylus was captured. Remove the stylus listeners.
        _commonDescription = nullptr;

        _inkCanvas.RemoveHandler(Stylus::StylusMoveEvent,
                                 RoutedEventHandlerT<EditingCoordinator, InputEventArgs, &EditingCoordinator::OnInkCanvasDeviceMove>(this));
        _inkCanvas.RemoveHandler(UIElement::LostStylusCaptureEvent,
                                 RoutedEventHandlerT<EditingCoordinator, EventArgs, &EditingCoordinator::OnInkCanvasLostDeviceCapture>(this));

        _capturedStylus = nullptr;

        if ( releaseDevice )
        {
            // Call ReleaseStylusCapture at the end of the routine. The method will cause an external event fired.
            // So it should be invoked after we set up our states.
            _inkCanvas.ReleaseStylusCapture();
        }
    }
    else if ( _capturedMouse != nullptr )
    {
        // The Mouse was captured. Remove the mouse listeners.
        _inkCanvas.RemoveHandler(Mouse::MouseMoveEvent,
                                 RoutedEventHandlerT<EditingCoordinator, InputEventArgs, &EditingCoordinator::OnInkCanvasDeviceMove>(this));
        _inkCanvas.RemoveHandler(UIElement::LostMouseCaptureEvent,
                                 RoutedEventHandlerT<EditingCoordinator, EventArgs, &EditingCoordinator::OnInkCanvasLostDeviceCapture>(this));

        _capturedMouse = nullptr;
        if ( releaseDevice )
        {
            // Call ReleaseMouseCapture at the end of the routine. The method will cause an external event fired.
            // So it should be invoked after we set up our states.
            _inkCanvas.ReleaseMouseCapture();
        }
    }
}

/// <summary>
/// Retrieve whether a specified device is captured by us.
/// </summary>
/// <param name="inputDevice"></param>
/// <returns></returns>
bool EditingCoordinator::IsInputDeviceCaptured(InputDevice* inputDevice)
{
    Debug::Assert(_capturedStylus == nullptr || _capturedMouse == nullptr, "InkCanvas cannot capture both stylus and mouse at the same time.");
    return (inputDevice == _capturedStylus/* && _capturedStylus->Captured() == &_inkCanvas*/)
        || (inputDevice == _capturedMouse/* && _capturedMouse->Captured() == &_inkCanvas*/);
}

/// <summary>
/// Return the cursor of the active behavior
/// </summary>
/// <returns></returns>
QCursor EditingCoordinator::GetActiveBehaviorCursor()
{
    EditingBehavior* behavior = ActiveEditingBehavior();

    if ( behavior == nullptr )
    {
        // Return the default Arrow cursor for the None mode.
        return Qt::ArrowCursor;
    }

    // Retrieve the cursor.
    QCursor cursor = behavior->Cursor();

    // Clean up the dirty flag when it's done.
    if ( !GetCursorValid(behavior) )
    {
        SetCursorValid(behavior, true);
    }

    return cursor;
}

/// <summary>
/// A private method which returns the valid flag of behaviors' cursor.
/// </summary>
/// <param name="behavior"></param>
/// <returns></returns>
bool EditingCoordinator::GetCursorValid(EditingBehavior* behavior)
{
    BehaviorValidFlag flag = GetBehaviorCursorFlag(behavior);
    return GetBitFlag(flag);
}

/// <summary>
/// A private method which sets/resets the valid flag of behaviors' cursor
/// </summary>
/// <param name="behavior"></param>
/// <param name="isValid"></param>
void EditingCoordinator::EditingCoordinator::SetCursorValid(EditingBehavior* behavior, bool isValid)
{
    BehaviorValidFlag flag = GetBehaviorCursorFlag(behavior);
    SetBitFlag(flag, isValid);
}

/// <summary>
/// A private method which returns the valid flag of behaviors' cursor.
/// </summary>
/// <param name="behavior"></param>
/// <returns></returns>
bool EditingCoordinator::GetTransformValid(EditingBehavior* behavior)
{
    BehaviorValidFlag flag = GetBehaviorTransformFlag(behavior);
    return GetBitFlag(flag);
}

/// <summary>
/// A private method which sets/resets the valid flag of behaviors' cursor
/// </summary>
/// <param name="behavior"></param>
/// <param name="isValid"></param>
void EditingCoordinator::SetTransformValid(EditingBehavior* behavior, bool isValid)
{
    BehaviorValidFlag flag = GetBehaviorTransformFlag(behavior);
    SetBitFlag(flag, isValid);
}

/// <summary>
/// GetBitFlag
/// </summary>
/// <param name="flag"></param>
/// <returns></returns>
bool EditingCoordinator::GetBitFlag(BehaviorValidFlag flag)
{
    return (_behaviorValidFlag & flag) != 0;
}

/// <summary>
/// SetBitFlag
/// </summary>
/// <param name="flag"></param>
/// <param name="value"></param>
void EditingCoordinator::EditingCoordinator::SetBitFlag(BehaviorValidFlag flag, bool value)
{
    if ( value )
    {
        _behaviorValidFlag |= flag;
    }
    else
    {
        _behaviorValidFlag &= (~flag);
    }

}

/// <summary>
/// A helper returns behavior cursor flag from a behavior instance
/// </summary>
/// <param name="behavior"></param>
/// <returns></returns>
EditingCoordinator::BehaviorValidFlag EditingCoordinator::GetBehaviorCursorFlag(EditingBehavior* behavior)
{
    BehaviorValidFlag flag = static_cast<BehaviorValidFlag>(0);

    if ( behavior == GetInkCollectionBehavior() )
    {
        flag = BehaviorValidFlag::InkCollectionBehaviorCursorValid;
    }
    else if ( behavior == GetEraserBehavior() )
    {
        flag = BehaviorValidFlag::EraserBehaviorCursorValid;
    }
    else if ( behavior == GetLassoSelectionBehavior() )
    {
        flag = BehaviorValidFlag::LassoSelectionBehaviorCursorValid;
    }
    else if ( behavior == GetSelectionEditingBehavior() )
    {
        flag = BehaviorValidFlag::SelectionEditingBehaviorCursorValid;
    }
    else if ( behavior == GetSelectionEditor())
    {
        flag = BehaviorValidFlag::SelectionEditorCursorValid;
    }
    else
    {
        Debug::Assert(false, "Unknown behavior");
    }

    return flag;
}

/// <summary>
/// A helper returns behavior transform flag from a behavior instance
/// </summary>
/// <param name="behavior"></param>
/// <returns></returns>
EditingCoordinator::BehaviorValidFlag EditingCoordinator::GetBehaviorTransformFlag(EditingBehavior* behavior)
{
    BehaviorValidFlag flag = static_cast<BehaviorValidFlag>(0);

    if ( behavior == GetInkCollectionBehavior() )
    {
        flag = BehaviorValidFlag::InkCollectionBehaviorTransformValid;
    }
    else if ( behavior == GetEraserBehavior() )
    {
        flag = BehaviorValidFlag::EraserBehaviorTransformValid;
    }
    else if ( behavior == GetLassoSelectionBehavior() )
    {
        flag = BehaviorValidFlag::LassoSelectionBehaviorTransformValid;
    }
    else if ( behavior == GetSelectionEditingBehavior() )
    {
        flag = BehaviorValidFlag::SelectionEditingBehaviorTransformValid;
    }
    else if ( behavior == GetSelectionEditor())
    {
        flag = BehaviorValidFlag::SelectionEditorTransformValid;
    }
    else
    {
        Debug::Assert(false, "Unknown behavior");
    }

    return flag;
}

void EditingCoordinator::ChangeEditingBehavior(EditingBehavior* newBehavior)
{
    Debug::Assert(!IsInMidStroke(), "ChangeEditingBehavior cannot be called in a mid-stroke");
    Debug::Assert(_activationStack.size() <= 1, "The behavior stack has to contain at most one behavior when user is not editing.");

    //try
    {
        FinallyHelper final([this, newBehavior]() {
            if ( ActiveEditingBehavior() != nullptr )
            {
                // Pop the old behavior.
                PopEditingBehavior();
            }

            // Push the new behavior
            if ( newBehavior != nullptr )
            {
                PushEditingBehavior(newBehavior);
            }

            RoutedEventArgs e(InkCanvas::ActiveEditingModeChangedEvent, &_inkCanvas);
            _inkCanvas.RaiseActiveEditingModeChanged(e);
        });
        _inkCanvas.ClearSelection(true);
    }
    //finally
    //{
    //    if ( ActiveEditingBehavior() != nullptr )
    //    {
            // Pop the old behavior.
    //        PopEditingBehavior();
    //    }

        // Push the new behavior
    //    if ( newBehavior != nullptr )
    //    {
    //        PushEditingBehavior(newBehavior);
    //    }

    //    _inkCanvas.RaiseActiveEditingMode()Changed(new RoutedEventArgs(InkCanvas.ActiveEditingModeChangedEvent, _inkCanvas));
    //}
}

/// <summary>
/// Update the Inverted state
/// </summary>
/// <param name="stylusDevice"></param>
/// <param name="stylusIsInverted"></param>
/// <returns>true if the behavior is updated</returns>
bool EditingCoordinator::UpdateInvertedState(StylusDevice* stylusDevice, bool stylusIsInverted)
{
    if ( !IsInMidStroke() ||
        ( IsInMidStroke() && IsInputDeviceCaptured(stylusDevice) ))
    {
        if ( stylusIsInverted != _stylusIsInverted )
        {
            _stylusIsInverted = stylusIsInverted;

            //
            // this can change editing state
            //
            UpdateActiveEditingState();

            return true;
        }
    }

    return false;
}

//#endregion Private Methods

//-------------------------------------------------------------------------------
//
// Private Properties
//
//-------------------------------------------------------------------------------

//#region Private Properties

/// <summary>
/// Gets the top level active EditingBehavior
/// </summary>
/// <value></value>
EditingBehavior* EditingCoordinator::ActiveEditingBehavior()
{
    EditingBehavior* EditingBehavior = nullptr;
    if ( _activationStack.size() > 0 )
    {
        EditingBehavior = _activationStack.back();
    }
    return EditingBehavior;
}

/// <summary>
/// Lazy init access to the InkCollectionBehavior
/// </summary>
/// <value></value>
InkCollectionBehavior* EditingCoordinator::GetInkCollectionBehavior()
{
    if ( _inkCollectionBehavior == nullptr )
    {
        _inkCollectionBehavior = new InkCollectionBehavior(*this, _inkCanvas);
    }
    return _inkCollectionBehavior;
}

/// <summary>
/// Lazy init access to the EraserBehavior
/// </summary>
/// <value></value>
EraserBehavior* EditingCoordinator::GetEraserBehavior()
{
    if ( _eraserBehavior == nullptr )
    {
        _eraserBehavior = new EraserBehavior(*this, _inkCanvas);
    }
    return _eraserBehavior;
}
