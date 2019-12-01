#ifndef EDITINGCOORDINATOR_H
#define EDITINGCOORDINATOR_H

#include "Windows/Controls/editingmode.h"

#include <QObject>
#include <QVector>
#include <QCursor>
#include <QSharedPointer>

class InkCanvas;
class EventArgs;
class EditingBehavior;
class InkCollectionBehavior;
class EraserBehavior;
class StylusEditingBehavior;
class LassoSelectionBehavior;
class SelectionEditingBehavior;
class SelectionEditor;
class InputDevice;
class StylusEventArgs;
class InputEventArgs;
class IStylusEditing;
class StylusDevice;
class StylusPointDescription;
class MouseDevice;

// namespace MS.Internal.Ink

/// <summary>
/// Internal class that represents the editing stack of InkCanvas
/// Please see the design detain at http://tabletpc/longhorn/Specs/Mid-Stroke%20and%20Pen%20Cursor%20Dev%20Design.mht
/// </summary>
class EditingCoordinator : public QObject
{
    Q_OBJECT

    /// <summary>
    /// Enum values which represent the cursor valid flag for each EditingBehavior.
    /// </summary>
    //[Flags]
    enum BehaviorValidFlag
    {
        InkCollectionBehaviorCursorValid      = 0x00000001,
        EraserBehaviorCursorValid             = 0x00000002,
        LassoSelectionBehaviorCursorValid     = 0x00000004,
        SelectionEditingBehaviorCursorValid   = 0x00000008,
        SelectionEditorCursorValid            = 0x00000010,
        InkCollectionBehaviorTransformValid   = 0x00000020,
        EraserBehaviorTransformValid          = 0x00000040,
        LassoSelectionBehaviorTransformValid  = 0x00000080,
        SelectionEditingBehaviorTransformValid= 0x00000100,
        SelectionEditorTransformValid         = 0x00000200,
    };

    Q_DECLARE_FLAGS(BehaviorValidFlags, BehaviorValidFlag)

public:
    /// <summary>
    /// Constructors
    /// </summary>
    /// <param name="inkCanvas">InkCanvas instance</param>
    EditingCoordinator(InkCanvas& inkCanvas);

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
    void ActivateDynamicBehavior(EditingBehavior* dynamicBehavior, InputDevice* inputDevice);

    /// <summary>
    /// Deactivate a dynamic behavior
    /// Called from:
    ///     EditingBehavior.Commit
    /// </summary>
    void DeactivateDynamicBehavior();

    /// <summary>
    /// Update the current active EditingMode
    /// Called from:
    ///     EditingCoordinator.UpdateInvertedState
    ///     InkCanvas.Initialize()
    /// </summary>
    void UpdateActiveEditingState();

    /// <summary>
    /// Update the EditingMode
    /// Called from:
    ///     InkCanvas.RaiseEditingModeChanged
    ///     InkCanvas.RaiseEditingModeInvertedChanged
    /// </summary>
    /// <param name="inverted">A flage which indicates whether the new mode is for the Inverted state</param>
    void UpdateEditingState(bool inverted);

    /// <summary>
    /// Update the PointEraerCursor
    /// Called from:
    ///     InkCanvas.set_EraserShape
    /// </summary>
    void UpdatePointEraserCursor();

    /// <summary>
    /// InvalidateTransform
    ///     Called by: InkCanvas.OnPropertyChanged
    /// </summary>
    void InvalidateTransform();

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
    void InvalidateBehaviorCursor(EditingBehavior* behavior);

    /// <summary>
    /// Check whether the cursor of the specified behavior is valid
    /// </summary>
    /// <param name="behavior">EditingBehavior</param>
    /// <returns>True if the cursor doesn't need to be updated</returns>
    bool IsCursorValid(EditingBehavior* behavior);

    /// <summary>
    /// Check whether the cursor of the specified behavior is valid
    /// </summary>
    /// <param name="behavior">EditingBehavior</param>
    /// <returns>True if the cursor doesn't need to be updated</returns>
    bool IsTransformValid(EditingBehavior* behavior);

    /// <summary>
    /// Change to another StylusEditing mode or None mode.
    /// </summary>
    /// <param name="sourceBehavior"></param>
    /// <param name="newMode"></param>
    /// <returns></returns>
    IStylusEditing* ChangeStylusEditingMode(StylusEditingBehavior* sourceBehavior, InkCanvasEditingMode newMode);

    /// <summary>
    /// Debug Checker
    /// </summary>
    /// <param name="behavior"></param>
    //[Conditional("DEBUG")]
    void DebugCheckActiveBehavior(EditingBehavior* behavior);

private:
    /// <summary>
    /// Debug check for the dynamic behavior
    /// </summary>
    /// <param name="behavior"></param>
    //[Conditional("DEBUG")]
    void DebugCheckDynamicBehavior(EditingBehavior* behavior);

    /// <summary>
    /// Debug check for the non dynamic behavior
    /// </summary>
    /// <param name="behavior"></param>
    //[Conditional("DEBUG")]
    void DebugCheckNonDynamicBehavior(EditingBehavior* behavior);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

public:
    /// <summary>
    /// Gets / sets whether move is enabled or note
    /// </summary>
    bool MoveEnabled()
    {
        return _moveEnabled;
    }
    void SetMoveEnabled(bool value)
    {
        _moveEnabled = value;
    }

    /// <summary>
    /// The property that indicates if the user is interacting with the current InkCanvas
    /// </summary>
    bool UserIsEditing()
    {
         return _userIsEditing;
    }
    void SetUserIsEditing(bool value)
    {
         _userIsEditing = value;
    }

    /// <summary>
    /// Helper flag that tells if we're between a preview down and an up.
    /// </summary>
    bool StylusOrMouseIsDown();

    /// <summary>
    /// Returns the StylusDevice to call DynamicRenderer.Reset with.  Stylus or Mouse
    /// must be in a down state.  If the down device is a Mouse, nullptr is returned, since
    /// that is what DynamicRenderer.Reset expects for the Mouse.
    /// </summary>
    /// <returns></returns>
    InputDevice* GetInputDeviceForReset();

    /// <summary>
    /// Gets / sets whether resize is enabled or note
    /// </summary>
    bool ResizeEnabled()
    {
        return _resizeEnabled;
    }
    void SetResizeEnabled(bool value)
    {
        _resizeEnabled = value;
    }

    /// <summary>
    /// Lazy init access to the LassoSelectionBehavior
    /// </summary>
    /// <value></value>
    LassoSelectionBehavior* GetLassoSelectionBehavior();

    /// <summary>
    /// Lazy init access to the SelectionEditingBehavior
    /// </summary>
    /// <value></value>
    SelectionEditingBehavior* GetSelectionEditingBehavior();

    /// <summary>
    /// helper prop to indicate the active editing mode
    /// </summary>
    InkCanvasEditingMode ActiveEditingMode();

    /// <summary>
    /// Lazy init access to the SelectionEditor
    /// </summary>
    /// <value></value>
    SelectionEditor* GetSelectionEditor();

    /// <summary>
    /// Gets the mid-stroke state
    /// </summary>
    bool IsInMidStroke();


    /// <summary>
    /// Returns Stylus Inverted state
    /// </summary>
    bool IsStylusInverted()
    {
         return _stylusIsInverted;
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
    EditingBehavior* GetBehavior(InkCanvasEditingMode editingMode);


    /// <summary>
    /// Pushes an EditingBehavior onto our stack, disables any current ones
    /// </summary>
    /// <param name="newEditingBehavior">The EditingBehavior to activate</param>
    void PushEditingBehavior(EditingBehavior* newEditingBehavior);

    /// <summary>
    /// Pops an EditingBehavior onto our stack, disables any current ones
    /// </summary>
    void PopEditingBehavior();

    /// <summary>
    /// Handles in-air stylus movements
    /// </summary>
    void OnInkCanvasStylusInAirOrInRangeMove(StylusEventArgs& args);

    /// <summary>
    /// Handle StylusOutofRange event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnInkCanvasStylusOutOfRange(StylusEventArgs& args);

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
    void OnInkCanvasDeviceDown(InputEventArgs& args);

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
    void OnInkCanvasDeviceMove(InputEventArgs& args);

    /// <summary>
    /// InkCanvas.StylusUp handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnInkCanvasDeviceUp(InputEventArgs& args);

    /// <summary>
    /// InkCanvas.LostStylusCapture handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnInkCanvasLostDeviceCapture(EventArgs& args);

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
    void InitializeCapture(InputDevice* inputDevice, IStylusEditing* stylusEditingBehavior, bool userInitiated, bool resetDynamicRenderer);

    /// <summary>
    /// Clean up the capture state
    /// </summary>
    /// <param name="releaseDevice"></param>
    void ReleaseCapture(bool releaseDevice);

    /// <summary>
    /// Retrieve whether a specified device is captured by us.
    /// </summary>
    /// <param name="inputDevice"></param>
    /// <returns></returns>
    bool IsInputDeviceCaptured(InputDevice* inputDevice);

    /// <summary>
    /// Return the cursor of the active behavior
    /// </summary>
    /// <returns></returns>
    QCursor GetActiveBehaviorCursor();

    /// <summary>
    /// A private method which returns the valid flag of behaviors' cursor.
    /// </summary>
    /// <param name="behavior"></param>
    /// <returns></returns>
    bool GetCursorValid(EditingBehavior* behavior);

    /// <summary>
    /// A private method which sets/resets the valid flag of behaviors' cursor
    /// </summary>
    /// <param name="behavior"></param>
    /// <param name="isValid"></param>
    void SetCursorValid(EditingBehavior* behavior, bool isValid);

    /// <summary>
    /// A private method which returns the valid flag of behaviors' cursor.
    /// </summary>
    /// <param name="behavior"></param>
    /// <returns></returns>
    bool GetTransformValid(EditingBehavior* behavior);

    /// <summary>
    /// A private method which sets/resets the valid flag of behaviors' cursor
    /// </summary>
    /// <param name="behavior"></param>
    /// <param name="isValid"></param>
    void SetTransformValid(EditingBehavior* behavior, bool isValid);

    /// <summary>
    /// GetBitFlag
    /// </summary>
    /// <param name="flag"></param>
    /// <returns></returns>
    bool GetBitFlag(BehaviorValidFlag flag);

    /// <summary>
    /// SetBitFlag
    /// </summary>
    /// <param name="flag"></param>
    /// <param name="value"></param>
    void SetBitFlag(BehaviorValidFlag flag, bool value);

    /// <summary>
    /// A helper returns behavior cursor flag from a behavior instance
    /// </summary>
    /// <param name="behavior"></param>
    /// <returns></returns>
    BehaviorValidFlag GetBehaviorCursorFlag(EditingBehavior* behavior);

    /// <summary>
    /// A helper returns behavior transform flag from a behavior instance
    /// </summary>
    /// <param name="behavior"></param>
    /// <returns></returns>
    BehaviorValidFlag GetBehaviorTransformFlag(EditingBehavior* behavior);

    void ChangeEditingBehavior(EditingBehavior* newBehavior);

    /// <summary>
    /// Update the Inverted state
    /// </summary>
    /// <param name="stylusDevice"></param>
    /// <param name="stylusIsInverted"></param>
    /// <returns>true if the behavior is updated</returns>
    bool UpdateInvertedState(StylusDevice* stylusDevice, bool stylusIsInverted);

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
    EditingBehavior* ActiveEditingBehavior();

    /// <summary>
    /// Lazy init access to the InkCollectionBehavior
    /// </summary>
    /// <value></value>
    InkCollectionBehavior* GetInkCollectionBehavior();

    /// <summary>
    /// Lazy init access to the EraserBehavior
    /// </summary>
    /// <value></value>
    EraserBehavior* GetEraserBehavior();


    //#endregion Private Properties

    //-------------------------------------------------------------------------------
    //
    // Private Enum
    //
    //-------------------------------------------------------------------------------

    //#region Private Enum

    //#endregion Private Enum

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------


    /// <summary>
    /// The InkCanvas this EditingStack is being used in
    /// </summary>
private:
    InkCanvas& _inkCanvas;
    QVector<EditingBehavior*> _activationStack;
    InkCollectionBehavior* _inkCollectionBehavior;
    EraserBehavior* _eraserBehavior;
    LassoSelectionBehavior* _lassoSelectionBehavior;
    SelectionEditingBehavior* _selectionEditingBehavior;
    SelectionEditor* _selectionEditor;
    bool _moveEnabled = true;
    bool _resizeEnabled = true;
    bool _userIsEditing = false;

    /// <summary>
    /// Flag that indicates if the stylus is inverted
    /// </summary>
    bool _stylusIsInverted = false;

    QSharedPointer<StylusPointDescription>  _commonDescription;
    StylusDevice*            _capturedStylus;
    MouseDevice*             _capturedMouse;

    // Fields related to cursor and transform.
    BehaviorValidFlags       _behaviorValidFlag;
};

#endif // EDITINGCOORDINATOR_H
