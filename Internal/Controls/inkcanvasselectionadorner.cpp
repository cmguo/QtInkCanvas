#include "Internal/Controls/inkcanvasselectionadorner.h"
#include "Windows/Media/drawingcontext.h"
#include "Internal/Controls/inkcanvasinnercanvas.h"
#include "Windows/Controls/inkcanvas.h"
#include "Windows/Media/geometry.h"
#include "Windows/Media/drawing.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QPainterPath>

INKCANVAS_BEGIN_NAMESPACE

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
    std::unique_ptr<DrawingContext> dc = nullptr;


    //try
    {
        FinallyHelper final([&dc]() {
            if ( dc != nullptr )
            {
                dc->Close( );
            }
        });

        dc.reset(hatchDG.Open( ));

        dc->DrawRectangle(
            QBrush(Qt::transparent),
            QPen(Qt::NoPen),
            Rect(0.0, 0.0, 1, 1));

        QPen squareCapPen(Qt::black, LineThickness);
        squareCapPen.setCapStyle(Qt::SquareCap);
        //squareCapPen.EndLineCap = PenLineCap.Square;

        dc->DrawLine(squareCapPen,
            Point(1, 0), Point(0, 1));
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
    //tileBrush.Viewport = Rect(0, 0, HatchBorderMargin, HatchBorderMargin);
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
InkCanvasSelectionHitResult InkCanvasSelectionAdorner::SelectionHandleHitTest(Point const & point)
{
    InkCanvasSelectionHitResult result = InkCanvasSelectionHitResult::None;
    Rect rectWireFrame = GetWireFrameRect();

    if ( !rectWireFrame.IsEmpty() )
    {
        // Hit test on the grab handles first
        for ( InkCanvasSelectionHitResult hitResult = InkCanvasSelectionHitResult::TopLeft;
                hitResult <= InkCanvasSelectionHitResult::Left; hitResult = (InkCanvasSelectionHitResult)((int)hitResult + 1) )
        {
            Rect toleranceRect;
            Rect visibleRect;
            GetHandleRect(hitResult, rectWireFrame, visibleRect, toleranceRect);

            if (toleranceRect.Contains(point))
            {
                result = hitResult;
                break;
            }
        }

        // Now, check if we hit on the frame
        if ( result == InkCanvasSelectionHitResult::None )
        {
            Rect outterRect = Rect::Inflate(rectWireFrame, -CornerResizeHandleSize / 2, CornerResizeHandleSize / 2);
            if ( outterRect.Contains(point) )
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
void InkCanvasSelectionAdorner::UpdateSelectionWireFrame(Rect const & strokesBounds, List<Rect> hatchBounds)
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
    int count = hatchBounds.Count();
    if ( count != _elementsBounds.Count() )
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
    Rect rectWireFrame = GetWireFrameRect();
    if ( !rectWireFrame.IsEmpty() )
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
void InkCanvasSelectionAdorner::DrawHandles(DrawingContext& drawingContext, Rect const & rectWireFrame)
{
    for ( InkCanvasSelectionHitResult hitResult = InkCanvasSelectionHitResult::TopLeft;
            hitResult <= InkCanvasSelectionHitResult::Left; hitResult = (InkCanvasSelectionHitResult)((int)hitResult + 1))
    {
        // Draw the handle
        Rect toleranceRect;
        Rect visibleRect;
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

    int count = _elementsBounds.Count();
    if ( count != 0 )
    {
        // Create a union collection of the element regions.
        for ( int i = 0; i < count; i++ )
        {
            Rect hatchRect = _elementsBounds[i];

            if ( hatchRect.IsEmpty() )
            {
                continue;
            }

            hatchRect.Inflate(HatchBorderMargin / 2, HatchBorderMargin / 2);

            if ( hatchGeometry == nullptr )
            {
                //PathFigure path = new PathFigure();
                //path.StartPoint = new Point(hatchRect.Left, hatchRect.Top);

                QPainterPath path(hatchRect.TopLeft());

                //PathSegmentCollection segments = new PathSegmentCollection();

                //PathSegment line = new LineSegment(new Point(hatchRect.right(), hatchRect.top()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(hatchRect.TopRight());

                //line = new LineSegment(new Point(hatchRect.right(), hatchRect.bottom()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(hatchRect.BottomRight());

                //line = new LineSegment(new Point(hatchRect.left(), hatchRect.bottom()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(hatchRect.BottomLeft());

                //line = new LineSegment(new Point(hatchRect.left(), hatchRect.top()), true);
                //line.Freeze();
                //segments.Add(line);
                path.lineTo(hatchRect.TopLeft());

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

                hatchGeometry = hatchGeometry->Combine(rectGeometry);
            }
        }
    }

    // Then, create a region which equals to "SelectionFrame - element1 bounds - element2 bounds - ..."
    GeometryGroup* backgroundGeometry = new GeometryGroup( );
    List<Geometry*> geometryCollection;

    // Add the entile rectanlge to the group.
    rectGeometry = new RectangleGeometry(Rect(0, 0, RenderSize().Width(), RenderSize().Height()));
    //rectGeometry.Freeze();
    geometryCollection.Add(rectGeometry);

    // Add the union of the element rectangles. Then the group will do oddeven operation.
    Geometry* outlineGeometry = nullptr;

    if ( hatchGeometry != nullptr )
    {
        //hatchGeometry.Freeze();

        outlineGeometry = hatchGeometry->GetOutlinedPathGeometry();
        //outlineGeometry.Freeze();
        if ( count == 1 && qobject_cast<InkCanvasInnerCanvas*>(AdornedElement())->GetInkCanvas().GetSelectedStrokes()->Count() == 0 )
        {
            geometryCollection.Add(outlineGeometry);
        }
    }

    //geometryCollection.Freeze();
    backgroundGeometry->Children() = geometryCollection;
    //backgroundGeometry.Freeze();

    // Then, draw the region which may contain holes so that the elements cannot be covered.
    // After that, the underneath elements can receive the messages.
#if DEBUG_OUTPUT
    // Draw the debug feedback
    drawingContext.DrawGeometry(QBrush(QColor::fromRgba(255, 255, 0, 128)), Qt::NoPen, backgroundGeometry);
#else
    drawingContext.DrawGeometry(QBrush(Qt::transparent), Qt::NoPen, backgroundGeometry);
#endif

    // At last, draw the hatch borders
    if ( outlineGeometry != nullptr )
    {
        drawingContext.DrawGeometry(QBrush(), _hatchPen, outlineGeometry);
    }
}

/// <summary>
/// Returns the handle rect (both visibile and the tolerance one)
/// </summary>
void InkCanvasSelectionAdorner::GetHandleRect(InkCanvasSelectionHitResult hitResult, Rect const & rectWireFrame, Rect& visibleRect, Rect& toleranceRect)
{
    QPointF center;
    double size = 0;
    double tolerance = ResizeHandleTolerance;

    switch ( hitResult )
    {
        case InkCanvasSelectionHitResult::TopLeft:
            {
                size = CornerResizeHandleSize;
                center = rectWireFrame.TopLeft();
                break;
            }
        case InkCanvasSelectionHitResult::Top:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.Left() + rectWireFrame.Width() / 2, rectWireFrame.Top());
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::TopRight:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.Right(), rectWireFrame.Top());
                break;
            }
        case InkCanvasSelectionHitResult::Left:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.Left(), rectWireFrame.Top() + rectWireFrame.Height() / 2);
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::Right:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.Right(), rectWireFrame.Top() + rectWireFrame.Height() / 2);
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::BottomLeft:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.Left(), rectWireFrame.Bottom());
                break;
            }
        case InkCanvasSelectionHitResult::Bottom:
            {
                size = MiddleResizeHandleSize;
                center = QPointF(rectWireFrame.Left() + rectWireFrame.Width() / 2, rectWireFrame.Bottom());
                tolerance = (CornerResizeHandleSize - MiddleResizeHandleSize) + ResizeHandleTolerance;
                break;
            }
        case InkCanvasSelectionHitResult::BottomRight:
            {
                size = CornerResizeHandleSize;
                center = QPointF(rectWireFrame.Right(), rectWireFrame.Bottom());
                break;
            }
    }

    visibleRect = Rect(center.x() - size / 2, center.y() - size / 2, size, size);
    toleranceRect = visibleRect;
    toleranceRect.Inflate(tolerance, tolerance);
}

/// <summary>
/// Returns the wire frame bounds which crosses the center of the selection handles
/// </summary>
/// <returns></returns>
Rect InkCanvasSelectionAdorner::GetWireFrameRect()
{
    Rect frameRect = Rect::Empty();
    Rect selectionRect = qobject_cast<InkCanvasInnerCanvas*>(AdornedElement())->GetInkCanvas().GetSelectionBounds();

    if ( !selectionRect.IsEmpty() )
    {
        frameRect = Rect::Inflate(selectionRect, BorderMargin, BorderMargin);
    }

    return frameRect;
}

INKCANVAS_END_NAMESPACE
