#include "selectioneditor.h"
#include "inkcanvas.h"
#include "mousebuttoneventargs.h"
#include "selectioneditingbehavior.h"
#include "lassoselectionbehavior.h"
#include "inkcanvasselection.h"
#include "inkcanvasselectionadorner.h"

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors

/// <summary>
/// SelectionEditor constructor
/// </summary>
SelectionEditor::SelectionEditor(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : EditingBehavior (editingCoordinator, inkCanvas)
{
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Internal Methods
//
//-------------------------------------------------------------------------------

//#region Internal Methods

/// <summary>
/// InkCanvas' Selection is changed
/// </summary>
void SelectionEditor::OnInkCanvasSelectionChanged()
{
    QPointF currentPosition = Mouse::PrimaryDevice->GetPosition(&GetInkCanvas().SelectionAdorner());
    UpdateSelectionCursor(currentPosition);
}

//#endregion Internal Methods

//-------------------------------------------------------------------------------
//
// Protected Methods
//
//-------------------------------------------------------------------------------

//#region Protected Methods

/// <summary>
/// Attach to new scope as defined by this element.
/// </summary>
void SelectionEditor::OnActivate()
{
    // Add mouse event handles to GetInkCanvas().SelectionAdorner()
    GetInkCanvas().SelectionAdorner().AddHandler(Mouse::MouseDownEvent,
            RoutedEventHandlerT<SelectionEditor, MouseButtonEventArgs, &SelectionEditor::OnAdornerMouseButtonDownEvent>(this));
    GetInkCanvas().SelectionAdorner().AddHandler(Mouse::MouseMoveEvent,
            RoutedEventHandlerT<SelectionEditor, MouseEventArgs, &SelectionEditor::OnAdornerMouseMoveEvent>(this));
    GetInkCanvas().SelectionAdorner().AddHandler(Mouse::MouseEnterEvent,
            RoutedEventHandlerT<SelectionEditor, MouseEventArgs, &SelectionEditor::OnAdornerMouseMoveEvent>(this));
    GetInkCanvas().SelectionAdorner().AddHandler(Mouse::MouseLeaveEvent,
            RoutedEventHandlerT<SelectionEditor, MouseEventArgs, &SelectionEditor::OnAdornerMouseLeaveEvent>(this));

    QPointF currentPosition = Mouse::PrimaryDevice->GetPosition(&GetInkCanvas().SelectionAdorner());
    UpdateSelectionCursor(currentPosition);
}

/// <summary>
/// Called when the SelectionEditor is detached
/// </summary>
void SelectionEditor::OnDeactivate()
{
    // Remove all event hanlders.
    GetInkCanvas().SelectionAdorner().RemoveHandler(Mouse::MouseDownEvent,
            RoutedEventHandlerT<SelectionEditor, MouseButtonEventArgs, &SelectionEditor::OnAdornerMouseButtonDownEvent>(this));
    GetInkCanvas().SelectionAdorner().RemoveHandler(Mouse::MouseMoveEvent,
            RoutedEventHandlerT<SelectionEditor, MouseEventArgs, &SelectionEditor::OnAdornerMouseMoveEvent>(this));
    GetInkCanvas().SelectionAdorner().RemoveHandler(Mouse::MouseEnterEvent,
            RoutedEventHandlerT<SelectionEditor, MouseEventArgs, &SelectionEditor::OnAdornerMouseMoveEvent>(this));
    GetInkCanvas().SelectionAdorner().RemoveHandler(Mouse::MouseLeaveEvent,
            RoutedEventHandlerT<SelectionEditor, MouseEventArgs, &SelectionEditor::OnAdornerMouseLeaveEvent>(this));

}

/// <summary>
/// OnCommit
/// </summary>
/// <param name="commit"></param>
void SelectionEditor::OnCommit(bool commit)
{
    (void)commit;
    // Nothing to do.
}

/// <summary>
/// Retrieve the cursor
/// </summary>
/// <returns></returns>
QCursor SelectionEditor::GetCurrentCursor()
{
    // NTRAID:WINDOWSOS#1648430-2006/05/12-WAYNEZEN,
    // We only show the selection related cursor when the mouse is over the SelectionAdorner().
    // If mouse is outside of the SelectionAdorner, we let the system to pick up the default cursor.
    if ( GetInkCanvas().SelectionAdorner().IsMouseOver() )
    {
        return PenCursorManager.GetSelectionCursor(_hitResult,
            (GetInkCanvas().GetFlowDirection() == FlowDirection::RightToLeft));
    }
    else
    {
        return QCursor();
    }
}

//#endregion Protected Methods

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// SelectionAdorner MouseButtonDown
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
/// <SecurityNote>
///     Critical: Calls critical method GetEditingCoordinator().ActivateDynamicBehavior
///
///     TreatAsSafe: Doesn't handle critical data.  Also, this method is called by
///         SecurityTransparent code in the input system and can not be marked SecurityCritical.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
void SelectionEditor::OnAdornerMouseButtonDownEvent(MouseButtonEventArgs& args)
{
    // If the ButtonDown is raised by RightMouse, we should just bail out.
    if ( (args.GetStylusDevice() == nullptr && args.LeftButton() != MouseButtonState::Pressed) )
    {
        return;
    }

    QPointF pointOnSelectionAdorner = args.GetPosition(&GetInkCanvas().SelectionAdorner());

    InkCanvasSelectionHitResult hitResult = InkCanvasSelectionHitResult::None;

    // Check if we should start resizing/moving
    hitResult = HitTestOnSelectionAdorner(pointOnSelectionAdorner);
    if ( hitResult != InkCanvasSelectionHitResult::None )
    {
        // We always use MouseDevice for the selection editing.
        GetEditingCoordinator().ActivateDynamicBehavior(GetEditingCoordinator().GetSelectionEditingBehavior(), args.Device());
    }
    else
    {
        //
        //push selection and we're done
        //
        // If the current captured device is Stylus, we should activate the LassoSelectionBehavior with
        // the Stylus. Otherwise, use Mouse::
        GetEditingCoordinator().ActivateDynamicBehavior(GetEditingCoordinator().GetLassoSelectionBehavior(),
            args.GetStylusDevice() != nullptr ? args.GetStylusDevice() : args.Device());
    }
}

/// <summary>
/// Selection Adorner MouseMove
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
/// <SecurityNote>
///     Critical: Calls critical method GetEditingCoordinator().ActivateDynamicBehavior
///
///     TreatAsSafe: Doesn't handle critical data.  Also, this method is called by
///         SecurityTransparent code in the input system and can not be marked SecurityCritical.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
void SelectionEditor::OnAdornerMouseMoveEvent(MouseEventArgs& args)
{
    QPointF pointOnSelectionAdorner = args.GetPosition(&GetInkCanvas().SelectionAdorner());
    UpdateSelectionCursor(pointOnSelectionAdorner);
}

/// <summary>
/// Adorner MouseLeave Handler
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void SelectionEditor::OnAdornerMouseLeaveEvent(MouseEventArgs& args)
{
    (void) args;
    // We are leaving the adorner, update our cursor.
    GetEditingCoordinator().InvalidateBehaviorCursor(this);
}

/// <summary>
/// Hit test for the grab handles
/// </summary>
/// <param name="position"></param>
/// <returns></returns>
InkCanvasSelectionHitResult SelectionEditor::HitTestOnSelectionAdorner(QPointF const & position)
{
    InkCanvasSelectionHitResult hitResult = InkCanvasSelectionHitResult::None;

    if ( GetInkCanvas().GetInkCanvasSelection().HasSelection() )
    {
        // First, do hit test on the adorner
        hitResult = GetInkCanvas().SelectionAdorner().SelectionHandleHitTest(position);

        // Now, check if ResizeEnabled or MoveEnabled has been set.
        // If so, reset the grab handle.
        if ( hitResult >= InkCanvasSelectionHitResult::TopLeft && hitResult <= InkCanvasSelectionHitResult::Left )
        {
            hitResult = GetInkCanvas().ResizeEnabled() ? hitResult : InkCanvasSelectionHitResult::None;
        }
        else if ( hitResult == InkCanvasSelectionHitResult::Selection )
        {
            hitResult = GetInkCanvas().MoveEnabled() ? hitResult : InkCanvasSelectionHitResult::None;
        }
    }

    return hitResult;
}

/// <summary>
/// This method updates the cursor when the mouse is hovering ont the selection adorner.
/// It is called from
///  OnAdornerMouseLeaveEvent
///  OnAdornerMouseEvent
/// </summary>
/// <param name="hitPoint">the handle is being hit</param>
void SelectionEditor::UpdateSelectionCursor(QPointF const & hitPoint)
{
    InkCanvasSelectionHitResult hitResult = HitTestOnSelectionAdorner(hitPoint);
    if ( _hitResult != hitResult )
    {
        // Keep the current handle
        _hitResult = hitResult;
        GetEditingCoordinator().InvalidateBehaviorCursor(this);
    }
}
