#include "Internal/Ink/inkcanvasselection.h"
#include "Windows/Controls/inkcanvas.h"
#include "Windows/Ink/stroke.h"
#include "Internal/doubleutil.h"
#include "Windows/uielement.h"
#include "Windows/Controls/inkevents.h"
#include "Internal/Controls/inkcanvasselectionadorner.h"
#include "Internal/Ink/editingcoordinator.h"
#include "Internal/Controls/inkcanvasinnercanvas.h"
#include "Internal/Controls/inkcanvasfeedbackadorner.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// InkCanvasSelection has an constructor to prevent direct instantiation
/// </summary>
/// <param name="inkCanvas">inkCanvas</param>
InkCanvasSelection::InkCanvasSelection(InkCanvas& inkCanvas)
    : _inkCanvas(inkCanvas)
{
    //validate
    //if (inkCanvas == nullptr)
    //{
    //    throw new ArgumentNullException("inkCanvas");
    //}
    //_inkCanvas = inkCanvas;

    _inkCanvas.FeedbackAdorner().UpdateBounds(Rect::Empty());
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------

//#region Properties

/// <summary>
/// Returns the collection of selected strokes
/// </summary>
SharedPointer<StrokeCollection> InkCanvasSelection::SelectedStrokes()
{
    if ( _selectedStrokes == nullptr )
    {
        _selectedStrokes.reset(new StrokeCollection());
        _areStrokesChanged = true;
    }

    return _selectedStrokes;
}

/// <summary>
/// Returns the collection of selected elements
/// </summary>
List<UIElement*> InkCanvasSelection::SelectedElements()
{
    //if ( _selectedElements == nullptr )
    //{
    //    _selectedElements = new List<UIElement>();
    //}

    return _selectedElements;
}

/// <summary>
/// Indicates whether there is a selection in the current InkCanvas::
/// </summary>
bool InkCanvasSelection::HasSelection()
{
    return SelectedStrokes()->Count() != 0 || SelectedElements().Count() != 0;
}

/// <summary>
/// Returns the selection bounds
/// </summary>
Rect InkCanvasSelection::SelectionBounds()
{
    return Rect::Union(GetStrokesBounds(), GetElementsUnionBounds());
}

//#endregion Properties

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// Start a feedback rubberband.
/// This method is called from SelectionEditingBehavior.OnActivated
/// </summary>
/// <param name="feedbackRect"></param>
/// <param name="activeSelectionHitResult"></param>
void InkCanvasSelection::StartFeedbackAdorner(Rect const & feedbackRect, InkCanvasSelectionHitResult activeSelectionHitResult)
{
    Debug::Assert( _inkCanvas.GetEditingCoordinator().UserIsEditing() == true );
    Debug::Assert(activeSelectionHitResult != InkCanvasSelectionHitResult::None, "activeSelectionHitResult cannot be InkCanvasSelectionHitResult::None.");

    _activeSelectionHitResult = {activeSelectionHitResult, true};

    AdornerLayer* adornerLayer = AdornerLayer::GetAdornerLayer(&_inkCanvas.InnerCanvas());

    // Until the next start, we are going to use this adorner regardless user will change feedback adorner or not during the editing.
    InkCanvasFeedbackAdorner& feedbackAdorner = _inkCanvas.FeedbackAdorner();

    // The feedback adorner shouldn't have been connected to the adorner layer yet.
    Debug::Assert(feedbackAdorner.VisualParent() == nullptr,
        "feedbackAdorner shouldn't be added to tree.");

    // Now, attach the feedback adorner to the adorner layer. Then update its bounds
    adornerLayer->Add(&feedbackAdorner);
    feedbackAdorner.UpdateBounds(feedbackRect);
}

/// <summary>
/// This method updates the feedback rubberband.
/// This method is called from SelectionEditingBehavior.OnMouseMove.
/// </summary>
/// <param name="feedbackRect"></param>
void InkCanvasSelection::UpdateFeedbackAdorner(Rect const & feedbackRect)
{
    Debug::Assert(_inkCanvas.FeedbackAdorner().VisualParent()
        == AdornerLayer::GetAdornerLayer(&_inkCanvas.InnerCanvas()),
        "feedbackAdorner should have been added to tree.");

    // Update the feedback bounds
    _inkCanvas.FeedbackAdorner().UpdateBounds(feedbackRect);
}

/// <summary>
/// Ends a feedback rubberband
/// This method is called from SelectionEditingBehavior.OnMoveUp
/// </summary>
/// <param name="finalRectangle"></param>
void InkCanvasSelection::EndFeedbackAdorner(Rect const & finalRectangle)
{
    AdornerLayer* adornerLayer = AdornerLayer::GetAdornerLayer(&_inkCanvas.InnerCanvas());
    InkCanvasFeedbackAdorner& feedbackAdorner = _inkCanvas.FeedbackAdorner();

    Debug::Assert(feedbackAdorner.VisualParent() == adornerLayer,
        "feedbackAdorner should have been added to tree.");

    // Reset the feedback bounds and detach it from the adorner layer.
    feedbackAdorner.UpdateBounds(Rect::Empty());
    adornerLayer->Remove(&feedbackAdorner);

    // Commit the new rectange of the selection.
    CommitChanges(finalRectangle, true);

    _activeSelectionHitResult.second = false;

}

/// <summary>
/// Set the new selection to this helper class.
/// </summary>
/// <param name="strokes"></param>
/// <param name="elements"></param>
/// <param name="raiseSelectionChanged"></param>
void InkCanvasSelection::Select(SharedPointer<StrokeCollection> strokes, List<UIElement*> const & elements, bool raiseSelectionChanged)
{
    // Can't be nullptr.
    //Debug::Assert(strokes != nullptr && elements != nullptr);

    //
    // check to see if we're different.  If not, there is no need to raise the changed event
    //
    bool strokesAreDifferent;
    bool elementsAreDifferent;
    int count = 0;
    SelectionIsDifferentThanCurrent(strokes, strokesAreDifferent, elements, elementsAreDifferent);

    // Besides the differences of both the stroks and the elements, there is a chance that the selected strokes and elements
    // have been removed from the InkCanvas:: Meanwhile, we may still hold the reference of _inkCanvasSelection which is empty.
    // So, we should make sure to remove _inkCanvasSelection in this case.
    if ( strokesAreDifferent || elementsAreDifferent )
    {
        if ( strokesAreDifferent && SelectedStrokes()->Count() != 0 )
        {
            // PERF-2006/05/02-WAYNEZEN,
            // Unsubscribe the event fisrt so that reseting IsSelected won't call into our handler.
            //
            // quit listening to changes
            //
            QuitListeningToStrokeChanges();

            count = SelectedStrokes()->Count();
            for ( int i = 0; i < count; i++ )
            {
                (*SelectedStrokes())[i]->SetIsSelected(false);
            }
        }

        _selectedStrokes = strokes;
        _areStrokesChanged = true;
        _selectedElements = elements;


        // PERF-2006/05/02-WAYNEZEN,
        // Update IsSelected property before adding the event handlers.
        // NTRAID#WINDOWS-1191424-2005/07/07-WAYNEZNE,
        // We don't have to render hollow strokes if the current mode isn't Select mode
        if ( _inkCanvas.ActiveEditingMode() == InkCanvasEditingMode::Select )
        {
            // NTRAID#T2-26511-2004/10/18-waynezen,
            // Updating the visuals of the hitted strokes.
            count = strokes->Count();
            for ( int i = 0; i < count; i++ )
            {
                (*strokes)[i]->SetIsSelected(true);
            }
        }

        // Add a LayoutUpdated handler if it's necessary.
        UpdateCanvasLayoutUpdatedHandler();

        // Update our selection adorner manually
        UpdateSelectionAdorner();

        ListenToStrokeChanges();

        //
        // And finally... raise the SelectionChanged event
        //
        if ( raiseSelectionChanged )
        {
            _inkCanvas.RaiseSelectionChanged(EventArgs::Empty);
        }
    }
}

/// <summary>
/// Our method to commit the current editing.
/// Called by EndFeedbackAdorner or InkCanvas::PasteFromDataObject
/// </summary>
void InkCanvasSelection::CommitChanges(Rect const & finalRectangle, bool raiseEvent)
{

    // The follow moving or resizing raises SelectionMove or SelectionResize event
    // The out-side code could throw exception in the their handlers. We use try/finally block to protect our status.
    Rect selectionBounds = SelectionBounds();

    // NTRAID:WINDOWS#1464216-2006/01/27-WAYNEZEN
    // If the selection is cleared programmatically somehow during user interaction, we will get an empty source rect.
    // We should check if the source still valid here. If not, no selection needs to be changed.
    if ( selectionBounds.IsEmpty() )
    {
        return;
    }

    FinallyHelper final([this](){
        ListenToStrokeChanges( );
    });

    //try
    {
        QuitListeningToStrokeChanges( );

        if ( raiseEvent )
        {
            //
            // see if we resized
            //
            if ( !DoubleUtil::AreClose(finalRectangle.Height(), selectionBounds.Height())
                || !DoubleUtil::AreClose(finalRectangle.Width(), selectionBounds.Width()) )
            {
                //
                // we resized
                //
                CommitResizeChange(finalRectangle);
            }
            else if ( !DoubleUtil::AreClose(finalRectangle.Top(), selectionBounds.Top())
                || !DoubleUtil::AreClose(finalRectangle.Left(), selectionBounds.Left()) )
            {
                //
                // we moved
                //
                CommitMoveChange(finalRectangle);
            }
        }
        else
        {
            //
            // Just perform the move without raising any events.
            //
            MoveSelection(selectionBounds, finalRectangle);
        }
    }
    //finally
    //{
    //    ListenToStrokeChanges( );
    //}
}

/// <summary>
/// Try to remove an element from the selected elements list.
/// Called by:
///             OnCanvasLayoutUpdated
/// </summary>
/// <param name="removedElement"></param>
void InkCanvasSelection::RemoveElement(UIElement* removedElement)
{
    // No selected element. Bail out
    if (_selectedElements.Count() == 0 )
    {
        return;
    }

    if ( _selectedElements.Remove(removedElement) )
    {
        // The element existed and was removed successfully.
        // Now if there is no selected element, we should remove our LayoutUpdated handler.
        if ( _selectedElements.Count() == 0 )
        {
            UpdateCanvasLayoutUpdatedHandler();
            UpdateSelectionAdorner();
        }
    }
}

/// <summary>
/// UpdateElementBounds:
///     Called by InkCanvasSelection.MoveSelection
/// </summary>
/// <param name="element"></param>
/// <param name="transform"></param>
void InkCanvasSelection::UpdateElementBounds(UIElement* element, Matrix const & transform)
{
    UpdateElementBounds(element, element, transform);
}

/// <summary>
/// UpdateElementBounds:
///     Called by   InkCanvasSelection.UpdateElementBounds
///                 ClipboardProcessor.CopySelectionInXAML
/// </summary>
/// <param name="originalElement"></param>
/// <param name="updatedElement"></param>
/// <param name="transform"></param>
void InkCanvasSelection::UpdateElementBounds(UIElement* originalElement, UIElement* updatedElement, Matrix const & transform)
{
    //if ( originalElement->DependencyObjectType.Id == updatedElement.DependencyObjectType.Id )
    {
        // Get the transform from element to Canvas
        Matrix elementToCanvas = originalElement->TransformToAncestor(&_inkCanvas.InnerCanvas());

        //cast to a FrameworkElement, nothing inherits from UIElement besides it right now
        FrameworkElement* frameworkElement = qobject_cast<FrameworkElement*>(originalElement);
        Size size;
        Rect thickness;
        if ( frameworkElement == nullptr )
        {
            // Get the element's render size.
            size = originalElement->RenderSize();
        }
        else
        {
            size = Size(frameworkElement->ActualWidth(), frameworkElement->ActualHeight());
            thickness = frameworkElement->Margin();
        }

        Rect elementBounds(0, 0, size.Width(), size.Height());   // Rect in element space
        elementBounds = elementToCanvas.Transform(elementBounds); // Rect in Canvas space

        // Now apply the matrix to the element bounds
        Rect newBounds = transform.Transform(elementBounds);

        if ( !DoubleUtil::AreClose(elementBounds.Width(), newBounds.Width()) )
        {
            if ( frameworkElement == nullptr )
            {
                Size newSize = originalElement->RenderSize();
                newSize.setWidth(newBounds.Width());
                updatedElement->SetRenderSize(newSize);
            }
            else
            {
                ((FrameworkElement*)updatedElement)->SetWidth(newBounds.Width());
            }
        }

        if ( !DoubleUtil::AreClose(elementBounds.Height(), newBounds.Height()) )
        {
            if ( frameworkElement == nullptr )
            {
                Size newSize = originalElement->RenderSize();
                newSize.setHeight(newBounds.Height());
                updatedElement->RenderSize() = newSize;
            }
            else
            {
                ( (FrameworkElement*)updatedElement )->SetHeight(newBounds.Height());
            }
        }

        double left = InkCanvas::GetLeft(originalElement);
        double top = InkCanvas::GetTop(originalElement);
        double right = InkCanvas::GetRight(originalElement);
        double bottom = InkCanvas::GetBottom(originalElement);

        Point originalPosition; // Default as (0, 0)
        if ( !Double::IsNaN(left) )
        {
            originalPosition.setX(left);
        }
        else if ( !Double::IsNaN(right) )
        {
            originalPosition.setX(right);
        }

        if ( !Double::IsNaN(top) )
        {
            originalPosition.setY(top);
        }
        else if ( !Double::IsNaN(bottom) )
        {
            originalPosition.setY( bottom);
        }

        Point newPosition = originalPosition * transform;

        if ( !Double::IsNaN(left) )
        {
            InkCanvas::SetLeft(updatedElement, newPosition.X() - thickness.Left());           // Left wasn't auto
        }
        else if ( !Double::IsNaN(right) )
        {
            // NOTICE-2005/05/05-WAYNEZEN
            // Canvas.RightProperty means the distance between element right side and its parent Canvas
            // right side. The same definition is applied to Canvas.BottomProperty
            InkCanvas::SetRight(updatedElement, ( right - ( newPosition.X() - originalPosition.X() ) )); // Right wasn't not auto
        }
        else
        {
            InkCanvas::SetLeft(updatedElement, newPosition.X() - thickness.Left());           // Both Left and Right were aut. Modify Left by default.
        }

        if ( !Double::IsNaN(top) )
        {
            InkCanvas::SetTop(updatedElement, newPosition.Y() - thickness.Top());           // Top wasn't auto
        }
        else if ( !Double::IsNaN(bottom) )
        {
            InkCanvas::SetBottom(updatedElement, ( bottom - ( newPosition.Y() - originalPosition.Y() ) ));      // Bottom wasn't not auto
        }
        else
        {
            InkCanvas::SetTop(updatedElement, newPosition.Y() - thickness.Top());           // Both Top and Bottom were aut. Modify Left by default.
        }
    }
    //else
    //{
    //    Debug::Assert(false, "The updatedElement has to be the same type as the originalElement.");
    //}
}

void InkCanvasSelection::TransformStrokes(SharedPointer<StrokeCollection> strokes, Matrix const & matrix)
{
    strokes->Transform(matrix, false /*Don't apply the transform to StylusTip*/);
}

InkCanvasSelectionHitResult InkCanvasSelection::HitTestSelection(Point const & pointOnInkCanvas)
{
    // If Selection is moving or resizing now, we should returns the active hit result.
    if ( _activeSelectionHitResult.second )
    {
        return _activeSelectionHitResult.first;
    }

    // No selection at all. Just return None.
    if ( !HasSelection() )
    {
        return InkCanvasSelectionHitResult::None;
    }

    // Get the transform from InkCanvas to SelectionAdorner
    GeneralTransform inkCanvasToSelectionAdorner = _inkCanvas.TransformToDescendant(&_inkCanvas.SelectionAdorner());
    Point pointOnSelectionAdorner = inkCanvasToSelectionAdorner.Transform(pointOnInkCanvas);

    InkCanvasSelectionHitResult hitResult = _inkCanvas.SelectionAdorner().SelectionHandleHitTest(pointOnSelectionAdorner);

    // If the hit test returns Selection and there is one and only one element is selected.
    // We have to check whether we hit on inside the element.
    if ( hitResult == InkCanvasSelectionHitResult::Selection
        && SelectedElements().Count() == 1
        && SelectedStrokes()->Count() == 0 )
    {
        GeneralTransform transformToInnerCanvas = _inkCanvas.TransformToDescendant(&_inkCanvas.InnerCanvas());
        Point pointOnInnerCanvas = transformToInnerCanvas.Transform(pointOnInkCanvas);

        // Try to find out whether we hit the single selement. If so, just return None.
        if ( HasHitSingleSelectedElement(pointOnInnerCanvas) )
        {
            hitResult = InkCanvasSelectionHitResult::None;
        }
    }

    return hitResult;
}

/// <summary>
/// helper to used to determine if the current selection
/// is different than the selection passed
/// </summary>
/// <param name="strokes">strokes</param>
/// <param name="strokesAreDifferent">strokesAreDifferent</param>
/// <param name="elements">elements</param>
/// <param name="elementsAreDifferent">elementsAreDifferent</param>
void InkCanvasSelection::SelectionIsDifferentThanCurrent(SharedPointer<StrokeCollection> strokes,
                                            bool& strokesAreDifferent,
                                            List<UIElement*> const & elements,
                                            bool& elementsAreDifferent)
{
    strokesAreDifferent = false;
    elementsAreDifferent = false;

    if ( SelectedStrokes()->Count() == 0 )
    {
        if ( strokes->Count() > 0 )
        {
            strokesAreDifferent = true;
        }
    }
    else if ( !InkCanvasSelection::StrokesAreEqual(SelectedStrokes(), strokes) )
    {
        strokesAreDifferent = true;
    }

    if ( SelectedElements().Count() == 0 )
    {
        if ( elements.Count() > 0 )
        {
            elementsAreDifferent = true;
        }
    }
    else if ( !InkCanvasSelection::FrameworkElementArraysAreEqual(elements, SelectedElements()) )
    {
        elementsAreDifferent = true;
    }
}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// HasHitSingleSelectedElement
/// This method hits test against the current selection. If the point is inside the single selected element,
/// it will return true. Otherwise, return false.
/// </summary>
/// <param name="pointOnInnerCanvas"></param>
/// <returns></returns>
bool InkCanvasSelection::HasHitSingleSelectedElement(Point const & pointOnInnerCanvas)
{
    bool hasHit = false;

    if ( SelectedElements().Count() == 1 )
    {
        List<Rect> list = SelectedElementsBoundsEnumerator();
        auto enumerator = list.begin();
        if ( enumerator != list.end())
        {
            Rect elementRect = *enumerator;
            hasHit = elementRect.Contains(pointOnInnerCanvas);
        }
        else
        {
            Debug::Assert(false, "An unexpected single selected Element");
        }
    }

    return hasHit;
}

/// <summary>
/// helper to stop listening to stroke changes
/// </summary>
void InkCanvasSelection::QuitListeningToStrokeChanges()
{
    if ( _inkCanvas.Strokes() != nullptr )
    {
        QObject::disconnect(_inkCanvas.Strokes().get(), &StrokeCollection::StrokesChanged,
                            this, &InkCanvasSelection::OnStrokeCollectionChanged);
        //_inkCanvas.Strokes.StrokesChanged -= new StrokeCollectionChangedEventHandler(this.OnStrokeCollectionChanged);
    }

    for ( SharedPointer<Stroke> s : *SelectedStrokes() )
    {
        QObject::disconnect(s.get(), &Stroke::Invalidated,
                            this, &InkCanvasSelection::OnStrokeInvalidated);
        //s.Invalidated -= new EventHandler(this.OnStrokeInvalidated);
    }
}

/// <summary>
/// helper to listen to stroke changes
/// </summary>
void InkCanvasSelection::ListenToStrokeChanges()
{
    if ( _inkCanvas.Strokes() != nullptr )
    {
        QObject::connect(_inkCanvas.Strokes().get(), &StrokeCollection::StrokesChanged,
                            this, &InkCanvasSelection::OnStrokeCollectionChanged);
        //_inkCanvas.Strokes.StrokesChanged += new StrokeCollectionChangedEventHandler(this.OnStrokeCollectionChanged);
    }

    for ( SharedPointer<Stroke> s : *SelectedStrokes() )
    {
        QObject::connect(s.get(), &Stroke::Invalidated,
                            this, &InkCanvasSelection::OnStrokeInvalidated);
        //s.Invalidated += new EventHandler(this.OnStrokeInvalidated);
    }
}

/// <summary>
/// Helper method that take a finalRectangle and raised the appropriate
/// events on the InkCanvas::  Handles cancellation
/// </summary>
void InkCanvasSelection::CommitMoveChange(Rect finalRectangle)
{
    Rect selectionBounds = SelectionBounds();
    //
    //*** MOVED ***
    //
    InkCanvasSelectionEditingEventArgs args(selectionBounds, finalRectangle);

    _inkCanvas.RaiseSelectionMoving(args);

    if ( !args.Cancel() )
    {
        if ( finalRectangle != args.NewRectangle() )
        {
            finalRectangle = args.NewRectangle();
        }

        //
        // perform the move
        //
        MoveSelection(selectionBounds, finalRectangle);

        //
        // raise the 'changed' event
        //
        _inkCanvas.RaiseSelectionMoved(EventArgs::Empty);
    }
}

/// <summary>
/// Helper method that take a finalRectangle and raised the appropriate
/// events on the InkCanvas::  Handles cancellation
/// </summary>
void InkCanvasSelection::CommitResizeChange(Rect finalRectangle)
{
    Rect selectionBounds = SelectionBounds();
    //
    // *** RESIZED ***
    //
    InkCanvasSelectionEditingEventArgs args(selectionBounds, finalRectangle);

    _inkCanvas.RaiseSelectionResizing(args);

    if ( !args.Cancel() )
    {
        if ( finalRectangle != args.NewRectangle() )
        {
            finalRectangle = args.NewRectangle();
        }

        //
        // perform the move
        //
        MoveSelection(selectionBounds, finalRectangle);

        //
        // raise the 'changed' event
        //
        _inkCanvas.RaiseSelectionResized(EventArgs::Empty);
    }
}

/// <summary>
/// helper that moves all selected elements from the previous location
/// to the new one
/// </summary>
/// <param name="previousRect"></param>
/// <param name="newRect"></param>
void InkCanvasSelection::MoveSelection(Rect const & previousRect, Rect const & newRect)
{
    //
    // compute our transform, but first remove our artificial border
    //
    Matrix matrix = InkCanvasSelection::MapRectToRect(newRect, previousRect);

    //
    // transform the elements
    //
    int count = SelectedElements().Count();
    List<UIElement*> elements = SelectedElements();
    for ( int i = 0; i < count ; i++ )
    {
        UpdateElementBounds(elements[i], matrix);
    }

    //
    // transform the strokes
    //
    if ( SelectedStrokes()->Count() > 0 )
    {
        TransformStrokes(SelectedStrokes(), matrix);
        // Flag the change
        _areStrokesChanged = true;
    }

    if ( SelectedElements().Count() == 0 )
    {
        // If there is no element in the current selection, the inner canvas won't have layout changes.
        // So there is no LayoutUpdated event. We have to manually update the selection bounds.
        UpdateSelectionAdorner();
    }

    // NTRAID:WINDOWSOS#1565058-2006/03/20-WAYNEZEN,
    // Always bring the new selection rectangle into the current view when the InkCanvas is hosted inside a ScrollViewer.
    _inkCanvas.BringIntoView(newRect);
}

/// <summary>
/// A handler which receives LayoutUpdated from the layout manager. Since the selected elements can be animated,
/// moved, resized or even removed programmatically from the InkCanvas, we have to monitor
/// the any layout changes caused by those actions.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void InkCanvasSelection::OnCanvasLayoutUpdated(EventArgs&)
{
    Debug::Assert( SelectedElements().Count() != 0,
        "The handler only can be hooked up when there are elements being selected" );

    // Update the selection adorner when receive the layout changed
    UpdateSelectionAdorner();
}

/// <summary>
/// Our own listener for packet changes
/// </summary>
void InkCanvasSelection::OnStrokeInvalidated(EventArgs&)
{
    StrokeCollectionChangedEventArgs args(
                SharedPointer<StrokeCollection>(new StrokeCollection()),
                SharedPointer<StrokeCollection>(new StrokeCollection()));
    OnStrokeCollectionChanged(args);
}

/// <summary>
/// Our own listener for strokes changed.
/// This is used so that if someone deletes or modifies a stroke
/// we are currently displaying for selection, we can update our size
/// </summary>
void InkCanvasSelection::OnStrokeCollectionChanged(StrokeCollectionChangedEventArgs& e)
{
    // If the strokes only get added to the InkCanvas, we don't have to update our selected strokes.
    if ( e.Added()->Count() != 0 && e.Removed()->Count() == 0 )
    {
        return;
    }

    for (SharedPointer<Stroke> s : *e.Removed())
    {
        if ( SelectedStrokes()->Contains(s) )
        {
            QObject::disconnect(s.get(), &Stroke::Invalidated,
                                this, &InkCanvasSelection::OnStrokeInvalidated);
            //s->Invalidated -= new EventHandler(this.OnStrokeInvalidated);
            s->SetIsSelected(false);

            // Now remove the stroke from our collection.
            SelectedStrokes()->Remove(s);
        }
    }

    // Mark the strokes change
    _areStrokesChanged = true;
    UpdateSelectionAdorner();
}

/// <summary>
/// Get the cached bounding boxes to be updated.
/// </summary>
Rect InkCanvasSelection::GetStrokesBounds()
{
    // Update the strokes bounds if they are changed.
    if ( _areStrokesChanged )
    {
        _cachedStrokesBounds = SelectedStrokes()->Count() != 0 ?
                                SelectedStrokes()->GetBounds( ) : Rect::Empty();
        _areStrokesChanged = false;
    }

    return _cachedStrokesBounds;
}

/// <summary>
/// Get the bounding boxes of the selected elements.
/// </summary>
/// <returns></returns>
List<Rect> InkCanvasSelection::GetElementsBounds()
{
    List<Rect> rects;

    if ( SelectedElements().Count() != 0 )
    {
        // The SelectedElementsBounds property will ensure the size is got after rendering's done.
        for ( Rect elementRect : SelectedElementsBoundsEnumerator() )
        {
            rects.Add(elementRect);
        }
    }

    return rects;
}

/// <summary>
/// Get the union box of the selected elements.
/// </summary>
/// <returns></returns>
Rect InkCanvasSelection::GetElementsUnionBounds()
{
    if ( SelectedElements().Count() == 0 )
    {
        return Rect::Empty();
    }

    Rect elementsBounds;

    // The SelectedElementsBounds property will ensure the size is got after rendering's done.
    for ( Rect elementRect : SelectedElementsBoundsEnumerator() )
    {
        elementsBounds.Union(elementRect);
    }

    return elementsBounds;
}

/// <summary>
/// Update the selection adorners state.
/// The method is called by:
///         MoveSelection
///         OnCanvasLayoutUpdated
///         OnStrokeCollectionChanged
///         RemoveElement
///         Select
/// </summary>
void InkCanvasSelection::UpdateSelectionAdorner()
{
    // The Selection Adorner will be visible all the time unless the ActiveEditingMode is None.
    if ( _inkCanvas.ActiveEditingMode() != InkCanvasEditingMode::None )
    {
        //Debug::Assert(_inkCanvas.SelectionAdorner().Visibility() == Visibility::Visible,
        //    "SelectionAdorner should be always visible except under the None ActiveEditingMode");

        _inkCanvas.SelectionAdorner().UpdateSelectionWireFrame(GetStrokesBounds(), GetElementsBounds());
    }
    else
    {
        //Debug::Assert(_inkCanvas.SelectionAdorner().Visibility() == Visibility::Collapsed,
        //    "SelectionAdorner should be collapsed except if the ActiveEditingMode is None");
    }

}

/// <summary>
/// Ensure the rendering is valid.
/// Called by:
///         SelectedElementsBounds
/// </summary>
void InkCanvasSelection::EnusreElementsBounds()
{
    InkCanvasInnerCanvas& innerCanvas = _inkCanvas.InnerCanvas();

    // Make sure the arrange is valid. If not, we invoke UpdateLayout now.
    if ( !innerCanvas.IsMeasureValid() || !innerCanvas.IsArrangeValid() )
    {
        innerCanvas.UpdateLayout();
    }
}

/// <summary>
/// Utility for computing a transformation that maps one rect to another
/// </summary>
/// <param name="target">Destination Rectangle</param>
/// <param name="source">Source Rectangle</param>
/// <returns>Transform that maps source rectangle to destination rectangle</returns>
Matrix InkCanvasSelection::MapRectToRect(Rect const & target, Rect const & source)
{
    if(source.IsEmpty())
        throw std::runtime_error("source");
    /*
    In the horizontal direction:

            M11*source.left  + Dx = target.left
            M11*source.right + Dx = target.right

    Subtracting the equations yields:

            M11*(source.right - source.left) = target.right - target.left
    hence:
            M11 = (target.right - target.left) / (source.right - source.left)

    Having computed M11, solve the first equation for Dx:

            Dx = target.left - M11*source.left
    */
    double m11 = target.Width() / source.Width();
    double dx = target.Left() - m11 * source.Left();

    // Now do the same thing in the vertical direction
    double m22 = target.Height() / source.Height();
    double dy = target.Top() - m22 * source.Top();
    /*
    We are looking for the transformation that takes one upright rectangle to
    another.  This involves neither rotation nor shear, so:
    */
    return Matrix(m11, 0, 0, m22, dx, dy);
}

/// <summary>
/// Adds/Removes the LayoutUpdated handler according to whether there is selected elements or not.
/// </summary>
void InkCanvasSelection::UpdateCanvasLayoutUpdatedHandler()
{
    // If there are selected elements, we should create a LayoutUpdated handler in order to monitor their arrange changes.
    // Otherwise, we don't have to listen the LayoutUpdated if there is no selected element at all.
    if ( SelectedElements().Count() != 0 )
    {
        // Make sure we hook up the event handler.
        if ( _layoutUpdatedHandler == nullptr )
        {
            _layoutUpdatedHandler = EventHandlerT<InkCanvasSelection, EventArgs, &InkCanvasSelection::OnCanvasLayoutUpdated>(this);
            QObject::connect(&_inkCanvas.InnerCanvas(), &UIElement::LayoutUpdated,
                             this, &InkCanvasSelection::OnCanvasLayoutUpdated);
        }
    }
    else
    {
        // Remove the handler if there is one.
        if ( _layoutUpdatedHandler != nullptr )
        {
            QObject::disconnect(&_inkCanvas.InnerCanvas(), &UIElement::LayoutUpdated,
                             this, &InkCanvasSelection::OnCanvasLayoutUpdated);
            _layoutUpdatedHandler = nullptr;
        }
    }
}

/// <summary>
/// helper method used to determine if two stroke collection either
/// 1) are both nullptr
/// or
/// 2) both refer to the identical set of strokes
/// </summary>
bool InkCanvasSelection::StrokesAreEqual(SharedPointer<StrokeCollection> strokes1, SharedPointer<StrokeCollection> strokes2)
{
    if ( strokes1 == nullptr && strokes2 == nullptr )
    {
        return true;
    }
    //
    // we know that one of the stroke collections is
    // not nullptr.  If the other one is, they're not
    // equal
    //
    if ( strokes1 == nullptr || strokes2 == nullptr )
    {

        return false;
    }
    if ( strokes1->Count() != strokes2->Count() )
    {
        return false;
    }
    for ( SharedPointer<Stroke> s : *strokes1 )
    {
        if ( !strokes2->Contains(s) )
        {
            return false;
        }
    }
    return true;
}

/// <summary>
/// helper method used to determine if two collection either
/// 1) are both nullptr
/// or
/// 2) both refer to the identical set of elements
/// </summary>
bool InkCanvasSelection::FrameworkElementArraysAreEqual(List<UIElement*> const & elements1, List<UIElement*> const & elements2)
{
    //if ( elements1 == nullptr && elements2 == nullptr )
    //{
    //    return true;
    //}
    //
    // we know that one of the stroke collections is
    // not nullptr.  If the other one is, they're not
    // equal
    //
    //if ( elements1 == nullptr || elements2 == nullptr )
    //{
    //    return false;
    //}
    if ( elements1.Count() != elements2.Count() )
    {
        return false;
    }
    for ( UIElement* e : elements1 )
    {
        if ( !elements2.Contains(e) )
        {
            return false;
        }
    }
    return true;
}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------

//#region Properties

/// <summary>
/// Iterates the bounding boxes of the selected elements relevent to their parent (InnerCanvas)
/// </summary>
List<Rect> InkCanvasSelection::SelectedElementsBoundsEnumerator()
{
    // Ensure the elements have been rendered completely.
    EnusreElementsBounds();
    List<Rect> list;
    InkCanvasInnerCanvas &innerCanvas = _inkCanvas.InnerCanvas();
    for ( UIElement* element : SelectedElements() )
    {
        // Get the transform from element to Canvas
        GeneralTransform elementToCanvas = element->TransformToAncestor(&innerCanvas);

        // Get the element's render size.
        Size size = element->RenderSize();

        Rect rect(0, 0, size.Width(), size.Height());   // Rect in element space
        rect = elementToCanvas.Transform(rect);          // Rect in Canvas space

        list.Add(rect);
    }
    return list;
}

INKCANVAS_END_NAMESPACE
