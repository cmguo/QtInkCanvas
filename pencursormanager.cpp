#include "pencursormanager.h"
#include "geometry.h"
#include "stylusshape.h"
#include "styluspoint.h"
#include "styluspointcollection.h"
#include "drawingattributes.h"
#include "stroke.h"
#include "doubleutil.h"
#include "debug.h"
#include "drawing.h"
#include "drawingcontext.h"

#include <QBrush>
#include <QPen>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>
#include <QSize>

/// <summary>
/// Create a pen cursor from DrawingAttributes object
/// </summary>
/// <SecurityNote>
///     Critical: Call a SecurityCritical method - CreateCursorFromDrawing
///     TreatAsSafe: The method is safe because only safe arguments are passed to CreateCursorFromDrawing
///                  Also the size of the cursor will be limited to the half size of the current primary screen.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
QCursor PenCursorManager::GetPenCursor(QSharedPointer<DrawingAttributes> drawingAttributes, bool isHollow, bool isRightToLeft, double dpiScaleX, double dpiScaleY)
{
    // Create pen Drawing.
    std::unique_ptr<Drawing> penDrawing(CreatePenDrawing(drawingAttributes, isHollow, isRightToLeft, dpiScaleX, dpiScaleY));

    // Create Cursor from Drawing
    return CreateCursorFromDrawing(*penDrawing, QPointF(0, 0));
}

/// <summary>
/// Create a point eraser cursor from StylusShape
/// </summary>
/// <param name="stylusShape">Eraser Shape</param>
/// <param name="tranform">Transform</param>
/// <returns></returns>
QCursor PenCursorManager::GetPointEraserCursor(StylusShape& stylusShape, QMatrix tranform, double dpiScaleX, double dpiScaleY)
{
    Debug::Assert(DoubleUtil::IsZero(tranform.dx()) && DoubleUtil::IsZero(tranform.dy()), "The EraserShape cannot be translated.");
    Debug::Assert(tranform.isInvertible(), "The transform has to be invertable.");

    // Create a DA with IsHollow being set. A point eraser will be rendered to a hollow stroke.
    QSharedPointer<DrawingAttributes> da(new DrawingAttributes);
    if (stylusShape.metaObject() == &RectangleStylusShape::staticMetaObject)
    {
        da->SetStylusTip(StylusTip::Rectangle);
    }
    else
    {
        da->SetStylusTip(StylusTip::Ellipse);
    }

    da->SetHeight(stylusShape.Height());
    da->SetWidth(stylusShape.Width());
    da->SetColor(Qt::black);

    if ( !tranform.isIdentity() )
    {
        // Apply the LayoutTransform and/or RenderTransform
        da->SetStylusTipTransform(da->StylusTipTransform() * tranform);
    }

    if ( !DoubleUtil::IsZero(stylusShape.Rotation()) )
    {
        // Apply the tip rotation
        QMatrix rotationMatrix;
        rotationMatrix.rotate(stylusShape.Rotation());
        da->SetStylusTipTransform(da->StylusTipTransform() * rotationMatrix);
    }

    // Forward to GetPenCursor.
    return GetPenCursor(da, true, false/*isRightToLeft*/, dpiScaleX, dpiScaleY);
}

/// <summary>
/// Create a stroke eraser cursor
/// </summary>
/// <returns></returns>
/// <SecurityNote>
///     Critical: Call a SecurityCritical method - CreateCursorFromDrawing
///     TreatAsSafe: The method is safe because there is no input parameter.
///                  The erase cursor is created internally which is safe.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
QCursor PenCursorManager::GetStrokeEraserCursor()
{
    if ( s_StrokeEraserCursor == QCursor() )
    {
        // Get Drawing
        std::unique_ptr<Drawing> drawing(CreateStrokeEraserDrawing());
        s_StrokeEraserCursor = CreateCursorFromDrawing(*drawing, QPointF(5, 5));
    }

    // Return cursor.
    return s_StrokeEraserCursor;
}

/// <summary>
/// Retrieve selection cursor
/// </summary>
/// <param name="hitResult">hitResult</param>
/// <param name="isRightToLeft">True if InkCanvas.FlowDirection is RightToLeft, false otherwise</param>
/// <returns></returns>
QCursor PenCursorManager::GetSelectionCursor(InkCanvasSelectionHitResult hitResult, bool isRightToLeft)
{
    QCursor cursor;

    switch ( hitResult )
    {
        case InkCanvasSelectionHitResult::TopLeft:
        case InkCanvasSelectionHitResult::BottomRight:
            {
                if (isRightToLeft)
                {
                    cursor = Qt::SizeFDiagCursor;
                }
                else
                {
                    cursor = Qt::SizeBDiagCursor;
                }
                break;
            }

        case InkCanvasSelectionHitResult::Bottom:
        case InkCanvasSelectionHitResult::Top:
            {
                cursor = Qt::SizeVerCursor;
                break;
            }

        case InkCanvasSelectionHitResult::BottomLeft:
        case InkCanvasSelectionHitResult::TopRight:
            {
                if (isRightToLeft)
                {
                    cursor = Qt::SizeBDiagCursor;
                }
                else
                {
                    cursor = Qt::SizeFDiagCursor;
                }
                break;
            }

        case InkCanvasSelectionHitResult::Left:
        case InkCanvasSelectionHitResult::Right:
            {
                cursor = Qt::SizeVerCursor;
                break;
            }
        case InkCanvasSelectionHitResult::Selection:
            {
                cursor = Qt::SizeAllCursor;
                break;
            }
        default:
            {
                // By default, use the Cross cursor.
                cursor = Qt::CrossCursor;
                break;
            }
    }

    return cursor;
}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// Create a Cursor from a Drawing object
/// </summary>
/// <param name="drawing">Drawing</param>
/// <param name="hotspot">Cursor Hotspot</param>
/// <returns></returns>
/// <SecurityNote>
///     Critical: Critical as this code calls IconHelper.CreateIconCursor which is Critical
/// </SecurityNote>
//[SecurityCritical]
QCursor PenCursorManager::CreateCursorFromDrawing(Drawing& drawing, QPointF hotspot)
{
    // A default cursor.
    QCursor cursor = Qt::ArrowCursor;

    QRectF drawingBounds = drawing.Bounds();

    double originalWidth = drawingBounds.width();
    double originalHeight = drawingBounds.height();

    // Cursors like to be multiples of 8 in dimension.
    int width = AlignToBytes(drawingBounds.width(), 1);
    int height = AlignToBytes(drawingBounds.height(), 1);

    // Now inflate the drawing bounds to the new dimension.
    drawingBounds.adjust(-(width - originalWidth) / 2, -(height - originalHeight) / 2, (width - originalWidth) / 2, (height - originalHeight) / 2);

    // Translate the hotspot accordingly.
    int xHotspot = qRound(hotspot.x() - drawingBounds.left());
    int yHotspot = qRound(hotspot.y() - drawingBounds.top());

    // Create a DrawingVisual which represents the cursor drawing.
    //DrawingVisual cursorDrawingVisual = CreateCursorDrawingVisual(drawing, width, height);

    // Render the cursor visual to a bitmap
    //RenderTargetBitmap rtb = RenderVisualToBitmap(cursorDrawingVisual, width, height);

    // Get pixel data in Bgra32 fromat from the bitmap
    //byte[] pixels = GetPixels(rtb, width, height);

    //NativeMethods.IconHandle finalCursor = IconHelper.CreateIconCursor(pixels, width, height, xHotspot, yHotspot, false);

    //if ( finalCursor.IsInvalid )
    //{
        // Return the default cursor if above is failed.
    //    return Cursors.Arrow;
    //}

    //cursor = CursorInteropHelper.CriticalCreate(finalCursor);
    return cursor;
}

/// <summary>
/// Custom Pen Drawing
/// </summary>
Drawing* PenCursorManager::CreatePenDrawing(QSharedPointer<DrawingAttributes> drawingAttributes, bool isHollow, bool isRightToLeft, double dpiScaleX, double dpiScaleY)
{
    // Create a single point stroke.
    QSharedPointer<StylusPointCollection> stylusPoints(new StylusPointCollection());
    stylusPoints->append(StylusPoint(0, 0));

    QSharedPointer<DrawingAttributes> da(new DrawingAttributes);
    da->SetColor(drawingAttributes->Color());
    da->SetWidth(drawingAttributes->Width());
    da->SetHeight(drawingAttributes->Height());
    da->SetStylusTipTransform(drawingAttributes->StylusTipTransform());
    da->SetIsHighlighter(drawingAttributes->IsHighlighter());
    da->SetStylusTip(drawingAttributes->GetStylusTip());

    QSharedPointer<Stroke> singleStroke(new Stroke(stylusPoints, da));
    // NTRAID#WINDOWS-1326403-2005/10/03-waynezen,
    // We should draw our cursor in the device unit since it's device dependent object.
    singleStroke->GetDrawingAttributes()->SetWidth(ConvertToPixel(singleStroke->GetDrawingAttributes()->Width(), dpiScaleX));
    singleStroke->GetDrawingAttributes()->SetHeight(ConvertToPixel(singleStroke->GetDrawingAttributes()->Height(), dpiScaleY));

    QSize screenSize = QApplication::primaryScreen()->size();
    double maxLength = qMin(screenSize.width() / 2, screenSize.height() / 2);

    //
    // NOTE: there are two ways to set the width / height of a stroke
    // 1) using .Width and .Height
    // 2) using StylusTipTransform and specifying a scale
    // these two can multiply and we need to prevent the size from ever going
    // over maxLength or under 1.0.  The simplest way to check if we're too big
    // is by checking the bounds of the stroke, which takes both into account
    //
    QRectF strokeBounds = singleStroke->GetBounds();
    bool outOfBounds = false;

    // Make sure that the cursor won't exceed the minimum or the maximum boundary.
    if ( DoubleUtil::LessThan(strokeBounds.width(), 1.0) )
    {
        singleStroke->GetDrawingAttributes()->SetWidth(1.0);
        outOfBounds = true;
    }
    else if ( DoubleUtil::GreaterThan(strokeBounds.width(), maxLength) )
    {
        singleStroke->GetDrawingAttributes()->SetWidth(maxLength);
        outOfBounds = true;
    }

    if ( DoubleUtil::LessThan(strokeBounds.height(), 1.0) )
    {
        singleStroke->GetDrawingAttributes()->SetHeight(1.0);
        outOfBounds = true;
    }
    else if ( DoubleUtil::GreaterThan(strokeBounds.height(), maxLength) )
    {
        singleStroke->GetDrawingAttributes()->SetHeight(maxLength);
        outOfBounds = true;
    }

    //drop the StylusTipTransform if we're out of bounds.  we might
    //consider trying to preserve any transform but this is such a rare
    //case (scaling over or under with a STT) that we don't care.
    if (outOfBounds)
    {
        singleStroke->GetDrawingAttributes()->SetStylusTipTransform(QMatrix());
    }

    if (isRightToLeft)
    {
        //reverse left to right to right to left
        QMatrix xf = singleStroke->GetDrawingAttributes()->StylusTipTransform();
        xf.scale(-1, 1);

        //only set the xf if it has an inverse or the STT will throw
        if (xf.isInvertible())
        {
            singleStroke->GetDrawingAttributes()->SetStylusTipTransform(xf);
        }
    }

    DrawingGroup* penDrawing = new DrawingGroup();
    DrawingContext* dc = nullptr;

    //try
    {
        dc = penDrawing->Open();

        // Call the drawing method on Stroke to draw as hollow if isHollow == true
        if ( isHollow )
        {
            singleStroke->DrawInternal(*dc, singleStroke->GetDrawingAttributes(), isHollow);
        }
        else
        {
            // Invoke the public Draw method which will handle the Highlighter correctly.
            singleStroke->Draw(*dc, singleStroke->GetDrawingAttributes());
        }
    }
    //finally
    {
        if ( dc != nullptr )
        {
            dc->Close();
        }
    }

    return penDrawing;
}

/// <summary>
/// Custom StrokeEraser Drawing
/// </summary>
/// <returns></returns>
Drawing* PenCursorManager::CreateStrokeEraserDrawing()
{
    DrawingGroup* drawingGroup = new DrawingGroup();
    DrawingContext* dc = nullptr;

    //try
    {
        dc = drawingGroup->Open();
        //LinearGradientBrush brush1 = new LinearGradientBrush(
        //                                    Color.FromRgb(240, 242, 255),   // Start Color
        //                                    Color.FromRgb(180, 207, 248),   // End Color
        //                                    45f                             // Angle
        //                                    );
        //brush1.Freeze();
        QLinearGradient linear;
        linear.setColorAt(0, QColor::fromRgb(240, 242, 255));
        linear.setColorAt(1, QColor::fromRgb(180, 207, 248));
        QBrush brush1(linear);

        QBrush brush2(QColor::fromRgb(180, 207, 248));
        //brush2.Freeze();

        QPen pen1 (Qt::gray, 0.7);
        //pen1.Freeze();

        PathGeometry* pathGeometry = new PathGeometry;

        //PathFigure path = new PathFigure();
        //path.StartPoint = QPointF(5, 5);
        QPainterPath path(QPointF(5, 5));

        //LineSegment segment = new LineSegment(QPointF(16, 5), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(16, 5));

        //segment = new LineSegment(QPointF(26, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(26, 15));

        //segment = new LineSegment(QPointF(15, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(15, 15));

        //segment = new LineSegment(QPointF(5, 5), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(5, 5));

        //path.IsClosed = true;
        //path.Freeze();
        path.closeSubpath();

        pathGeometry->Add(path);

        //path = new PathFigure();
        //path.StartPoint = QPointF(5, 5);
        path = QPainterPath(QPointF(5, 5));

        //segment = new LineSegment(QPointF(5, 10), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(5, 10));

        //segment = new LineSegment(QPointF(15, 19), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(15, 19));

        //segment = new LineSegment(QPointF(15, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(15, 15));

        //segment = new LineSegment(QPointF(5, 5), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        //path.IsClosed = true;
        //path.Freeze();
        path.lineTo(QPointF(5, 5));
        path.closeSubpath();

        pathGeometry->Add(path);
        //pathGeometry.Freeze();

        PathGeometry* pathGeometry1 = new PathGeometry;
        //path = new PathFigure();
        //path.StartPoint = QPointF(15, 15);
        path = QPainterPath(QPointF(15, 15));

        //segment = new LineSegment(QPointF(15, 19), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(15, 19));

        //segment = new LineSegment(QPointF(26, 19), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(26, 19));

        //segment = new LineSegment(QPointF(26, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(QPointF(26, 15));
        //segment.Freeze();
        //segment = new LineSegment(QPointF(15, 15), true);

        //path.Segments.Add(segment);
        //path.IsClosed = true;
        //path.Freeze();
        path.lineTo(QPointF(15, 15));
        path.closeSubpath();

        pathGeometry1->Add(path);
        //pathGeometry1.Freeze();

        dc->DrawGeometry(brush1, pen1, pathGeometry);
        dc->DrawGeometry(brush2, pen1, pathGeometry1);
        dc->DrawLine(pen1, QPointF(5, 5), QPointF(5, 0));
        dc->DrawLine(pen1, QPointF(5, 5), QPointF(0, 5));
        dc->DrawLine(pen1, QPointF(5, 5), QPointF(2, 2));
        dc->DrawLine(pen1, QPointF(5, 5), QPointF(8, 2));
        dc->DrawLine(pen1, QPointF(5, 5), QPointF(2, 8));
    }
    //finally
    {
        if ( dc != nullptr )
        {
            dc->Close();
        }
    }

    return drawingGroup;
}

// Also used by PenCursorManager
/// <summary>
/// Calculate the bits count aligned to N-Byte based on the input count
/// </summary>
/// <param name="original">The original value</param>
/// <param name="nBytesCount">N-Byte</param>
/// <returns>the nearest bit count which is aligned to N-Byte</returns>
int PenCursorManager::AlignToBytes(double original, int nBytesCount)
{
    Debug::Assert(nBytesCount > 0, "The N-Byte has to be greater than 0!");

    int nBitsCount = 8 << (nBytesCount - 1);
    return ((ceil(original) + (nBitsCount - 1)) / nBitsCount) * nBitsCount;
}

QCursor PenCursorManager::s_StrokeEraserCursor;

