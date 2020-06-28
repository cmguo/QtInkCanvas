#ifndef SELECTIONEDITINGBEHAVIOR_H
#define SELECTIONEDITINGBEHAVIOR_H

#include "editingbehavior.h"
#include "Windows/Controls/inkcanvasselectionhitresult.h"
#include "Windows/rect.h"

INKCANVAS_BEGIN_NAMESPACE

class MouseEventArgs;
class MouseButtonEventArgs;

// namespace MS.Internal.Ink

class SelectionEditingBehavior : public EditingBehavior
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
    /// SelectionEditingBehavior constructor
    /// </summary>
    SelectionEditingBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

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
    virtual void OnActivate();

    /// <summary>
    /// Called when the ResizeEditingBehavior is detached
    /// </summary>
    virtual void OnDeactivate();

    virtual void OnCommit(bool commit);

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
    /// The handler of the MouseMove event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnMouseMove(MouseEventArgs& args);

    /// <summary>
    /// The handler of the MouseUp event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnMouseUp(MouseButtonEventArgs& args);

    /// <summary>
    /// GetInkCanvas().LostStylusCapture handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnLostMouseCapture(MouseEventArgs& args);


    /// <summary>
    /// Get the new feedback rectangle based on the location
    /// </summary>
    /// <param name="newPoint"></param>
    Rect ChangeFeedbackRectangle(Point & newPoint);

    /// <summary>
    ///     Resize a given element based on the grab handle
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    Rect CalculateRect(double x, double y);

    // ExtendSelectionLeft
    static Rect ExtendSelectionLeft(Rect const & rect, double extendBy);

    // ExtendSelectionTop
    static Rect ExtendSelectionTop(Rect const & rect, double extendBy);

    // ExtendSelectionRight
    static Rect ExtendSelectionRight(Rect const & rect, double extendBy);

    // ExtendSelectionBottom
    static Rect ExtendSelectionBottom(Rect const & rect, double extendBy);

    /// <summary>
    /// Capture Mouse
    /// </summary>
    void InitializeCapture();

    /// <summary>
    /// Release Mouse capture
    /// </summary>
    /// <param name="releaseDevice"></param>
    /// <param name="commit"></param>
    void ReleaseCapture(bool releaseDevice, bool commit);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

    //#region Fields

private:
    static constexpr double MinimumHeightWidthSize = 16.0;
    Point _previousLocation;
    Rect _previousRect;
    Rect _selectionRect;
    InkCanvasSelectionHitResult _hitResult;
    bool _actionStarted = false;

    //#endregion Fields
};

INKCANVAS_END_NAMESPACE

#endif // SELECTIONEDITINGBEHAVIOR_H
