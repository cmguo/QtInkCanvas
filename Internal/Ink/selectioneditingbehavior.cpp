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
    QPointF curPoint = args.GetPosition(&GetInkCanvas().SelectionAdorner());

    // Check if we have a mouse movement at all.
    if ( !DoubleUtil::AreClose(curPoint.x(), _previousLocation.x())
        || !DoubleUtil::AreClose(curPoint.y(), _previousLocation.y()) )
    {
        // We won't start the move until we really see a movement.
        if ( !_actionStarted )
        {
            _actionStarted = true;
        }

        // Get the new rectangle
        QRectF newRect = ChangeFeedbackRectangle(curPoint);

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
        QPointF point = args.GetPosition(&GetInkCanvas().SelectionAdorner());
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
QRectF SelectionEditingBehavior::ChangeFeedbackRectangle(QPointF & newPoint)
{
    // First check to make sure that we are not going past the zero point on
    //  the original element in which we hit the resize handle.

    // If moving the left side -- don't go past the right side.
    if ( _hitResult == InkCanvasSelectionHitResult::TopLeft ||
        _hitResult == InkCanvasSelectionHitResult::BottomLeft ||
        _hitResult == InkCanvasSelectionHitResult::Left )
    {
        if ( newPoint.x() > _selectionRect.right() - MinimumHeightWidthSize )
        {
            newPoint.setX(_selectionRect.right() - MinimumHeightWidthSize);
        }
    }

    // If moving the right side -- don't go past the left side.
    if ( _hitResult == InkCanvasSelectionHitResult::TopRight ||
        _hitResult == InkCanvasSelectionHitResult::BottomRight ||
        _hitResult == InkCanvasSelectionHitResult::Right )
    {
        if ( newPoint.x() < _selectionRect.left() + MinimumHeightWidthSize )
        {
            newPoint.setX(_selectionRect.left() + MinimumHeightWidthSize);
        }
    }

    // If moving the top side -- don't go past the bottom side.
    if ( _hitResult == InkCanvasSelectionHitResult::TopLeft ||
        _hitResult == InkCanvasSelectionHitResult::TopRight ||
        _hitResult == InkCanvasSelectionHitResult::Top )
    {
        if ( newPoint.y() > _selectionRect.bottom() - MinimumHeightWidthSize )
        {
            newPoint.setY(_selectionRect.bottom() - MinimumHeightWidthSize);
        }
    }

    // If moving the bottom side -- don't go past the top side.
    if ( _hitResult == InkCanvasSelectionHitResult::BottomLeft ||
        _hitResult == InkCanvasSelectionHitResult::BottomRight ||
        _hitResult == InkCanvasSelectionHitResult::Bottom )
    {
        if ( newPoint.y() < _selectionRect.top() + MinimumHeightWidthSize )
        {
            newPoint.setY(_selectionRect.top() + MinimumHeightWidthSize);
        }
    }

    // Get the new boundary of the selection
    QRectF newRect = CalculateRect(newPoint.x() - _previousLocation.x(), newPoint.y() - _previousLocation.y());

    // Depends on the current grab handle, we record the tracking position accordingly.
    if ( _hitResult == InkCanvasSelectionHitResult::BottomRight ||
        _hitResult == InkCanvasSelectionHitResult::BottomLeft ||
        _hitResult == InkCanvasSelectionHitResult::TopRight ||
        _hitResult == InkCanvasSelectionHitResult::TopLeft ||
        _hitResult == InkCanvasSelectionHitResult::Selection )
    {
        _previousLocation.setX(newPoint.x());
        _previousLocation.setY(newPoint.y());
    }
    else if ( _hitResult == InkCanvasSelectionHitResult::Left ||
        _hitResult == InkCanvasSelectionHitResult::Right )
    {
        _previousLocation.setX(newPoint.x());
    }
    else if ( _hitResult == InkCanvasSelectionHitResult::Top ||
        _hitResult == InkCanvasSelectionHitResult::Bottom )
    {
        _previousLocation.setY(newPoint.y());
    }

    return newRect;

}

/// <summary>
///     Resize a given element based on the grab handle
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
QRectF SelectionEditingBehavior::CalculateRect(double x, double y)
{
    QRectF newRect = _previousRect;

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
                newRect.adjust(x, y, x, y);
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
QRectF SelectionEditingBehavior::ExtendSelectionLeft(QRectF const & rect, double extendBy)
{
    QRectF newRect = rect;
    newRect.adjust(extendBy, 0, 0, 0);

    return newRect;
}

// ExtendSelectionTop
QRectF SelectionEditingBehavior::ExtendSelectionTop(QRectF const & rect, double extendBy)
{
    QRectF newRect = rect;

    newRect.adjust(0, extendBy, 0, 0);
    return newRect;
}

// ExtendSelectionRight
QRectF SelectionEditingBehavior::ExtendSelectionRight(QRectF const & rect, double extendBy)
{
    QRectF newRect = rect;

    newRect.adjust(0, 0, extendBy, 0);

    return newRect;
}

// ExtendSelectionBottom
QRectF SelectionEditingBehavior::ExtendSelectionBottom(QRectF const & rect, double extendBy)
{
    QRectF newRect = rect;

    newRect.adjust(0, 0, 0, extendBy);

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
