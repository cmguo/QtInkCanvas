#include "Internal/Controls/inkcanvasfeedbackadorner.h"
#include "Internal/Controls/inkcanvasinnercanvas.h"
#include "Windows/Controls/inkcanvas.h"
#include "Internal/doubleutil.h"
#include "Windows/Media/drawingcontext.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

InkCanvasFeedbackAdorner::InkCanvasFeedbackAdorner()
    : Adorner (nullptr)
    , _inkCanvas(*static_cast<InkCanvas*>(nullptr))
{
}

/// <summary>
/// InkCanvasFeedbackAdorner Constructor
/// </summary>
/// <param name="inkCanvas">The adorned InkCanvas</param>
InkCanvasFeedbackAdorner::InkCanvasFeedbackAdorner(InkCanvas& inkCanvas)
    : Adorner(&inkCanvas.InnerCanvas())
    , _inkCanvas(inkCanvas)
{
    //if ( inkCanvas == null )
    //    throw std::exception("inkCanvas");

    // Initialize the internal data
    //_inkCanvas = inkCanvas;

    _adornerBorderPen = QPen(Qt::black, 1.0);
    QVector<double> dashes;
    dashes.append(4.5);
    dashes.append(4.5);
    _adornerBorderPen.setDashPattern(dashes);
    _adornerBorderPen.setDashOffset(2.25);
    _adornerBorderPen.setCapStyle(Qt::PenCapStyle::FlatCap);

}

/// <summary>
/// The overridden GetDesiredTransform method
/// </summary>
GeneralTransform InkCanvasFeedbackAdorner::GetDesiredTransform(GeneralTransform const &transform)
{
    //if ( transform == null )
    //{
    //    throw std::exception("transform");
    //}

    VerifyAccess();
    GeneralTransform desiredTransform = transform;

    // Check if we need translate the adorner.
    if ( !DoubleUtil::AreClose(_offsetX, 0) || !DoubleUtil::AreClose(_offsetY, 0) )
    {
        //desiredTransform *= QTransform::fromTranslate(_offsetX, _offsetY);
        desiredTransform.Translate(_offsetX, _offsetY);
    }

    return desiredTransform;
}

/// <summary>
/// The OnBoundsUpdated method
/// </summary>
/// <param name="rect"></param>
void InkCanvasFeedbackAdorner::OnBoundsUpdated(Rect const &rect)
{
    VerifyAccess();

    // Check if the rectangle has been changed.
    if ( rect != _previousRect )
    {
        Size newSize;
        double offsetX;
        double offsetY;
        bool invalidArrange = false;

        if ( !rect.IsEmpty() )
        {
            double offset = BorderMargin + CornerResizeHandleSize / 2;
            Rect adornerRect = Rect::Inflate(rect, offset, offset);

            newSize = adornerRect.GetSize();
            offsetX = adornerRect.Left();
            offsetY = adornerRect.Top();
        }
        else
        {
            newSize = Size(0, 0);
            offsetX = 0;
            offsetY = 0;
        }

        // Check if the size has been changed
        if ( _frameSize != newSize )
        {
            _frameSize = newSize;
            invalidArrange = true;
        }

        if ( !DoubleUtil::AreClose(_offsetX, offsetX) || !DoubleUtil::AreClose(_offsetY, offsetY) )
        {
            _offsetX = offsetX;
            _offsetY = offsetY;
            invalidArrange = true;
        }

        if ( invalidArrange )
        {
            InvalidateMeasure();
            InvalidateVisual(); //ensure re-rendering
            UIElement* parent = Parent();

            if ( parent != nullptr   )
            {
               Parent()->InvalidateArrange( );
               Arrange(Rect(_offsetX, _offsetY, _frameSize.Width(), _frameSize.Height()));
            }
        }

        _previousRect = rect;
    }
}

/// <summary>
/// The overridden MeasureOverride method
/// </summary>
/// <param name="constraint"></param>
Size InkCanvasFeedbackAdorner::MeasureOverride(Size)
{
    VerifyAccess();

    // return the frame size.
    return _frameSize;
}

/// <summary>
/// The overridden OnRender method
/// </summary>
/// <param name="drawingContext"></param>
void InkCanvasFeedbackAdorner::OnRender(DrawingContext& drawingContext)
{
    // No need to invoke VerifyAccess since this method calls DrawingContext.DrawRectangle.

    Debug::Assert(_frameSize != Size(0, 0));
    // Draw the wire frame.
    drawingContext.DrawRectangle(QBrush(), _adornerBorderPen,
        Rect(CornerResizeHandleSize / 2, CornerResizeHandleSize / 2,
            _frameSize.Width() - CornerResizeHandleSize, _frameSize.Height() - CornerResizeHandleSize));
}

/// <summary>
/// The method is called by InkCanvasSelection.UpdateFeedbackRect
/// </summary>
/// <param name="rect"></param>
void InkCanvasFeedbackAdorner::UpdateBounds(Rect const &rect)
{
    // Invoke OnBoundsUpdated.
    OnBoundsUpdated(rect);
}

INKCANVAS_END_NAMESPACE
