#ifndef WINDOWS_CONTROLS_INKPRESENTER_H
#define WINDOWS_CONTROLS_INKPRESENTER_H

#include "Windows/uielement.h"
#include "Windows/Controls/decorator.h"
#include "sharedptr.h"

#include <utility>

INKCANVAS_BEGIN_NAMESPACE

class Renderer;
class Stroke;
class StrokeCollection;
class StrokeCollectionChangedEventArgs;
class EventArgs;
class DrawingAttributes;
class DependencyPropertyChangedEventArgs;
class AutomationPeer;

// namespace System.Windows.Controls

/// <summary>
/// Renders the specified StrokeCollection data.
/// </summary>
class InkPresenter : public Decorator
{
    Q_OBJECT
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------
public:
    //#region Constructors

    /// <summary>
    /// The constructor of InkPresenter
    /// </summary>
    InkPresenter();

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

    /// <summary>
    /// AttachVisual method
    /// </summary>
    /// <param name="visual">The stroke visual which needs to be attached</param>
    /// <param name="drawingAttributes">The DrawingAttributes of the stroke</param>
    void AttachVisuals(Visual* visual, SharedPointer<DrawingAttributes> drawingAttributes);

    /// <summary>
    /// DetachVisual method
    /// </summary>
    /// <param name="visual">The stroke visual which needs to be detached</param>
    void DetachVisuals(Visual* visual);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

    /// <summary>
    /// The DependencyProperty for the Strokes property.
    /// </summary>
    static DependencyProperty const * const StrokesProperty;

    /// <summary>
    /// Gets/Sets the Strokes property.
    /// </summary>
    SharedPointer<StrokeCollection> Strokes();
    void SetStrokes(SharedPointer<StrokeCollection> value);

private:
    static void OnStrokesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e);


    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

    /// <summary>
    /// Override of <seealso cref="FrameworkElement.MeasureOverride" />
    /// </summary>
    /// <param name="constraint">Constraint size.</param>
    /// <returns>Computed desired size.</returns>
protected:
    virtual Size MeasureOverride(Size constraint);

    /// <summary>
    /// Override of <seealso cref="FrameworkElement.ArrangeOverride" />.
    /// </summary>
    /// <param name="arrangeSize">Size that element should use to arrange itself and its children.</param>
    /// <returns>The InkPresenter's desired size.</returns>
    virtual Size ArrangeOverride(Size arrangeSize);

    /// <summary>
    /// The overridden GetLayoutClip method
    /// </summary>
    /// <returns>Geometry to use as additional clip if ClipToBounds=true</returns>
    virtual Geometry* GetLayoutClip(Size layoutSlotSize);

    /// <summary>
    /// Returns the child at the specified index.
    /// </summary>
    virtual Visual* GetVisualChild(int index);


    /// <summary>
    ///  Derived classes override this property to enable the Visual code to enumerate
    ///  the Visual children. Derived classes need to return the number of children
    ///  from this method.
    ///
    ///    By default a Visual does not have any children.
    ///
    ///  Remark: During this virtual method the Visual tree must not be modified.
    /// </summary>
    virtual int VisualChildrenCount();

    /// <summary>
    /// UIAutomation support
    /// </summary>
    virtual AutomationPeer* OnCreateAutomationPeer();


    //#endregion Protected Methods

    //#region Internal Methods

public:
    /// <summary>
    /// Internal helper used to indicate if a visual was previously attached
    /// via a call to AttachIncrementalRendering
    /// </summary>
    bool ContainsAttachedVisual(Visual* visual);

    /// <summary>
    /// Internal helper used to determine if a visual is in the right spot in the visual tree
    /// </summary>
    bool AttachedVisualIsPositionedCorrectly(Visual* visual, SharedPointer<DrawingAttributes> drawingAttributes);

    //#endregion Internal Methods
    //------------------------------------------------------
    //
    //  Classes
    //
    //------------------------------------------------------

    //#region Classes

    /// <summary>
    /// A helper class for the high contrast support
    /// </summary>
private:
    class InkPresenterHighContrastCallback;

    //#endregion Classes

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

private:
    void StrokesChangedHandlers(SharedPointer<StrokeCollection> newStrokes, SharedPointer<StrokeCollection> oldStrokes);

    /// <summary>
    /// StrokeCollectionChanged event handler
    /// </summary>
    void OnStrokesChanged2(StrokeCollectionChangedEventArgs& eventArgs);

    void SetStrokeChangedHandlers(SharedPointer<StrokeCollection> addedStrokes, SharedPointer<StrokeCollection> removedStrokes);

    void OnStrokeChanged(EventArgs& e);

    /// <summary>
    /// The method is called from Stroke setter, OnStrokesChanged, DrawingAttributesChanged and OnPacketsChanged
    /// </summary>
    void OnStrokeChanged2();

    /// <summary>
    /// Attaches event handlers to stroke events
    /// </summary>
    void StartListeningOnStrokeEvents(SharedPointer<Stroke> stroke);

    /// <summary>
    /// Detaches event handlers from stroke
    /// </summary>
    void StopListeningOnStrokeEvents(SharedPointer<Stroke> stroke);

    /// <summary>
    /// Ensure the renderer root to be connected. The method is called from
    ///     AttachVisuals
    ///     DetachVisuals
    ///     ArrangeOverride
    /// </summary>
    void EnsureRootVisual();

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

    Rect StrokesBounds();

    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------
private:
    //#region Fields
    Renderer*    _renderer = nullptr;
    std::pair<Rect, bool>  _cachedBounds;
    bool            _hasAddedRoot = false;
    //
    // HighContrast support
    //
    //InkPresenterHighContrastCallback*    _contrastCallback = nullptr;

    Size                                 _constraintSize = Size::Empty();

    //#endregion Fields

};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_CONTROLS_INKPRESENTER_H
