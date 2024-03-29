#include "Internal/Ink/renderer.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Media/visual.h"
#include "Windows/Media/drawingcontext.h"
#include "Internal/Ink/strokerenderer.h"
#include "Windows/Ink/events.h"
#include "Windows/Media/hittestresult.h"
#include "Windows/Media/drawingvisual.h"
#include "eventargs.h"
#include "Internal/debug.h"
#include "Internal/finallyhelper.h"

INKCANVAS_BEGIN_NAMESPACE

class Renderer::StrokeVisual : public DrawingVisual
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="stroke">a stroke to render into this visual</param>
    /// <param name="renderer">a renderer associated to this visual</param>
    StrokeVisual(SharedPointer<Stroke> stroke, Renderer& renderer)
        : _renderer(renderer)
    {
        //Debug::Assert(renderer != nullptr);

        if (stroke == nullptr)
        {
            throw std::runtime_error("stroke");
        }

        _stroke = stroke;
        //_renderer = renderer;

        setObjectName("StrokeVisual");

        // The original value of the color and IsHighlighter are cached so
        // when Stroke.Invalidated is fired, Renderer knows whether re-arranging
        // the visual tree is needed.
        _cachedColor = stroke->GetDrawingAttributes()->Color();
        _cachedIsHighlighter = stroke->GetDrawingAttributes()->IsHighlighter();

        // Update the visual contents
        Update();
    }

    /// <summary>
    /// The Stroke rendedered into this visual.
    /// </summary>
    SharedPointer<Stroke> GetStroke()
    {
        return _stroke;
    }

    /// <summary>
    /// Updates the contents of the visual.
    /// </summary>
    void Update()
    {
        std::unique_ptr<DrawingContext> drawingContext(RenderOpen());
        {
            FinallyHelper final([&drawingContext](){
               drawingContext->Close();
            });
            bool highContrast = _renderer.IsHighContrast();

            if (highContrast == true && _stroke->GetDrawingAttributes()->IsHighlighter())
            {
                // we don't render highlighters in high contrast
                return;
            }

            SharedPointer<DrawingAttributes> da;
            if (highContrast)
            {
                da = _stroke->GetDrawingAttributes()->Clone();
                da->SetColor(_renderer.GetHighContrastColor());
            }
            else if (_stroke->GetDrawingAttributes()->IsHighlighter() == true)
            {
                // Get the drawing attributes to use for a highlighter stroke. This can be a copied DA with color.A
                // overridden if color.A != 255.
                da = StrokeRenderer::GetHighlighterAttributes(*_stroke, _stroke->GetDrawingAttributes());
            }
            else
            {
                // Otherwise, usethe DA on this stroke
                da = _stroke->GetDrawingAttributes();
            }

            // Draw selected stroke as hollow
            _stroke->DrawInternal (*drawingContext, da, _stroke->IsSelected() );
        }
    }

protected:
    /// <summary>
    /// StrokeVisual should not be hittestable as it interferes with event routing
    /// </summary>
    virtual HitTestResult HitTestCore(PointHitTestParameters hitTestParams)
    {
        (void) hitTestParams;
        return nullptr;
    }

public:
    /// <summary>
    /// The previous value of IsHighlighter
    /// </summary>
    bool CachedIsHighlighter()
    {
        return _cachedIsHighlighter;
    }
    void SetCachedIsHighlighter(bool value)
    {
        _cachedIsHighlighter = value;
    }


    /// <summary>
    /// The previous value of Color
    /// </summary>
    QColor CachedColor()
    {
        return _cachedColor;
    }
    void SetCachedColor(QColor value)
    {
        _cachedColor = value;
    }


private:
    SharedPointer<Stroke>                      _stroke;
    bool                        _cachedIsHighlighter;
    QColor                       _cachedColor;
    Renderer&                    _renderer;

};



/// <summary>
/// Public Constructor
/// </summary>
Renderer::Renderer(QObject * parent)
    : QObject(parent)
{
    // Initialize the data members.
    // We intentionally don't use lazy initialization for the core members to avoid
    // hidden bug situations when one thing has been created while another not.
    // If the user is looking for lazy initialization, she should do that on her
    // own and create Renderer only when there's a need for it.

    // Create visuals that'll be the containers for all other visuals
    // created by the Renderer. This visuals are created once and are
    // not supposed to be replaced nor destroyed while the Renderer is alive.
    _rootVisual = new ContainerVisual();
    _rootVisual->setObjectName("Renderer::RootVisual");
    _highlightersRoot = new ContainerVisual();
    _highlightersRoot->setObjectName("Renderer::HighlightersRoot");
    _regularInkVisuals = new ContainerVisual();
    _regularInkVisuals->setObjectName("Renderer::RegularInkVisuals");
    _incrementalRenderingVisuals = new ContainerVisual();
    _incrementalRenderingVisuals->setObjectName("Renderer::IncrementalRenderingVisuals");

    // Highlighters go to the bottom, then regular ink, and regular
    // ink' incremental rendering in on top.
    VisualCollection & rootChildren = _rootVisual->Children();
    rootChildren.Add(_highlightersRoot);
    rootChildren.Add(_regularInkVisuals);
    rootChildren.Add(_incrementalRenderingVisuals);

    // Set the default value of highcontrast to be false.
    _highContrast = false;

    // Create a stroke-visual dictionary
    //_visuals = new Dictionary<Stroke, StrokeVisual>();
}

/// <summary>
/// Returns a reference to a visual tree that can be used to render the ink.
/// This property may be either a single visual or a container visual with
/// children. The element uses this visual as a child visual and arranges
/// it with respects to the other siblings.
/// Note: No visuals are actually generated until the application gets
/// this property for the first time. If no strokes are set then an empty
/// visual is returned.
/// </summary>
Visual* Renderer::RootVisual() { return _rootVisual; }

/// <summary>
/// Set the strokes property to the collection of strokes to be rendered.
/// The Renderer will then listen to changes to the StrokeCollection
/// and update its state to reflect the changes.
/// </summary>
SharedPointer<StrokeCollection> Renderer::Strokes()
{
    // We should never return a nullptr value.
    if ( _strokes == nullptr )
    {
        _strokes.reset(new StrokeCollection());

        // Start listening on events from the stroke collection.
        //_strokes->GetStroke()sChanged+= new StrokeCollectionChangedEventHandler(OnStrokesChanged);
        QObject::connect(_strokes.get(), &StrokeCollection::StrokesChanged,
                         this, &Renderer::OnStrokesChanged);
    }

    return _strokes;
}
void Renderer::SetStrokes(SharedPointer<StrokeCollection> value)
{
    if (value == nullptr)
    {
        throw std::runtime_error("value");
    }
    if (value == _strokes)
    {
        return;
    }

    // Detach the current stroke collection
    if (nullptr != _strokes)
    {
        // Stop listening on events from the stroke collection.
        //_strokes->GetStroke()sChanged-= new StrokeCollectionChangedEventHandler(OnStrokesChanged);
        QObject::disconnect(_strokes.get(), &StrokeCollection::StrokesChanged,
                         this, &Renderer::OnStrokesChanged);

        for (StrokeVisual* visual : _visuals.values())
        {
            StopListeningOnStrokeEvents(visual->GetStroke());
            // Detach the visual from the tree
            DetachVisual(visual);
            delete visual;
        }
        _visuals.clear();
    }

    // Set it.
    _strokes = value;

    // Create visuals
    for (SharedPointer<Stroke> stroke : *_strokes)
    {
        // Create a visual per stroke
        StrokeVisual* visual = new StrokeVisual(stroke, *this);

        // Store the stroke-visual pair in the dictionary
        _visuals.insert(stroke, visual);

        StartListeningOnStrokeEvents(visual->GetStroke());

        // Attach it to the visual tree
        AttachVisual(visual, true/*buildingStrokeCollection*/);
    }

    // Start listening on events from the stroke collection.
    //_strokes->GetStroke()sChanged+= new StrokeCollectionChangedEventHandler(OnStrokesChanged);
    QObject::connect(_strokes.get(), &StrokeCollection::StrokesChanged,
                     this, &Renderer::OnStrokesChanged);
}

/// <summary>
/// User supposed to use this method to attach IncrementalRenderer's root visual
/// to the visual tree of a stroke collection view.
/// </summary>
/// <param name="visual">visual to attach</param>
/// <param name="drawingAttributes">drawing attributes that used in the incremental rendering</param>
void Renderer::AttachIncrementalRendering(Visual* visual, SharedPointer<DrawingAttributes> drawingAttributes)
{
    // Check the input parameters
    if (visual == nullptr)
    {
        throw std::runtime_error("visual");
    }
    if (drawingAttributes == nullptr)
    {
        throw std::runtime_error("drawingAttributes");
    }

    //harden against eaten exceptions
    bool exceptionRaised = false;

    // Verify that the visual hasn't been attached already
    //if (_attachedVisuals != nullptr)
    {
        for(Visual* alreadyAttachedVisual : _attachedVisuals)
        {
            if (visual == alreadyAttachedVisual)
            {
                exceptionRaised = true;
                throw std::runtime_error("SR.Get(SRID.CannotAttachVisualTwice)""");
            }
        }
    }
    //else
    {
        // Create the list to register attached visuals in
    //    _attachedVisuals = new List<Visual*>();
    }


    if (!exceptionRaised)
    {
        // The position of the visual in the tree depends on the drawingAttributes
        // Find the appropriate parent visual to attach this visual to.
        ContainerVisual* parent = drawingAttributes->IsHighlighter() ? GetContainerVisual(drawingAttributes) : _incrementalRenderingVisuals;

        // Attach the visual to the tree
        parent->Children().Add(visual);

        // Put the visual into the list of visuals attached via this method
        _attachedVisuals.Add(visual);
    }
}

/// <summary>
/// Detaches a visual previously attached via AttachIncrementalRendering
/// </summary>
/// <param name="visual">the visual to detach</param>
void Renderer::DetachIncrementalRendering(Visual* visual)
{
    if (visual == nullptr)
    {
        throw std::runtime_error("visual");
    }

    // Remove the visual in the list of attached via AttachIncrementalRendering
    if ((_attachedVisuals.Remove(visual) == false))
    {
        throw std::runtime_error("SR.Get(SRID.VisualCannotBeDetached)");
    }

    // Detach it from the tree
    DetachVisual(visual);
}

/// <summary>
/// helper used to indicate if a visual was previously attached
/// via a call to AttachIncrementalRendering
/// </summary>
bool Renderer::ContainsAttachedIncrementalRenderingVisual(Visual* visual)
{
    if (visual == nullptr)
    {
        return false;
    }

    return _attachedVisuals.Contains(visual);
}

/// <summary>
/// helper used to determine if a visual is in the right spot in the visual tree
/// </summary>
bool Renderer::AttachedVisualIsPositionedCorrectly(Visual* visual, SharedPointer<DrawingAttributes> drawingAttributes)
{
    if (visual == nullptr || drawingAttributes == nullptr || !_attachedVisuals.Contains(visual))
    {
        return false;
    }

    ContainerVisual* correctParent
        = drawingAttributes->IsHighlighter() ? GetContainerVisual(drawingAttributes) : _incrementalRenderingVisuals;

    ContainerVisual* currentParent
        = ContainerVisual::fromItem(visual->parentItem());

    if (currentParent == nullptr || correctParent != currentParent)
    {
        return false;
    }
    return true;
}



/// <summary>
/// TurnOnHighContrast turns on the HighContrast rendering mode
/// </summary>
/// <param name="strokeColor">The stroke color under high contrast</param>
void Renderer::TurnHighContrastOn(QColor strokeColor)
{
    if ( !_highContrast || strokeColor != _highContrastColor )
    {
        _highContrast = true;
        _highContrastColor = strokeColor;
        UpdateStrokeVisuals();
    }
}

/// <summary>
/// ResetHighContrast turns off the HighContrast mode
/// </summary>
void Renderer::TurnHighContrastOff()
{
    if ( _highContrast )
    {
        _highContrast = false;
        UpdateStrokeVisuals();
    }
}


//#endregion

//#region Event handlers

/// <summary>
/// StrokeCollectionChanged event handler
/// </summary>
void Renderer::OnStrokesChanged(StrokeCollectionChangedEventArgs& eventArgs)
{
    ////System.Diagnostics.Debug::Assert(sender == _strokes);

    // Read the args
    SharedPointer<StrokeCollection> added = eventArgs.Added();
    SharedPointer<StrokeCollection> removed = eventArgs.Removed();

    // Add new strokes
    for (SharedPointer<Stroke> stroke : *added)
    {
        // Verify that it's not a dupe
        if (_visuals.contains(stroke))
        {
            throw std::runtime_error("SR.Get(SRID.DuplicateStrokeAdded)");
        }

        // Create a visual for the new stroke and add it to the dictionary
        StrokeVisual* visual = new StrokeVisual(stroke, *this);
        _visuals.insert(stroke, visual);

        // Start listening on the stroke events
        StartListeningOnStrokeEvents(visual->GetStroke());

        // Attach it to the visual tree
        AttachVisual(visual, false/*buildingStrokeCollection*/);
    }

    // Deal with removed strokes first
    for (SharedPointer<Stroke> stroke : *removed)
    {
        // Verify that the event is in sync with the view
        StrokeVisual* visual = nullptr;
        if (_visuals.contains(stroke))
        {
            visual = _visuals.value(stroke);
            // get rid of both the visual and the stroke
            DetachVisual(visual);
            StopListeningOnStrokeEvents(visual->GetStroke());
            _visuals.remove(stroke);
            delete visual;
        }
        else
        {
            throw std::runtime_error("SR.Get(SRID.UnknownStroke3)");
        }
    }
}

/// <summary>
/// Stroke Invalidated event handler
/// </summary>
void Renderer::OnStrokeInvalidated(EventArgs& eventArgs)
{
    (void) eventArgs;
    //System.Diagnostics.Debug::Assert(_strokes.IndexOf(sender as Stroke) != -1);

    // Find the visual associated with the changed stroke.
    StrokeVisual* visual;
    SharedPointer<Stroke> stroke = static_cast<Stroke*>(sender())->sharedFromThis();
    if (_visuals.contains(stroke) == false)
    {
        throw std::runtime_error("SR.Get(SRID.UnknownStroke1)");
    }
    visual = _visuals.value(stroke);
    // The original value of IsHighligher and Color are cached in StrokeVisual.
    // if (IsHighlighter value changed or (IsHighlighter == true and not changed and color changed)
    // detach and re-attach the corresponding visual;
    // otherwise Invalidate the corresponding StrokeVisual
    if (visual->CachedIsHighlighter() != stroke->GetDrawingAttributes()->IsHighlighter() ||
        (stroke->GetDrawingAttributes()->IsHighlighter() &&
            StrokeRenderer::GetHighlighterColor(visual->CachedColor()) != StrokeRenderer::GetHighlighterColor(stroke->GetDrawingAttributes()->Color())))
    {
        // The change requires reparenting the visual in the tree.
        DetachVisual(visual);
        AttachVisual(visual, false/*buildingStrokeCollection*/);

        // Update the cached values
        visual->SetCachedIsHighlighter(stroke->GetDrawingAttributes()->IsHighlighter());
        visual->SetCachedColor(stroke->GetDrawingAttributes()->Color());
    }
    // Update the visual.
    visual->Update();
}

//#endregion

//#region Helper methods

/// <summary>
/// Update the stroke visuals
/// </summary>
void Renderer::UpdateStrokeVisuals()
{
    for ( StrokeVisual* strokeVisual : _visuals.values() )
    {
        strokeVisual->Update();
    }
}

/// <summary>
/// Attaches a stroke visual to the tree based on the stroke's
/// drawing attributes and/or its z-order (index in the collection).
/// </summary>
void Renderer::AttachVisual(StrokeVisual* visual, bool buildingStrokeCollection)
{
    //System.Diagnostics.Debug::Assert(_strokes != nullptr);

    if (visual->GetStroke()->GetDrawingAttributes()->IsHighlighter())
    {
        // Find or create a container visual for highlighter strokes of the color
        ContainerVisual* parent = GetContainerVisual(visual->GetStroke()->GetDrawingAttributes());
        //Debug::Assert(visual is StrokeVisual);

        //insert StrokeVisuals under any non-StrokeVisuals used for dynamic inking
        int i = 0;
        for (int j = parent->Children().Count() - 1; j >= 0; j--)
        {
            //if (parent->Children()[j]->metaObject()->inherits(&StrokeVisual::staticMetaObject))
            {
                i = j + 1;
                break;
            }
        }
        parent->Children().Insert(i, visual);
    }
    else
    {
        // For regular ink we have to respect the z-order of the strokes.
        // The implementation below is not optimal in a generic case, but the
        // most simple and should work ok in most common scenarios.

        // Find the nearest non-highlighter stroke with a lower z-order
        // and insert the new visual right next to the visual of that stroke.
        StrokeVisual* precedingVisual = nullptr;
        int i = 0;
        if (buildingStrokeCollection)
        {
            SharedPointer<Stroke> visualStroke = visual->GetStroke();
            //we're building up a stroke collection, no need to start at IndexOf,
            i = qMin(_visuals.size(), _strokes->Count()); //not -1, we're about to decrement
            while (--i >= 0)
            {
                if ((*_strokes)[i] == visualStroke)
                {
                    break;
                }
            }
        }
        else
        {
            i = _strokes->IndexOf(visual->GetStroke());
        }
        while (--i >= 0)
        {
            SharedPointer<Stroke> stroke = (*_strokes)[i];
            if ((stroke->GetDrawingAttributes()->IsHighlighter() == false)
                && (_visuals.contains(stroke) == true)
                && ((precedingVisual = _visuals.value(stroke))->parentItem() != nullptr))
            {
                VisualCollection & children = ContainerVisual::fromItem(precedingVisual->parentItem())->Children();
                int index = children.IndexOf(precedingVisual);
                children.Insert(index + 1, visual);
                break;
            }
        }
        // If found no non-highlighter strokes with a lower z-order, insert
        // the stroke at the very bottom of the regular ink visual tree.
        if (i < 0)
        {
            ContainerVisual* parent = GetContainerVisual(visual->GetStroke()->GetDrawingAttributes());
            parent->Children().Insert(0, visual);
        }
    }
}

/// <summary>
/// Detaches a visual from the tree, also removes highligher parents if empty
/// when true is passed
/// </summary>
void Renderer::DetachVisual(Visual* visual)
{
    ContainerVisual* parent = ContainerVisual::fromItem(visual->parentItem());
    if (parent != nullptr)
    {
        VisualCollection & children = parent->Children();
        children.Remove(visual);

        // If the parent is a childless highlighter, detach it too.
        HighlighterContainerVisual* hcVisual = qobject_cast<HighlighterContainerVisual*>(parent);
        if (hcVisual != nullptr &&
            hcVisual->Children().Count() == 0 &&
            //_highlighters != nullptr &&
            _highlighters.values().contains(hcVisual))
        {
            DetachVisual(hcVisual);
            _highlighters.remove(hcVisual->Color());
            delete hcVisual;
        }
    }
}

/// <summary>
/// Attaches event handlers to stroke events
/// </summary>
void Renderer::StartListeningOnStrokeEvents(SharedPointer<Stroke> stroke)
{
    ////System.Diagnostics.Debug::Assert(stroke != nullptr);
    //stroke.Invalidated += new EventHandler(OnStrokeInvalidated);
    QObject::connect(stroke.get(), &Stroke::Invalidated,
                     this, &Renderer::OnStrokeInvalidated);
}

/// <summary>
/// Detaches event handlers from stroke
/// </summary>
void Renderer::StopListeningOnStrokeEvents(SharedPointer<Stroke> stroke)
{
    ////System.Diagnostics.Debug::Assert(stroke != nullptr);
    //stroke.Invalidated -= new EventHandler(OnStrokeInvalidated);
    QObject::disconnect(stroke.get(), &Stroke::Invalidated,
                     this, &Renderer::OnStrokeInvalidated);
}

INKCANVAS_END_NAMESPACE

static bool operator<(QColor const & l, QColor const & r)
{
    return l.rgba() < r.rgba();
}

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Finds a container for a new visual based on the drawing attributes
/// of the stroke rendered into that visual.
/// </summary>
/// <param name="drawingAttributes">drawing attributes</param>
/// <returns>visual</returns>
ContainerVisual* Renderer::GetContainerVisual(SharedPointer<DrawingAttributes> drawingAttributes)
{
    ////System.Diagnostics.Debug::Assert(drawingAttributes != nullptr);

    if (drawingAttributes->IsHighlighter())
    {
        // For a highlighter stroke, the color.A is neglected.
        QColor color = StrokeRenderer::GetHighlighterColor(drawingAttributes->Color());
        HighlighterContainerVisual* hcVisual = _highlighters.value(color, nullptr);
        if (hcVisual == nullptr)
        {
            //if (_highlighters == nullptr)
            //{
            //    _highlighters = new Dictionary<Color, HighlighterContainerVisual>();
            //}

            hcVisual = new HighlighterContainerVisual(color);
            hcVisual->SetOpacity(StrokeRenderer::HighlighterOpacity);
            _highlightersRoot->Children().Add(hcVisual);

            _highlighters.insert(color, hcVisual);
        }
        else if (hcVisual->parentWidget() == nullptr)
        {
            _highlightersRoot->Children().Add(hcVisual);
        }
        return hcVisual;
    }
    else
    {
        return _regularInkVisuals;
    }
}

INKCANVAS_END_NAMESPACE
