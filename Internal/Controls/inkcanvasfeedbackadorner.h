#ifndef INKCANVASFEEDBACKADORNER_H
#define INKCANVASFEEDBACKADORNER_H

#include "Windows/Controls/decorator.h"

class InkCanvas;
class DrawingContext;

#include <QPen>

// namespace MS.Internal.Controls

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
    virtual QTransform GetDesiredTransform(QTransform const & transform) override;

private:
    /// <summary>
    /// The OnBoundsUpdated method
    /// </summary>
    /// <param name="rect"></param>
    void OnBoundsUpdated(QRectF const &rect);

protected:
    /// <summary>
    /// The overridden MeasureOverride method
    /// </summary>
    /// <param name="constraint"></param>
    virtual QSizeF MeasureOverride(QSizeF constraint) override;

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
    void UpdateBounds(QRectF const &rect);

private:
    InkCanvas&   _inkCanvas;
    QSizeF        _frameSize;
    QRectF        _previousRect;
    double      _offsetX = 0;
    double      _offsetY = 0;

    QPen         _adornerBorderPen;

    static constexpr int       CornerResizeHandleSize = 8;
    static constexpr double    BorderMargin = 8;

};

#endif // INKCANVASFEEDBACKADORNER_H
