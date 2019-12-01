#ifndef INKRENDERER_H
#define INKRENDERER_H

#include <QSharedPointer>
#include <QColor>
#include <QMap>
#include <QObject>

class Visual;
class StrokeCollection;
class Stroke;
class DrawingAttributes;
class StrokeCollectionChangedEventArgs;
class EventArgs;
class ContainerVisual;

// namespace System.Windows.Ink

/// <summary>
/// The Renderer class is used to render a stroke collection.
/// This class listens to stroke added and removed events on the
/// stroke collection and updates the visual tree.
/// It also listens to the Invalidated event on Stroke (fired when
/// DrawingAttributes changes, packet data changes, DrawingAttributes
/// replaced, as well as Stroke developer calls Stroke.OnInvalidated)
/// and updates the visual state as necessary.
/// </summary>
///
//[FriendAccessAllowed] // Built into Core, also used by Framework.
class InkRenderer : public QObject
{
    Q_OBJECT
    //#region StrokeVisual

    /// <summary>
    /// A retained visual for rendering a single stroke in a stroke collection view
    /// </summary>
private:
    class StrokeVisual;

    /// <summary>
    /// helper that helps reverse map the highlighter dictionary
    /// </summary>
private:
    class HighlighterContainerVisual;

    //#endregion

    //#region interface

public:
    /// <summary>
    /// Public Constructor
    /// </summary>
    InkRenderer();

    /// <summary>
    /// Returns a reference to a visual tree that can be used to render the ink.
    /// This property may be either a single visual or a container visual with
    /// children. The element uses this visual as a child visual and arranges
    /// it with respects to the other siblings.
    /// Note: No visuals are actually generated until the application gets
    /// this property for the first time. If no strokes are set then an empty
    /// visual is returned.
    /// </summary>
    Visual* RootVisual();

    /// <summary>
    /// Set the strokes property to the collection of strokes to be rendered.
    /// The Renderer will then listen to changes to the StrokeCollection
    /// and update its state to reflect the changes.
    /// </summary>
    QSharedPointer<StrokeCollection> Strokes();
    void SetStrokes(QSharedPointer<StrokeCollection> value);

    /// <summary>
    /// User supposed to use this method to attach IncrementalRenderer's root visual
    /// to the visual tree of a stroke collection view.
    /// </summary>
    /// <param name="visual">visual to attach</param>
    /// <param name="drawingAttributes">drawing attributes that used in the incremental rendering</param>
    void AttachIncrementalRendering(Visual* visual, QSharedPointer<DrawingAttributes> drawingAttributes);

    /// <summary>
    /// Detaches a visual previously attached via AttachIncrementalRendering
    /// </summary>
    /// <param name="visual">the visual to detach</param>
    void DetachIncrementalRendering(Visual* visual);

    /// <summary>
    /// helper used to indicate if a visual was previously attached
    /// via a call to AttachIncrementalRendering
    /// </summary>
    bool ContainsAttachedIncrementalRenderingVisual(Visual* visual);

    /// <summary>
    /// helper used to determine if a visual is in the right spot in the visual tree
    /// </summary>
    bool AttachedVisualIsPositionedCorrectly(Visual* visual, QSharedPointer<DrawingAttributes> drawingAttributes);



    /// <summary>
    /// TurnOnHighContrast turns on the HighContrast rendering mode
    /// </summary>
    /// <param name="strokeColor">The stroke color under high contrast</param>
    void TurnHighContrastOn(QColor strokeColor);

    /// <summary>
    /// ResetHighContrast turns off the HighContrast mode
    /// </summary>
    void TurnHighContrastOff();

    /// <summary>
    /// Indicates whether the renderer is in high contrast mode.
    /// </summary>
    /// <returns></returns>
    bool IsHighContrast()
    {
        return _highContrast;
    }

    /// <summary>
    /// returns the stroke color for the high contrast rendering.
    /// </summary>
    /// <returns></returns>
    QColor GetHighContrastColor()
    {
        return _highContrastColor;
    }

    //#endregion

    //#region Event handlers

private:
    /// <summary>
    /// StrokeCollectionChanged event handler
    /// </summary>
    void OnStrokesChanged(StrokeCollectionChangedEventArgs& eventArgs);

    /// <summary>
    /// Stroke Invalidated event handler
    /// </summary>
    void OnStrokeInvalidated(EventArgs& eventArgs);

    //#endregion

    //#region Helper methods

    /// <summary>
    /// Update the stroke visuals
    /// </summary>
    void UpdateStrokeVisuals();

    /// <summary>
    /// Attaches a stroke visual to the tree based on the stroke's
    /// drawing attributes and/or its z-order (index in the collection).
    /// </summary>
    void AttachVisual(StrokeVisual* visual, bool buildingStrokeCollection);

    /// <summary>
    /// Detaches a visual from the tree, also removes highligher parents if empty
    /// when true is passed
    /// </summary>
    void DetachVisual(Visual* visual);

    /// <summary>
    /// Attaches event handlers to stroke events
    /// </summary>
    void StartListeningOnStrokeEvents(QSharedPointer<Stroke> stroke);

    /// <summary>
    /// Detaches event handlers from stroke
    /// </summary>
    void StopListeningOnStrokeEvents(QSharedPointer<Stroke> stroke);

    /// <summary>
    /// Finds a container for a new visual based on the drawing attributes
    /// of the stroke rendered into that visual.
    /// </summary>
    /// <param name="drawingAttributes">drawing attributes</param>
    /// <returns>visual</returns>
    ContainerVisual* GetContainerVisual(QSharedPointer<DrawingAttributes> drawingAttributes);

    //#endregion

    //#region Fields

    // The renderer's top level container visuals
    ContainerVisual* _rootVisual;
    ContainerVisual* _highlightersRoot;
    ContainerVisual* _incrementalRenderingVisuals;
    ContainerVisual* _regularInkVisuals;

    // Stroke-to-visual map
    QMap<QSharedPointer<Stroke>, StrokeVisual*> _visuals;

    // Color-to-visual map for highlighter ink container visuals
    QMap<QColor, HighlighterContainerVisual*> _highlighters;

    // Collection of strokes this Renderer renders
    QSharedPointer<StrokeCollection> _strokes;

    // List of visuals attached via AttachIncrementalRendering
    QList<Visual*> _attachedVisuals;

    // Whhen true, will render in high contrast mode
    bool _highContrast;
    QColor _highContrastColor = Qt::white;

    //#endregion
};

#include "Windows/Media/containervisual.h"

class InkRenderer::HighlighterContainerVisual : public ContainerVisual
{
    Q_OBJECT
public:
    HighlighterContainerVisual(QColor color)
    {
        _color = color;
    }

    /// <summary>
    /// The Color of the strokes in this highlighter container visual
    /// </summary>
    QColor Color()
    {
        return _color;
    }
private:
    QColor _color;
};



#endif // INKRENDERER_H
