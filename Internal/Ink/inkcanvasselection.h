#ifndef INKCANVASSELECTION_H
#define INKCANVASSELECTION_H

#include "InkCanvas_global.h"
#include "Windows/Controls/inkcanvasselectionhitresult.h"
#include "eventargs.h"

#include <QSharedPointer>
#include <QRectF>

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class InkCanvas;
class StrokeCollection;
class UIElement;
class StrokeCollectionChangedEventArgs;

/// <summary>
/// InkCanvasSelection
/// </summary>
class InkCanvasSelection : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors

    /// <summary>
    /// InkCanvasSelection has an constructor to prevent direct instantiation
    /// </summary>
    /// <param name="inkCanvas">inkCanvas</param>
    InkCanvasSelection(InkCanvas& inkCanvas);

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
    QSharedPointer<StrokeCollection> SelectedStrokes();

    /// <summary>
    /// Returns the collection of selected elements
    /// </summary>
    QList<UIElement*> SelectedElements();

    /// <summary>
    /// Indicates whether there is a selection in the current InkCanvas.
    /// </summary>
    bool HasSelection();

    /// <summary>
    /// Returns the selection bounds
    /// </summary>
    QRectF SelectionBounds();

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
    void StartFeedbackAdorner(QRectF const & feedbackRect, InkCanvasSelectionHitResult activeSelectionHitResult);

    /// <summary>
    /// This method updates the feedback rubberband.
    /// This method is called from SelectionEditingBehavior.OnMouseMove.
    /// </summary>
    /// <param name="feedbackRect"></param>
    void UpdateFeedbackAdorner(QRectF const & feedbackRect);

    /// <summary>
    /// Ends a feedback rubberband
    /// This method is called from SelectionEditingBehavior.OnMoveUp
    /// </summary>
    /// <param name="finalRectangle"></param>
    void EndFeedbackAdorner(QRectF const & finalRectangle);

    /// <summary>
    /// Set the new selection to this helper class.
    /// </summary>
    /// <param name="strokes"></param>
    /// <param name="elements"></param>
    /// <param name="raiseSelectionChanged"></param>
    void Select(QSharedPointer<StrokeCollection> strokes, QList<UIElement*> const & elements, bool raiseSelectionChanged);

    /// <summary>
    /// Our method to commit the current editing.
    /// Called by EndFeedbackAdorner or InkCanvas.PasteFromDataObject
    /// </summary>
    void CommitChanges(QRectF const & finalRectangle, bool raiseEvent);

    /// <summary>
    /// Try to remove an element from the selected elements list.
    /// Called by:
    ///             OnCanvasLayoutUpdated
    /// </summary>
    /// <param name="removedElement"></param>
    void RemoveElement(UIElement* removedElement);

    /// <summary>
    /// UpdateElementBounds:
    ///     Called by InkCanvasSelection.MoveSelection
    /// </summary>
    /// <param name="element"></param>
    /// <param name="transform"></param>
    void UpdateElementBounds(UIElement* element, QMatrix const & transform);

    /// <summary>
    /// UpdateElementBounds:
    ///     Called by   InkCanvasSelection.UpdateElementBounds
    ///                 ClipboardProcessor.CopySelectionInXAML
    /// </summary>
    /// <param name="originalElement"></param>
    /// <param name="updatedElement"></param>
    /// <param name="transform"></param>
    void UpdateElementBounds(UIElement* originalElement, UIElement* updatedElement, QMatrix const & transform);

    void TransformStrokes(QSharedPointer<StrokeCollection> strokes, QMatrix const & matrix);

    InkCanvasSelectionHitResult HitTestSelection(QPointF const & pointOnInkCanvas);

    /// <summary>
    /// helper to used to determine if the current selection
    /// is different than the selection passed
    /// </summary>
    /// <param name="strokes">strokes</param>
    /// <param name="strokesAreDifferent">strokesAreDifferent</param>
    /// <param name="elements">elements</param>
    /// <param name="elementsAreDifferent">elementsAreDifferent</param>
    void SelectionIsDifferentThanCurrent(QSharedPointer<StrokeCollection> strokes,
                                                bool& strokesAreDifferent,
                                                QList<UIElement*> const & elements,
                                                bool& elementsAreDifferent);

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
    bool HasHitSingleSelectedElement(QPointF const & pointOnInnerCanvas);

    /// <summary>
    /// helper to stop listening to stroke changes
    /// </summary>
    void QuitListeningToStrokeChanges();

    /// <summary>
    /// helper to listen to stroke changes
    /// </summary>
    void ListenToStrokeChanges();

    /// <summary>
    /// Helper method that take a finalRectangle and raised the appropriate
    /// events on the InkCanvas.  Handles cancellation
    /// </summary>
    void CommitMoveChange(QRectF finalRectangle);

    /// <summary>
    /// Helper method that take a finalRectangle and raised the appropriate
    /// events on the InkCanvas.  Handles cancellation
    /// </summary>
    void CommitResizeChange(QRectF finalRectangle);

    /// <summary>
    /// helper that moves all selected elements from the previous location
    /// to the new one
    /// </summary>
    /// <param name="previousRect"></param>
    /// <param name="newRect"></param>
    void MoveSelection(QRectF const & previousRect, QRectF const & newRect);

    /// <summary>
    /// A handler which receives LayoutUpdated from the layout manager. Since the selected elements can be animated,
    /// moved, resized or even removed programmatically from the InkCanvas, we have to monitor
    /// the any layout changes caused by those actions.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnCanvasLayoutUpdated(EventArgs& e);

    /// <summary>
    /// Our own listener for packet changes
    /// </summary>
    void OnStrokeInvalidated(EventArgs& e);

    /// <summary>
    /// Our own listener for strokes changed.
    /// This is used so that if someone deletes or modifies a stroke
    /// we are currently displaying for selection, we can update our size
    /// </summary>
    void OnStrokeCollectionChanged(StrokeCollectionChangedEventArgs& e);

    /// <summary>
    /// Get the cached bounding boxes to be updated.
    /// </summary>
    QRectF GetStrokesBounds();

    /// <summary>
    /// Get the bounding boxes of the selected elements.
    /// </summary>
    /// <returns></returns>
    QList<QRectF> GetElementsBounds();

    /// <summary>
    /// Get the union box of the selected elements.
    /// </summary>
    /// <returns></returns>
    QRectF GetElementsUnionBounds();

    /// <summary>
    /// Update the selection adorners state.
    /// The method is called by:
    ///         MoveSelection
    ///         OnCanvasLayoutUpdated
    ///         OnStrokeCollectionChanged
    ///         RemoveElement
    ///         Select
    /// </summary>
    void UpdateSelectionAdorner();

    /// <summary>
    /// Ensure the rendering is valid.
    /// Called by:
    ///         SelectedElementsBounds
    /// </summary>
    void EnusreElementsBounds();

    /// <summary>
    /// Utility for computing a transformation that maps one rect to another
    /// </summary>
    /// <param name="target">Destination Rectangle</param>
    /// <param name="source">Source Rectangle</param>
    /// <returns>Transform that maps source rectangle to destination rectangle</returns>
    static QMatrix MapRectToRect(QRectF const & target, QRectF const & source);

    /// <summary>
    /// Adds/Removes the LayoutUpdated handler according to whether there is selected elements or not.
    /// </summary>
    void UpdateCanvasLayoutUpdatedHandler();

    /// <summary>
    /// helper method used to determine if two stroke collection either
    /// 1) are both nullptr
    /// or
    /// 2) both refer to the identical set of strokes
    /// </summary>
    static bool StrokesAreEqual(QSharedPointer<StrokeCollection> strokes1, QSharedPointer<StrokeCollection> strokes2);

    /// <summary>
    /// helper method used to determine if two collection either
    /// 1) are both nullptr
    /// or
    /// 2) both refer to the identical set of elements
    /// </summary>
    static bool FrameworkElementArraysAreEqual(QList<UIElement*> const & elements1, QList<UIElement*> const & elements2);

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
    QList<QRectF> SelectedElementsBoundsEnumerator();

    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

    //#region Fields

    /// <summary>
    /// Our logical parent.
    /// </summary>
private:
    InkCanvas&                   _inkCanvas;

    /// <summary>
    /// The collection of strokes this selectedinkelement represents
    /// </summary>
    QSharedPointer<StrokeCollection>            _selectedStrokes;
    /// <summary>
    /// The last known rectangle before the current edit.  used to revert
    /// when events are cancelled and expose the value to the InkCanvas.GetSelectionBounds
    /// </summary>
    QRectF                        _cachedStrokesBounds;
    bool                        _areStrokesChanged;

    /// <summary>
    /// The collection of elements this selectedinkelement represents
    /// </summary>
    QList<UIElement*>             _selectedElements;
    EventHandler                _layoutUpdatedHandler;

    std::pair<InkCanvasSelectionHitResult, bool> _activeSelectionHitResult;

    //#endregion Fields

};

INKCANVAS_END_NAMESPACE

#endif // INKCANVASSELECTION_H
