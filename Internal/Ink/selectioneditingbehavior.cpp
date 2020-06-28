#include "Internal/Ink/selectioneditingbehavior.h"
#include "Internal/doubleutil.h"
#include "Windows/Input/mousedevice.h"
#include "Windows/Controls/inkcanvas.h"
#include "Internal/Ink/editingcoordinator.h"
#include "Internal/Ink/inkcanvasselection.h"
#include "Internal/Controls/inkcanvasselectionadorner.h"
#include "Windows/Input/mouseeventargs.h"
#include "Windows/Input/mousebuttoneventargs.h"
#include "Internal/Ink/pencursormanager.h"

INKCANVAS_BEGIN_NAMESPACE

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors

/// <summary>
/// SelectionEditingBehavior constructor
/// </summary>
SelectionEditingBehavior::SelectionEditingBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : EditingBehavior(editingCoordinator, inkCanvas)
{
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Protected Methods
//
//-------------------------------------------------------------------------------

//#region Protected Methods

/// <summary>
///     Attaching to the element, we get attached in StylusDown
/// </summary>
void SelectionEditingBehavior::OnActivate()
{
    _actionStarted = false;

    // Capture the Mouse::
    InitializeCapture();

    // Hittest for the grab handle
    MouseDevice* mouse = Mouse::PrimaryDevice;

    _hitResult = GetInkCanvas().SelectionAdorner().SelectionHandleHitTest(
            mouse->GetPosition(&GetInkCanvas().SelectionAdorner()));

    //Debug.Assert(_hitResult != InkCanvasSelectionHitResult::None);
    GetEditingCoordinator().InvalidateBehaviorCursor(this);

    // Get the current selection bounds.
    _selectionRect = GetInkCanvas().GetSelectionBounds( );

    // Set the initial tracking position and rectangle
    _previousLocation = mouse->GetPosition(&GetInkCanvas().SelectionAdorner());
    _previousRect = _selectionRect;

    // Start the feedback rubber band.
    GetInkCanvas().GetInkCanvasSelection().StartFeedbackAdorner(_selectionRect, _hitResult);

    // Add handlers to the mouse events.
    GetInkCanvas().SelectionAdorner().AddHandler(Mouse::MouseUpEvent,
            RoutedEventHandlerT<SelectionEditingBehavior, MouseButtonEventArgs, &SelectionEditingBehavior::OnMouseUp>(this));
    GetInkCanvas().SelectionAdorner().AddHandler(Mouse::MouseMoveEvent,
            RoutedEventHandlerT<SelectionEditingBehavior, MouseEventArgs, &SelectionEditingBehavior::OnMouseMove>(this));
    GetInkCanvas().SelectionAdorner().AddHandler(UIElement::LostMouseCaptureEvent,
            RoutedEventHandlerT<SelectionEditingBehavior, MouseEventArgs, &SelectionEditingBehavior::OnLostMouseCapture>(this));

}

/// <summary>
/// Called when the ResizeEditingBehavior is detached
/// </summary>
void SelectionEditingBehavior::OnDeactivate()
{
    // Remove the mouse event handlers.
    GetInkCanvas().SelectionAdorner().RemoveHandler(Mouse::MouseUpEvent,
            RoutedEventHandlerT<SelectionEditingBehavior, MouseButtonEventArgs, &SelectionEditingBehavior::OnMouseUp>(this));
    GetInkCanvas().SelectionAdorner().RemoveHandler(Mouse::MouseMoveEvent,
            RoutedEventHandlerT<SelectionEditingBehavior, MouseEventArgs, &SelectionEditingBehavior::OnMouseMove>(this));
    GetInkCanvas().SelectionAdorner().RemoveHandler(UIElement::LostMouseCaptureEvent,
            RoutedEventHandlerT<SelectionEditingBehavior, MouseEventArgs, &SelectionEditingBehavior::OnLostMouseCapture>(this));
}

void SelectionEditingBehavior::OnCommit(bool commit)
{
    // Release the inputs.
    ReleaseCapture(true, commit);
}

QCursor SelectionEditingBehavior::GetCurrentCursor()
{
    return PenCursorManager::GetSelectionCursor(_hitResult,
        (GetInkCanvas().GetFlowDirection() == FlowDirection::RightToLeft));
}

//#endregion Protected Methods

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// The handler of the MouseMove event
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void SelectionEditingBehavior::OnMouseMove(MouseEventArgs& args)
{
    // Get the current mouse location.
    Point curPoint = args.GetPosition(&GetInkCanvas().SelectionAdorner());

    // Check if we have a mouse movement at all.
    if ( !DoubleUtil::AreClose(curPoint.X(), _previousLocation.X())
        || !DoubleUtil::AreClose(curPoint.Y(), _previousLocation.Y()) )
    {
        // We won't start the move until we really see a movement.
        if ( !_actionStarted )
        {
            _actionStarted = true;
        }

        // Get the new rectangle
        Rect newRect = ChangeFeedbackRectangle(curPoint);

        // Update the feedback rubber band and record the current tracking rectangle.
        GetInkCanvas().GetInkCanvasSelection().UpdateFeedbackAdorner(newRect);
        _previousRect = newRect;
    }
}

/// <summary>
/// The handler of the MouseUp event
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void SelectionEditingBehavior::OnMouseUp(MouseButtonEventArgs& args)
{
    // We won't start the move until we really see a movement.
    if ( _actionStarted )
    {
        Point point = args.GetPosition(&GetInkCanvas().SelectionAdorner());
        _previousRect = ChangeFeedbackRectangle(point);
    }

    Commit(true);
}

/// <summary>
/// GetInkCanvas().LostStylusCapture handler
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void SelectionEditingBehavior::OnLostMouseCapture(MouseEventArgs& args)
{
    (void) args;
    // If user is editing, we have to commit the current operation and reset our state.
    if ( GetEditingCoordinator().UserIsEditing() )
    {
        ReleaseCapture(false, true);
    }
}


/// <summary>
/// Get the new feedback rectangle based on the location
/// </summary>
/// <param name="newPoint"></param>
Rect SelectionEditingBehavior::ChangeFeedbackRectangle(Point & newPoint)
{
    // First check to make sure that we are not going past the zero point on
    //  the original element in which we hit the resize handle.

    // If moving the left side -- don't go past the right side.
    if ( _hitResult == InkCanvasSelectionHitResult::TopLeft ||
        _hitResult == InkCanvasSelectionHitResult::BottomLeft ||
        _hitResult == InkCanvasSelectionHitResult::Left )
    {
        if ( newPoint.X() > _selectionRect.Right() - MinimumHeightWidthSize )
        {
            newPoint.setX(_selectionRect.Right() - MinimumHeightWidthSize);
        }
    }

    // If moving the right side -- don't go past the left side.
    if ( _hitResult == InkCanvasSelectionHitResult::TopRight ||
        _hitResult == InkCanvasSelectionHitResult::BottomRight ||
        _hitResult == InkCanvasSelectionHitResult::Right )
    {
        if ( newPoint.X() < _selectionRect.Left() + MinimumHeightWidthSize )
        {
            newPoint.setX(_selectionRect.Left() + MinimumHeightWidthSize);
        }
    }

    // If moving the top side -- don't go past the bottom side.
    if ( _hitResult == InkCanvasSelectionHitResult::TopLeft ||
        _hitResult == InkCanvasSelectionHitResult::TopRight ||
        _hitResult == InkCanvasSelectionHitResult::Top )
    {
        if ( newPoint.Y() > _selectionRect.Bottom() - MinimumHeightWidthSize )
        {
            newPoint.setY(_selectionRect.Bottom() - MinimumHeightWidthSize);
        }
    }

    // If moving the bottom side -- don't go past the top side.
    if ( _hitResult == InkCanvasSelectionHitResult::BottomLeft ||
        _hitResult == InkCanvasSelectionHitResult::BottomRight ||
        _hitResult == InkCanvasSelectionHitResult::Bottom )
    {
        if ( newPoint.Y() < _selectionRect.Top() + MinimumHeightWidthSize )
        {
            newPoint.setY(_selectionRect.Top() + MinimumHeightWidthSize);
        }
    }

    // Get the new boundary of the selection
    Rect newRect = CalculateRect(newPoint.X() - _previousLocation.X(), newPoint.Y() - _previousLocation.Y());

    // Depends on the current grab handle, we record the tracking position accordingly.
    if ( _hitResult == InkCanvasSelectionHitResult::BottomRight ||
        _hitResult == InkCanvasSelectionHitResult::BottomLeft ||
        _hitResult == InkCanvasSelectionHitResult::TopRight ||
        _hitResult == InkCanvasSelectionHitResult::TopLeft ||
        _hitResult == InkCanvasSelectionHitResult::Selection )
    {
        _previousLocation.setX(newPoint.X());
        _previousLocation.setY(newPoint.Y());
    }
    else if ( _hitResult == InkCanvasSelectionHitResult::Left ||
        _hitResult == InkCanvasSelectionHitResult::Right )
    {
        _previousLocation.setX(newPoint.X());
    }
    else if ( _hitResult == InkCanvasSelectionHitResult::Top ||
        _hitResult == InkCanvasSelectionHitResult::Bottom )
    {
        _previousLocation.setY(newPoint.Y());
    }

    return newRect;

}

/// <summary>
///     Resize a given element based on the grab handle
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
Rect SelectionEditingBehavior::CalculateRect(double x, double y)
{
    Rect newRect = _previousRect;

    switch ( _hitResult )
    {
        case InkCanvasSelectionHitResult::BottomRight:
            {
                newRect = ExtendSelectionRight(newRect, x);
                newRect = ExtendSelectionBottom(newRect, y);
                break;
            }
        case InkCanvasSelectionHitResult::Bottom:
            {
                newRect = ExtendSelectionBottom(newRect, y);
                break;
            }
        case InkCanvasSelectionHitResult::BottomLeft:
            {
                newRect = ExtendSelectionLeft(newRect, x);
                newRect = ExtendSelectionBottom(newRect, y);
                break;
            }
        case InkCanvasSelectionHitResult::TopRight:
            {
                newRect = ExtendSelectionTop(newRect, y);
                newRect = ExtendSelectionRight(newRect, x);
                break;
            }
        case InkCanvasSelectionHitResult::Top:
            {
                newRect = ExtendSelectionTop(newRect, y);
                break;
            }
        case InkCanvasSelectionHitResult::TopLeft:
            {
                newRect = ExtendSelectionTop(newRect, y);
                newRect = ExtendSelectionLeft(newRect, x);
                break;
            }
        case InkCanvasSelectionHitResult::Left:
            {
                newRect = ExtendSelectionLeft(newRect, x);
                break;
            }
        case InkCanvasSelectionHitResult::Right:
            {
                newRect = ExtendSelectionRight(newRect, x);
                break;
            }
        case InkCanvasSelectionHitResult::Selection:
            {
                // Translate the rectangle
                newRect.Offset(x, y);
                break;
            }
        default:
            {
                //Debug.Assert(false);
                break;
            }
    }

    return newRect;
}

// ExtendSelectionLeft
Rect SelectionEditingBehavior::ExtendSelectionLeft(Rect const & rect, double extendBy)
{
    Rect newRect = rect;
    newRect.SetX(newRect.X() + extendBy);

    return newRect;
}

// ExtendSelectionTop
Rect SelectionEditingBehavior::ExtendSelectionTop(Rect const & rect, double extendBy)
{
    Rect newRect = rect;

    newRect.SetY(newRect.Y() + extendBy);
    return newRect;
}

// ExtendSelectionRight
Rect SelectionEditingBehavior::ExtendSelectionRight(Rect const & rect, double extendBy)
{
    Rect newRect = rect;

    newRect.SetWidth(newRect.Width() + extendBy);

    return newRect;
}

// ExtendSelectionBottom
Rect SelectionEditingBehavior::ExtendSelectionBottom(Rect const & rect, double extendBy)
{
    Rect newRect = rect;

    newRect.SetHeight(newRect.Height() + extendBy);

    return newRect;
}

/// <summary>
/// Capture Mouse
/// </summary>
void SelectionEditingBehavior::InitializeCapture()
{
    //Debug.Assert(GetEditingCoordinator().UserIsEditing() == false, "Unexpect UserIsEditng state." );
    GetEditingCoordinator().SetUserIsEditing(true);
    GetInkCanvas().SelectionAdorner().CaptureMouse();
}

/// <summary>
/// Release Mouse capture
/// </summary>
/// <param name="releaseDevice"></param>
/// <param name="commit"></param>
void SelectionEditingBehavior::ReleaseCapture(bool releaseDevice, bool commit)
{
    if ( GetEditingCoordinator().UserIsEditing() )
    {
        GetEditingCoordinator().SetUserIsEditing(false);
        if ( releaseDevice )
        {
            GetInkCanvas().SelectionAdorner().ReleaseMouseCapture();
        }

        SelfDeactivate();

        // End the rubber band. If the operation is committed, we set the selection to the tracking rectangle.
        // Otherwise, reset the selection to the original bounds.
        GetInkCanvas().GetInkCanvasSelection().EndFeedbackAdorner(commit ? _previousRect : _selectionRect);
    }
}

INKCANVAS_END_NAMESPACE
