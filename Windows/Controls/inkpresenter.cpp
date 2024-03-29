#include "Windows/Controls/inkpresenter.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Controls/inkevents.h"
#include "Windows/dependencypropertychangedeventargs.h"
#include "eventargs.h"
#include "Internal/Ink/renderer.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

//#region Constructors

/// <summary>
/// The constructor of InkPresenter
/// </summary>
InkPresenter::InkPresenter()
{
    // Create the internal Renderer object.
    _renderer = new Renderer(this);

    StrokesChangedHandlers(Strokes(), nullptr);

    //_contrastCallback = new InkPresenterHighContrastCallback(this);

    // Register rti high contrast callback. Then check whether we are under the high contrast already.
    //HighContrastHelper.RegisterHighContrastCallback(_contrastCallback);
    //if ( SystemParameters.HighContrast )
    //{
    //    _contrastCallback.TurnHighContrastOn(SystemColors.WindowTextColor);
    //}

    //_constraintSize = Size.Empty;
}

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
void InkPresenter::AttachVisuals(Visual* visual, SharedPointer<DrawingAttributes> drawingAttributes)
{
    VerifyAccess();

    EnsureRootVisual();
    _renderer->AttachIncrementalRendering(visual, drawingAttributes);
}

/// <summary>
/// DetachVisual method
/// </summary>
/// <param name="visual">The stroke visual which needs to be detached</param>
void InkPresenter::DetachVisuals(Visual* visual)
{
    VerifyAccess();

    EnsureRootVisual();
    _renderer->DetachIncrementalRendering(visual);
}

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
DependencyProperty const * const InkPresenter::StrokesProperty =
        new DependencyProperty(new StrokeCollectionDefaultValueFactory,
                               &InkPresenter::OnStrokesChanged);

/// <summary>
/// Gets/Sets the Strokes property.
/// </summary>
SharedPointer<StrokeCollection> InkPresenter::Strokes()
{
    return GetValue<SharedPointer<StrokeCollection>>(StrokesProperty);
}
void InkPresenter::SetStrokes(SharedPointer<StrokeCollection> value)
{
    SetValue(StrokesProperty, value);
}

void InkPresenter::OnStrokesChanged(DependencyObject& d, DependencyPropertyChangedEventArgs& e)
{
    InkPresenter& inkPresenter = static_cast<InkPresenter&>(d);

    SharedPointer<StrokeCollection> oldValue = e.OldValue().value<SharedPointer<StrokeCollection>>();
    SharedPointer<StrokeCollection> newValue = e.NewValue().value<SharedPointer<StrokeCollection>>();

    inkPresenter.StrokesChangedHandlers(newValue, oldValue);
    inkPresenter.OnStrokeChanged(EventArgs::Empty);
}


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
Size InkPresenter::MeasureOverride(Size constraint)
{
    // No need to call VerifyAccess since we call the method on the base here.

    SharedPointer<StrokeCollection> strokes = Strokes();

    // Measure the child first
    Size newSize = UIElement::MeasureOverride(constraint);

    // If there are strokes in IP, we need to combine the size to final size.
    if ( strokes != nullptr && strokes->Count() != 0 )
    {
        // Get the bounds of the stroks
        Rect boundingRect = StrokesBounds();

        // If we have an empty bounding box or the Right/Bottom value is negative,
        // an empty size will be returned.
        if ( !boundingRect.IsEmpty() && boundingRect.Right() > 0.0 && boundingRect.Bottom() > 0.0 )
        {
            // The new size needs to contain the right boundary and bottom boundary.
            Size sizeStrokes = Size(boundingRect.Right(), boundingRect.Bottom());

            newSize.setWidth(qMax(newSize.Width(), sizeStrokes.Width()));
            newSize.setHeight(qMax(newSize.Height(), sizeStrokes.Height()));
        }
    }

    if ( Child() != nullptr )
    {
        _constraintSize = constraint;
    }
    else
    {
        _constraintSize = Size();
    }

    return newSize;
}

/// <summary>
/// Override of <seealso cref="FrameworkElement.ArrangeOverride" />.
/// </summary>
/// <param name="arrangeSize">Size that element should use to arrange itself and its children.</param>
/// <returns>The InkPresenter's desired size.</returns>
Size InkPresenter::ArrangeOverride(Size arrangeSize)
{
    VerifyAccess();

    EnsureRootVisual();

    // NTRAID-WINDOWSOS#1091908-WAYNEZEN,
    // When we arrange the child, we shouldn't count in the strokes' bounds.
    // We only use the constraint size for the child.
    Size availableSize = arrangeSize;
    if ( !_constraintSize.IsEmpty() )
    {
        availableSize = Size(qMin(arrangeSize.Width(), _constraintSize.Width()),
                                        qMin(arrangeSize.Height(), _constraintSize.Height()));
    }

    // We arrange our child as what Decorator does
    // exceopt we are using the available size computed from our cached measure size.
    UIElement* child = Child();
    if ( child != nullptr )
    {
        child->Arrange(Rect(Point(0, 0), availableSize));
    }

    return arrangeSize;
}

/// <summary>
/// The overridden GetLayoutClip method
/// </summary>
/// <returns>Geometry to use as additional clip if ClipToBounds=true</returns>
Geometry* InkPresenter::GetLayoutClip(Size layoutSlotSize)
{
    // NTRAID:WINDOWSOS#1516798-2006/02/17-WAYNEZEN
    // By default an FE will clip its content if the ink size exceeds the layout size (the final arrange size).
    // Since we are auto growing, the ink size is same as the desired size. So it ends up the strokes will be clipped
    // regardless ClipToBounds is set or not.
    // We override the GetLayoutClip method so that we can bypass the default layout clip if ClipToBounds is set to false.
    // So we allow the ink to be drown anywhere when no clip is set.
    if ( ClipToBounds() )
    {
        return UIElement::GetLayoutClip(layoutSlotSize);
    }
    else
        return nullptr;
}

/// <summary>
/// Returns the child at the specified index.
/// </summary>
Visual* InkPresenter::GetVisualChild(int index)
{
    int count = VisualChildrenCount();

    if(count == 2)
    {
        switch (index)
        {
            case 0:
                return  Child();

            case 1:
                return _renderer->RootVisual();

            default:
                throw std::runtime_error("index");
        }
    }
    else if (index == 0 && count == 1)
    {
        if ( _hasAddedRoot )
        {
            return _renderer->RootVisual();
        }
        else if(Child() != nullptr)
        {
            return Child();
        }
    }
    throw std::runtime_error("index");
}


/// <summary>
///  Derived classes override this property to enable the Visual code to enumerate
///  the Visual children. Derived classes need to return the number of children
///  from this method.
///
///    By default a Visual does not have any children.
///
///  Remark: During this virtual method the Visual tree must not be modified.
/// </summary>
int InkPresenter::VisualChildrenCount()
{
    // we can have 4 states:-
    // 1. no children
    // 2. only Child()
    // 3. only _renderer->RootVisual as the child
    // 4. both Child() and  _renderer->RootVisual

    if(Child() != nullptr)
    {
        if ( _hasAddedRoot )
        {
           return 2;
        }
        else
        {
            return 1;
        }
    }
    else if ( _hasAddedRoot )
    {
        return 1;
    }

    return 0;
}

/// <summary>
/// UIAutomation support
/// </summary>
AutomationPeer* InkPresenter::OnCreateAutomationPeer()
{
    return nullptr;// new InkPresenterAutomationPeer(this);
}


//#endregion Protected Methods

//#region Internal Methods

/// <summary>
/// Internal helper used to indicate if a visual was previously attached
/// via a call to AttachIncrementalRendering
/// </summary>
bool InkPresenter::ContainsAttachedVisual(Visual* visual)
{
    VerifyAccess();
    return _renderer->ContainsAttachedIncrementalRenderingVisual(visual);
}


/// <summary>
/// Internal helper used to determine if a visual is in the right spot in the visual tree
/// </summary>
bool InkPresenter::AttachedVisualIsPositionedCorrectly(Visual* visual, SharedPointer<DrawingAttributes> drawingAttributes)
{
    VerifyAccess();
    return _renderer->AttachedVisualIsPositionedCorrectly(visual, drawingAttributes);
}

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

class InkPresenter::InkPresenterHighContrastCallback
{
    //------------------------------------------------------
    //
    //  Cnostructors
    //
    //------------------------------------------------------
public:
    //#region Constructors

    InkPresenterHighContrastCallback(InkPresenter& inkPresenter)
        : _thisInkPresenter(inkPresenter)
    {
        //_thisInkPresenter = inkPresenter;
    }

    InkPresenterHighContrastCallback()
        : _thisInkPresenter(*reinterpret_cast<InkPresenter*>(1))
    { }

    //#endregion Constructors

    //------------------------------------------------------
    //
    //  Internal Methods
    //
    //------------------------------------------------------

    //#region Internal Methods

    /// <summary>
    /// TurnHighContrastOn
    /// </summary>
    /// <param name="highContrastColor"></param>
    virtual void TurnHighContrastOn(QColor highContrastColor)
    {
        _thisInkPresenter._renderer->TurnHighContrastOn(highContrastColor);
        _thisInkPresenter.OnStrokeChanged2();
    }

    /// <summary>
    /// TurnHighContrastOff
    /// </summary>
    virtual void TurnHighContrastOff()
    {
        _thisInkPresenter._renderer->TurnHighContrastOff();
        _thisInkPresenter.OnStrokeChanged2();
    }

    //#endregion Internal Methods

    //------------------------------------------------------
    //
    //  Internal Properties
    //
    //------------------------------------------------------

    //#region Internal Properties

    /// <summary>
    /// Returns the dispatcher if the object is associated to a UIContext.
    /// </summary>
    virtual Dispatcher* Dispatcher()
    {
        return _thisInkPresenter.GetDispatcher();
    }

    //#endregion Internal Properties

    //------------------------------------------------------
    //
    //  Fields
    //
    //------------------------------------------------------

    //#region Fields
private:
    InkPresenter& _thisInkPresenter;

    //#endregion Fields
};

//#endregion Classes

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

void InkPresenter::StrokesChangedHandlers(SharedPointer<StrokeCollection> newStrokes, SharedPointer<StrokeCollection> oldStrokes)
{
    Debug::Assert(newStrokes != nullptr, "Cannot set a nullptr to InkPresenter");

    // Remove the event handlers from the old stroke collection
    if ( nullptr != oldStrokes )
    {
        // Stop listening on events from the stroke collection.
        //oldStrokes.StrokesChanged -= new StrokeCollectionChangedEventHandler(OnStrokesChanged);
        QObject::disconnect(oldStrokes.get(), &StrokeCollection::StrokesChanged,
                         this, &InkPresenter::OnStrokesChanged2);
    }

    // Start listening on events from the stroke collection.
    //newStrokes.StrokesChanged += new StrokeCollectionChangedEventHandler(OnStrokesChanged);
    QObject::connect(newStrokes.get(), &StrokeCollection::StrokesChanged,
                     this, &InkPresenter::OnStrokesChanged2);

    // Replace the renderer stroke collection.
    _renderer->SetStrokes(newStrokes);

    SetStrokeChangedHandlers(newStrokes, oldStrokes);
}

/// <summary>
/// StrokeCollectionChanged event handler
/// </summary>
void InkPresenter::OnStrokesChanged2(StrokeCollectionChangedEventArgs& eventArgs)
{
    //System.Diagnostics.Debug::Assert(sender == Strokes());

    SetStrokeChangedHandlers(eventArgs.Added(), eventArgs.Removed());
    OnStrokeChanged(EventArgs::Empty);
}

void InkPresenter::SetStrokeChangedHandlers(SharedPointer<StrokeCollection> addedStrokes, SharedPointer<StrokeCollection> removedStrokes)
{
    Debug::Assert(addedStrokes != nullptr, "The added StrokeCollection cannot be nullptr.");
    int count, i;

    if ( removedStrokes != nullptr )
    {
        // Deal with removed strokes first
        count = removedStrokes->Count();
        for ( i = 0; i < count; i++ )
        {
            StopListeningOnStrokeEvents((*removedStrokes)[i]);
        }
    }

    // Add new strokes
    count = addedStrokes->Count();
    for ( i = 0; i < count; i++ )
    {
        StartListeningOnStrokeEvents((*addedStrokes)[i]);
    }
}

void InkPresenter::OnStrokeChanged(EventArgs&)
{
    OnStrokeChanged2();
}

/// <summary>
/// The method is called from Stroke setter, OnStrokesChanged, DrawingAttributesChanged and OnPacketsChanged
/// </summary>
void InkPresenter::OnStrokeChanged2()
{
    // Recalculate the bound of the StrokeCollection
    _cachedBounds.second = false;
    // Invalidate the current measure when any change happens on the stroke or strokes.
    InvalidateMeasure();
}

/// <summary>
/// Attaches event handlers to stroke events
/// </summary>
void InkPresenter::StartListeningOnStrokeEvents(SharedPointer<Stroke> stroke)
{
    //System.Diagnostics.Debug::Assert(stroke != nullptr);
    //stroke.Invalidated += new EventHandler(OnStrokeChanged);
    QObject::connect(stroke.get(), &Stroke::Invalidated,
                     this, &InkPresenter::OnStrokeChanged2);
}

/// <summary>
/// Detaches event handlers from stroke
/// </summary>
void InkPresenter::StopListeningOnStrokeEvents(SharedPointer<Stroke> stroke)
{
    //System.Diagnostics.Debug::Assert(stroke != nullptr);
    //stroke.Invalidated -= new EventHandler(OnStrokeChanged);
    QObject::disconnect(stroke.get(), &Stroke::Invalidated,
                     this, &InkPresenter::OnStrokeChanged2);
}

/// <summary>
/// Ensure the renderer root to be connected. The method is called from
///     AttachVisuals
///     DetachVisuals
///     ArrangeOverride
/// </summary>
void InkPresenter::EnsureRootVisual()
{
    if ( !_hasAddedRoot )
    {
        // Ideally we should set _hasAddedRoot to true before calling AddVisualChild.
        // So that VisualDiagnostics.OnVisualChildChanged can get correct child index.
        // However it is not clear what can regress. For now we'll temporary set
        // _parentIndex property. We'll use _parentIndex.
        // Note that there is a comment in Visual.cs stating that _parentIndex should
        // be set to -1 in DEBUG builds when child is removed. We are not going to
        // honor it. There is no _parentIndex == -1 validation is performed anywhere.
        //_renderer->RootVisual()->_parentIndex = 0;

        AddVisualChild(_renderer->RootVisual());
        _hasAddedRoot = true;
    }
}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------

//#region Properties

Rect InkPresenter::StrokesBounds()
{
    if ( !_cachedBounds.second )
    {
        _cachedBounds.first = Strokes()->GetBounds();
        _cachedBounds.second = true;
    }

    return _cachedBounds.first;
}

INKCANVAS_END_NAMESPACE
