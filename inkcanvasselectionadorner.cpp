#include "inkcanvasselectionadorner.h"
#include "drawingcontext.h"
#include "inkcanvasinnercanvas.h"
#include "debug.h"
#include "inkcanvas.h"
#include "geometry.h"

#include <QPainterPath>

/// <summary>
/// Constructor
/// </summary>
/// <param name="adornedElement">The adorned InkCanvas</param>
InkCanvasSelectionAdorner::InkCanvasSelectionAdorner(UIElement* adornedElement)
    : Adorner(adornedElement)
    , _adornerBorderPen(Qt::black, 1.0)
    , _adornerPenBrush(QColor::fromRgb(132, 146, 222), 1.0)
{
    Debug::Assert(adornedElement->InstanceOf<InkCanvasInnerCanvas>(),
        "InkCanvasSelectionAdorner only should be used by InkCanvas internally");

    // Initialize the data.
    //_adornerBorderPen = new Pen(Brushes.Black, 1.0);
    QVector<double> dashes;
    dashes.append(4.5);
    dashes.append(4.5);
    _adornerBorderPen.setDashPattern(dashes);
    _adornerBorderPen.setDashOffset(2.25);
    _adornerBorderPen.setCapStyle(Qt::PenCapStyle::FlatCap);
    //_adornerBorderPen.Freeze();

    _adornerPenBrush = QPen(QColor::fromRgb(132, 146, 222), 1.0);
    //_adornerPenBrush.Freeze();

    QLinearGradient linear;
    linear.setColorAt(0, QColor::fromRgb(240, 242, 255));
    linear.setColorAt(1, QColor::fromRgb(180, 207, 248));
    _adornerFillBrush = QBrush(linear);
    //_adornerFillBrush = new LinearGradientBrush(  Color.FromRgb(240, 242, 255), //start color
    //                                Color.FromRgb(180, 207, 248),               //end color
    //                                45f                                         //angle
    //                                );
    //_adornerFillBrush.Freeze();

    // Create a hatch pen
    DrawingGroup hatchDG;
    DrawingContext* dc = nullptr;

    //try
    {
        dc = hatchDG.Open( );

        dc->DrawRectangle(
            QBrush(Qt::transparent),
            QPen(Qt::NoPen),
            QRectF(0.0, 0.0, 1, 1));

        QPen squareCapPen(Qt::black, LineThickness);
        squareCapPen.setCapStyle(Qt::SquareCap);
        //squareCapPen.EndLineCap = PenLineCap.Square;

        dc->DrawLine(squareCapPen,
            QPointF(1, 0), QPointF(0, 1));
    }
    //finally
    //{
    //    if ( dc != nullptr )
    //    {
    //        dc.Close( );
    //    }
    //}
    //hatchDG.Freeze();

    QBrush tileBrush;// = new DrawingBrush(hatchDG);
    //tileBrush.TileMode = TileMode.Tile;
    //tileBrush.Viewport = QRectF(0, 0, HatchBorderMargin, HatchBorderMargin);
    //tileBrush.ViewportUnits = BrushMappingMode.Absolute;
    //tileBrush.Freeze();

    _hatchPen = QPen(tileBrush, HatchBorderMargin);
    //_hatchPen.Freeze();

    //_elementsBounds = new List<Rect>();
    //_strokesBounds = Rect.Empty;
}

/// <summary>
/// SelectionHandleHitTest
/// </summary>
/// <param name="point"></param>
/// <returns></returns>
InkCanvasSelectionHitResult InkCanvasSelectionAdorner::SelectionHandleHitTest(QPointF const & point)
{
    InkCanvasSelectionHitResult result = InkCanvasSelectionHitResult::None;
    QRectF rectWireFrame = GetWireFrameRect();

    if ( !rectWireFrame.isEmpty() )
    {
        // Hit test on the grab handles first
        for ( InkCanvasSelectionHitResult hitResult = InkCanvasSelectionHitResult::TopLeft;
                hitResult <= InkCanvasSelectionHitResult::Left; hitResult = (InkCanvasSelectionHitResult)((int)hitResult + 1) )
        {
            QRectF toleranceRect;
            QRectF visibleRect;
            GetHandleRect(hitResult, rectWireFrame, visibleRect, toleranceRect);

            if (toleranceRect.contains(point))
            {
                result = hitResult;
                break;
            }
        }

        // Now, check if we hit on the frame
        if ( result == InkCanvasSelectionHitResult::None )
        {
            QRectF outterRect = rectWireFrame.adjusted(-CornerResizeHandleSize / 2, CornerResizeHandleSize / 2,
                                                     -CornerResizeHandleSize / 2, CornerResizeHandleSize / 2);
            if ( outterRect.contains(point) )
            {
                result = InkCanvasSelectionHitResult::Selection;

                //
            }
        }

    }

    return result;
}

/// <summary>
/// Update the selection wire frame.
/// Called by
///         InkCanvasSelection.UpdateSelectionAdorner
/// </summary>
/// <param name="strokesBounds"></param>
/// <param name="hatchBounds"></param>
void InkCanvasSelectionAdorner::UpdateSelectionWireFrame(QRectF const & strokesBounds, QList<QRectF> hatchBounds)
{
    bool isStrokeBoundsDifferent = false;
    bool isElementsBoundsDifferent = false;

    // Check if the strokes' bounds are changed.
    if ( _strokesBounds != strokesBounds )
    {
        _strokesBounds = strokesBounds;
        isStrokeBoundsDifferent = true;
    }

    // Check if the elements' bounds are changed.
    int count = hatchBounds.size();
    if ( count != _elementsBounds.size() )
    {
        isElementsBoundsDifferent = true;
    }
    else
    {
        for ( int i = 0; i < count; i++ )
        {
            if ( _elementsBounds[i] != hatchBounds[i] )
            {
                isElementsBoundsDifferent = true;
                break;
            }
        }
    }


    if ( isStrokeBoundsDifferent || isElementsBoundsDifferent )
    {
        if ( isElementsBoundsDifferent )
        {
            _elementsBounds = hatchBounds;
        }

        // Invalidate our visual since the selection is changed.
        InvalidateVisual();
    }
}

/// <summary>
/// OnRender
/// </summary>
/// <param name="drawingContext"></param>
void InkCanvasSelectionAdorner::OnRender(DrawingContext& drawingContext)
{
    // Draw the background and hatch border around the elements
    DrawBackgound(drawingContext);

    // Draw the selection frame.
    QRectF rectWireFrame = GetWireFrameRect();
    if ( !rectWireFrame.isEmpty() )
    {
        // Draw the wire frame.
        drawingContext.DrawRectangle(QBrush(),
            _adornerBorderPen,
            rectWireFrame);

        // Draw grab handles
        DrawHandles(drawingContext, rectWireFrame);
    }
}


/// <summary>
/// Draw Handles
/// </summary>
/// <param name="drawingContext"></param>
/// <param name="rectWireFrame"></param>
void InkCanvasSelectionAdorner::DrawHandles(DrawingContext& drawingContext, QRectF const & rectWireFrame)
{
    for ( InkCanvasSelectionHitResult hitResult = InkCanvasSelectionHitResult::TopLeft;
            hitResult <= InkCanvasSelectionHitResult::Left; (InkCanvasSelectionHitResult)((int)hitResult + 1))
    {
        // Draw the handle
        QRectF toleranceRect;
        QRectF visibleRect;
        GetHandleRect(hitResult, rectWireFrame, visibleRect, toleranceRect);

        drawingContext.DrawRectangle(_adornerFillBrush, _adornerPenBrush, visibleRect);
    }
}

/// <summary>
/// Draw the hatches and the transparent area where isn't covering the elements.
/// </summary>
/// <param name="drawingContext"></param>
void InkCanvasSelectionAdorner::DrawBackgound(DrawingContext& drawingContext)
{
    PathGeometry* hatchGeometry = nullptr;
    Geometry* rectGeometry = nullptr;

    int count = _elementsBounds.size();
    if ( count != 0 )
    {
        // Create a union collection of the element regions.
        for ( int i = 0; i < count; i++ )
        {
            QRectF hatchRect = _elementsBounds[i];

            if ( hatchRect.isEmpty() )
            {
                continue;
            }

            hatchRect.adjust(-HatchBorderMargin / 2, -HatchBorderMargin / 2, HatchBorderMargin / 2, HatchBorderMargin / 2);

            if ( hatchGeometry == nullptr )
            {
                //PathFigure path = new PathFigure();
                //path.StartPoint = QPointF(hatchRect.left(), hatchRect.top());

                QPainterPath path(QPointF(hatchRect.left(), hatchRect.top()));

                //PathSegmentCollection segments = new PathSegmentCollection();

                //PathSegment line = new LineSegment(new Point(hatchRect.right(), hatchRect.top()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(QPointF(hatchRect.right(), hatchRect.top()));

                //line = new LineSegment(new Point(hatchRect.right(), hatchRect.bottom()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(QPointF(hatchRect.right(), hatchRect.bottom()));

                //line = new LineSegment(new Point(hatchRect.left(), hatchRect.bottom()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(QPointF(hatchRect.left(), hatchRect.bottom()));

                //line = new LineSegment(new Point(hatchRect.left(), hatchRect.top()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(QPointF(hatchRect.left(), hatchRect.top()));

                //segments.Freeze();
                //path.Segments = segments;

                //path.IsClosed = true;
                //path.Freeze();
                path.closeSubpath();

                //hatchGeometry = new PathGeometry();
                //hatchGeometry.Figures.Add(path);
            }
            else
            {
                rectGeometry = new RectangleGeometry(hatchRect);
                //rectGeometry.Freeze();

                hatchGeometry = hatchGeometry->Combine(rectGeometry, GeometryCombineMode.Union, nullptr);
            }
        }
    }

    // Then, create a region which equals to "SelectionFrame - element1 bounds - element2 bounds - ..."
    GeometryGroup* backgroundGeometry = new GeometryGroup( );
    QList<Geometry*> geometryCollection;

    // Add the entile rectanlge to the group.
    rectGeometry = new RectangleGeometry(QRectF(0, 0, RenderSize().width(), RenderSize().height()));
    //rectGeometry.Freeze();
    geometryCollection.append(rectGeometry);

    // Add the union of the element rectangles. Then the group will do oddeven operation.
    Geometry* outlineGeometry = nullptr;

    if ( hatchGeometry != nullptr )
    {
        //hatchGeometry.Freeze();

        outlineGeometry = hatchGeometry->GetOutlinedPathGeometry();
        //outlineGeometry.Freeze();
        if ( count == 1 && ((InkCanvasInnerCanvas*)AdornedElement())->GetInkCanvas().GetSelectedStrokes()->size() == 0 )
        {
            geometryCollection.append(outlineGeometry);
        }
    }

    //geometryCollection.Freeze();
    backgroundGeometry->Children = geometryCollection;
    //backgroundGeometry.Freeze();

    // Then, draw the region which may contain holes so that the elements cannot be covered.
    // After that, the underneath elements can receive the messages.
#if DEBUG_OUTPUT
    // Draw the debug feedback
    drawingContext.DrawGeometry(new SolidColorBrush(Color.FromArgb(128, 255, 255, 0)), QPen(Qt::NoPen), *backgroundGeometry);
#else
    drawingContext.DrawGeometry(QBrush(Qt::transparent), QPen(Qt::NoPen), *backgroundGeometry);
#endif

    // At last, draw the hatch borders
    if ( outlineGeometry != nullptr )
    {
        drawingContext.DrawGeometry(QBrush(), _hatchPen, *outlineGeometry);
    }
}

/// <summary>
/// Returns the handle rect (both visibile and the tolerance one)
/// </summary>
void InkCanvasSelectionAdorner::GetHandleRect(InkCanvasSelectionHitResult hitResult, QRectF const & rectWireFrame, QRectF& visibleRect, QRectF& toleranceRect)
{
    QPointF center;
    double size = 0;
    double tolerance = ResizeHandleTolerance;

    switch ( hitResult )
    {
        case InkCanvasSelectionHitResult::TopLeft:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.left(), rectWireFrame.top());
                break;
            }
        case InkCanvasSelectionHitResult::Top:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.left() + rectWireFrame.width() / 2, rectWireFrame.top());
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::TopRight:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.right(), rectWireFrame.top());
                break;
            }
        case InkCanvasSelectionHitResult::Left:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.left(), rectWireFrame.top() + rectWireFrame.height() / 2);
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::Right:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.right(), rectWireFrame.top() + rectWireFrame.height() / 2);
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::BottomLeft:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.left(), rectWireFrame.bottom());
                break;
            }
        case InkCanvasSelectionHitResult::Bottom:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.left() + rectWireFrame.width() / 2, rectWireFrame.bottom());
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::BottomRight:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.right(), rectWireFrame.bottom());
                break;
            }
    }

    visibleRect = QRectF(center.x() - size / 2, center.y() - size / 2, size, size);
    toleranceRect = visibleRect;
    toleranceRect.adjust(-tolerance, -tolerance, tolerance, tolerance);
}

/// <summary>
/// Returns the wire frame bounds which crosses the center of the selection handles
/// </summary>
/// <returns></returns>
QRectF InkCanvasSelectionAdorner::GetWireFrameRect()
{
    QRectF frameRect;
    QRectF selectionRect = ((InkCanvasInnerCanvas*)AdornedElement())->GetInkCanvas().GetSelectionBounds();

    if ( !selectionRect.isEmpty() )
    {
        frameRect = selectionRect.adjusted(-BorderMargin, -BorderMargin, BorderMargin, BorderMargin);
    }

    return frameRect;
}

