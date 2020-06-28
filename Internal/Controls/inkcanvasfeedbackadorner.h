#ifndef INKCANVASFEEDBACKADORNER_H
#define INKCANVASFEEDBACKADORNER_H

#include "Windows/Controls/decorator.h"

#include <QPen>

// namespace MS.Internal.Controls
INKCANVAS_BEGIN_NAMESPACE

class InkCanvas;
class DrawingContext;

/// <summary>
/// InkCanvasFeedbackAdorner
/// </summary>
class InkCanvasFeedbackAdorner : public Adorner
{
    Q_OBJECT
    // No default constructor
private:
    InkCanvasFeedbackAdorner();

public:
    /// <summary>
    /// InkCanvasFeedbackAdorner Constructor
    /// </summary>
    /// <param name="inkCanvas">The adorned InkCanvas</param>
    InkCanvasFeedbackAdorner(InkCanvas& inkCanvas);

    /// <summary>
    /// The overridden GetDesiredTransform method
    /// </summary>
    virtual GeneralTransform GetDesiredTransform(GeneralTransform const & transform) override;

private:
    /// <summary>
    /// The OnBoundsUpdated method
    /// </summary>
    /// <param name="rect"></param>
    void OnBoundsUpdated(Rect const &rect);

protected:
    /// <summary>
    /// The overridden MeasureOverride method
    /// </summary>
    /// <param name="constraint"></param>
    virtual Size MeasureOverride(Size constraint) override;

    /// <summary>
    /// The overridden OnRender method
    /// </summary>
    /// <param name="drawingContext"></param>
    virtual void OnRender(DrawingContext& drawingContext) override;

public:
    /// <summary>
    /// The method is called by InkCanvasSelection.UpdateFeedbackRect
    /// </summary>
    /// <param name="rect"></param>
    void UpdateBounds(Rect const &rect);

private:
    InkCanvas&   _inkCanvas;
    Size        _frameSize;
    Rect        _previousRect = Rect::Empty();
    double      _offsetX = 0;
    double      _offsetY = 0;

    QPen         _adornerBorderPen;

    static constexpr int       CornerResizeHandleSize = 8;
    static constexpr double    BorderMargin = 8;

};

INKCANVAS_END_NAMESPACE

#endif // INKCANVASFEEDBACKADORNER_H
