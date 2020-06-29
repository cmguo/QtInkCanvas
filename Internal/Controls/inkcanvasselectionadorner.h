#ifndef INKCANVASSELECTIONADORNER_H
#define INKCANVASSELECTIONADORNER_H

#include "Windows/Controls/inkcanvasselectionhitresult.h"
#include "Windows/Controls/decorator.h"
#include <Collections/Generic/list.h>
#include <Windows/rect.h>

#include <QPen>
#include <QBrush>

INKCANVAS_BEGIN_NAMESPACE

class DrawingContext;

// namespace MS.Internal.Controls

/// <summary>
/// InkCanvasSelectionAdorner
/// </summary>
class InkCanvasSelectionAdorner : public Adorner
{
    Q_OBJECT
public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="adornedElement">The adorned InkCanvas</param>
    InkCanvasSelectionAdorner(UIElement* adornedElement);

    /// <summary>
    /// SelectionHandleHitTest
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    InkCanvasSelectionHitResult SelectionHandleHitTest(Point const & point);

    /// <summary>
    /// Update the selection wire frame.
    /// Called by
    ///         InkCanvasSelection.UpdateSelectionAdorner
    /// </summary>
    /// <param name="strokesBounds"></param>
    /// <param name="hatchBounds"></param>
    void UpdateSelectionWireFrame(Rect const & strokesBounds, List<Rect> hatchBounds);

protected:
    /// <summary>
    /// OnRender
    /// </summary>
    /// <param name="drawingContext"></param>
    virtual void OnRender(DrawingContext& drawingContext);

private:
    /// <summary>
    /// Draw Handles
    /// </summary>
    /// <param name="drawingContext"></param>
    /// <param name="rectWireFrame"></param>
    void DrawHandles(DrawingContext& drawingContext, Rect const & rectWireFrame);

    /// <summary>
    /// Draw the hatches and the transparent area where isn't covering the elements.
    /// </summary>
    /// <param name="drawingContext"></param>
    void DrawBackgound(DrawingContext& drawingContext);

    /// <summary>
    /// Returns the handle rect (both visibile and the tolerance one)
    /// </summary>
    void GetHandleRect(InkCanvasSelectionHitResult hitResult, Rect const & rectWireFrame, Rect& visibleRect, Rect& toleranceRect);

    /// <summary>
    /// Returns the wire frame bounds which crosses the center of the selection handles
    /// </summary>
    /// <returns></returns>
    Rect GetWireFrameRect();

private:
    QPen             _adornerBorderPen;
    QPen             _adornerPenBrush;
    QBrush           _adornerFillBrush;
    QPen             _hatchPen;
    Rect             _strokesBounds = Rect::Empty();
    List<Rect>       _elementsBounds;

    // The buffer around the outside of this element
    static constexpr double    HatchBorderMargin = 6;
    static constexpr double    BorderMargin = HatchBorderMargin + 2;

    // Constants for Resize handles.
    static constexpr int       CornerResizeHandleSize = 8;
    static constexpr int       MiddleResizeHandleSize = 6;
    static constexpr double    ResizeHandleTolerance = 3;
    static constexpr double    LineThickness = 0.16;

};

INKCANVAS_END_NAMESPACE

#endif // INKCANVASSELECTIONADORNER_H
