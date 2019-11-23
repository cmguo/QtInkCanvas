#ifndef SELECTIONEDITOR_H
#define SELECTIONEDITOR_H

#include "editingbehavior.h"
#include "inkcanvasselectionhitresult.h"

class MouseEventArgs;
class MouseButtonEventArgs;

/// <summary>
///     SelectionEditor
/// </summary>
class SelectionEditor : public EditingBehavior
{

    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors
public:
    /// <summary>
    /// SelectionEditor constructor
    /// </summary>
    SelectionEditor(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

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
    void OnInkCanvasSelectionChanged();

    //#endregion Internal Methods

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

protected:
    /// <summary>
    /// Attach to new scope as defined by this element.
    /// </summary>
    virtual void OnActivate();

    /// <summary>
    /// Called when the SelectionEditor is detached
    /// </summary>
    virtual void OnDeactivate();

    /// <summary>
    /// OnCommit
    /// </summary>
    /// <param name="commit"></param>
    virtual void OnCommit(bool commit);

    /// <summary>
    /// Retrieve the cursor
    /// </summary>
    /// <returns></returns>
    virtual QCursor GetCurrentCursor();

    //#endregion Protected Methods

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

private:
    /// <summary>
    /// SelectionAdorner MouseButtonDown
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    /// <SecurityNote>
    ///     Critical: Calls critical method EditingCoordinator.ActivateDynamicBehavior
    ///
    ///     TreatAsSafe: Doesn't handle critical data.  Also, this method is called by
    ///         SecurityTransparent code in the input system and can not be marked SecurityCritical.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    void OnAdornerMouseButtonDownEvent(MouseButtonEventArgs& args);

    /// <summary>
    /// Selection Adorner MouseMove
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    /// <SecurityNote>
    ///     Critical: Calls critical method EditingCoordinator.ActivateDynamicBehavior
    ///
    ///     TreatAsSafe: Doesn't handle critical data.  Also, this method is called by
    ///         SecurityTransparent code in the input system and can not be marked SecurityCritical.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    void OnAdornerMouseMoveEvent(MouseEventArgs& args);

    /// <summary>
    /// Adorner MouseLeave Handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnAdornerMouseLeaveEvent(MouseEventArgs& args);

    /// <summary>
    /// Hit test for the grab handles
    /// </summary>
    /// <param name="position"></param>
    /// <returns></returns>
    InkCanvasSelectionHitResult HitTestOnSelectionAdorner(QPointF const & position);

    /// <summary>
    /// This method updates the cursor when the mouse is hovering ont the selection adorner.
    /// It is called from
    ///  OnAdornerMouseLeaveEvent
    ///  OnAdornerMouseEvent
    /// </summary>
    /// <param name="hitPoint">the handle is being hit</param>
    void UpdateSelectionCursor(QPointF const & hitPoint);


    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

private:
    //#region Fields

    InkCanvasSelectionHitResult _hitResult;

    //#endregion Fields
};

#endif // SELECTIONEDITOR_H
