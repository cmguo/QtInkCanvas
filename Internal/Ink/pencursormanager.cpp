#include "Internal/Ink/pencursormanager.h"
#include "Windows/Media/geometry.h"
#include "Windows/Ink/stylusshape.h"
#include "Windows/Input/styluspoint.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/stroke.h"
#include "Internal/doubleutil.h"
#include "Windows/Media/drawing.h"
#include "Windows/Media/drawingcontext.h"
#include "Windows/Media/drawingvisual.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QBrush>
#include <QPen>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>
#include <QSize>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

INKCANVAS_BEGIN_NAMESPACE

QSvgRenderer PenCursorManager::EraserImage;

/// <summary>
/// Create a pen cursor from DrawingAttributes object
/// </summary>
/// <SecurityNote>
///     Critical: Call a SecurityCritical method - CreateCursorFromDrawing
///     TreatAsSafe: The method is safe because only safe arguments are passed to CreateCursorFromDrawing
///                  Also the size of the cursor will be limited to the half size of the current primary screen.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe]
QCursor PenCursorManager::GetPenCursor(SharedPointer<DrawingAttributes> drawingAttributes, bool isHollow, bool isRightToLeft, double dpiScaleX, double dpiScaleY)
{
    // Create pen Drawing.
    std::unique_ptr<Drawing> penDrawing(CreatePenDrawing(drawingAttributes, isHollow, isRightToLeft, dpiScaleX, dpiScaleY));

    // Create Cursor from Drawing
    return CreateCursorFromDrawing(*penDrawing, Point(0, 0));
}

/// <summary>
/// Create a point eraser cursor from StylusShape
/// </summary>
/// <param name="stylusShape">Eraser Shape</param>
/// <param name="tranform">Transform</param>
/// <returns></returns>
QCursor PenCursorManager::GetPointEraserCursor(StylusShape& stylusShape, Matrix const & tranform, double dpiScaleX, double dpiScaleY)
{
    // Create a DA with IsHollow being set. A point eraser will be rendered to a hollow stroke.
    SharedPointer<DrawingAttributes> da(new DrawingAttributes);
    if (stylusShape.IsPolygon())
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

    if ( !tranform.IsIdentity() )
    {
        // Apply the LayoutTransform and/or RenderTransform
        da->SetStylusTipTransform(da->StylusTipTransform() * tranform);
    }

    if ( !DoubleUtil::IsZero(stylusShape.Rotation()) )
    {
        // Apply the tip rotation
        Matrix rotationMatrix;
        rotationMatrix.Rotate(stylusShape.Rotation());
        da->SetStylusTipTransform(da->StylusTipTransform() * rotationMatrix);
    }

    // Forward to GetPenCursor.
    return GetPenCursor(da, true, false/*isRightToLeft*/, dpiScaleX, dpiScaleY);
}

QCursor PenCursorManager::GetPointEraserCursor1(double, double)
{
    static QPixmap eraserPixmap(":/inkcanvas/erasercursor.svg");
    static QCursor eraserCursor(eraserPixmap);
    return eraserCursor;
}

QCursor PenCursorManager::GetPointEraserCursor2(StylusShape& stylusShape, Matrix const &tranform, double, double)
{
    Debug::Assert(DoubleUtil::IsZero(tranform.OffsetX()) && DoubleUtil::IsZero(tranform.OffsetY()), "The EraserShape cannot be translated.");
    Debug::Assert(tranform.HasInverse(), "The transform has to be invertable.");

    QPolygonF polygon;
    for (Vector const & p : stylusShape.GetVerticesAsVectors())
        polygon.append((Point)p);
    Rect rect = polygon.boundingRect();
    std::unique_ptr<Drawing> eraserDrawing(GetEraserImage(tranform.Transform(rect)));
    return CreateCursorFromDrawing(*eraserDrawing, Point(0, 0));
}

Drawing* PenCursorManager::GetEraserImage(Rect const & bound)
{
    if (!EraserImage.isValid())
        EraserImage.load(QString(":/inkcanvas/eraser.svg"));
    std::unique_ptr<SvgImageDrawing> drawing(new SvgImageDrawing);
    drawing->SetImageSource(&EraserImage);
    drawing->SetRect(bound);
    return drawing.release();
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
    static QCursor s_StrokeEraserCursor;
    if ( s_StrokeEraserCursor == QCursor() )
    {
        // Get Drawing
        std::unique_ptr<Drawing> drawing(CreateStrokeEraserDrawing());
        s_StrokeEraserCursor = CreateCursorFromDrawing(*drawing, Point(5, 5));
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
                    cursor = Qt::SizeBDiagCursor;
                }
                else
                {
                    cursor = Qt::SizeFDiagCursor;
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
                    cursor = Qt::SizeFDiagCursor;
                }
                else
                {
                    cursor = Qt::SizeBDiagCursor;
                }
                break;
            }

        case InkCanvasSelectionHitResult::Left:
        case InkCanvasSelectionHitResult::Right:
            {
                cursor = Qt::SizeHorCursor;
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
QCursor PenCursorManager::CreateCursorFromDrawing(Drawing& drawing, Point const & hotspot)
{
    // A default cursor.
    QCursor cursor = Qt::ArrowCursor;

    Rect drawingBounds = drawing.Bounds();

    double originalWidth = drawingBounds.Width();
    double originalHeight = drawingBounds.Height();

    // Cursors like to be multiples of 8 in dimension.
    int width = AlignToBytes(drawingBounds.Width(), 1);
    int height = AlignToBytes(drawingBounds.Height(), 1);

    // Now inflate the drawing bounds to the new dimension.
    drawingBounds.Inflate((width - originalWidth) / 2, (height - originalHeight) / 2);

    // Translate the hotspot accordingly.
    int xHotspot = qRound(hotspot.X() - drawingBounds.Left());
    int yHotspot = qRound(hotspot.Y() - drawingBounds.Top());

    // Create a DrawingVisual which represents the cursor drawing.
    std::unique_ptr<DrawingVisual> cursorDrawingVisual(CreateCursorDrawingVisual(drawing, width, height));

    // Render the cursor visual to a bitmap
    //RenderTargetBitmap rtb = RenderVisualToBitmap(cursorDrawingVisual, width, height);
    QPixmap rtb(width, height);
    rtb.fill(Qt::transparent);
    QPainter painter(&rtb);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QStyleOptionGraphicsItem style;
    cursorDrawingVisual->paint(&painter, &style);

    // Get pixel data in Bgra32 fromat from the bitmap
    //byte[] pixels = GetPixels(rtb, width, height);

    //NativeMethods.IconHandle finalCursor = IconHelper.CreateIconCursor(pixels, width, height, xHotspot, yHotspot, false);

    //if ( finalCursor.IsInvalid )
    //{
        // Return the default cursor if above is failed.
    //    return Cursors.Arrow;
    //}

    //cursor = CursorInteropHelper.CriticalCreate(finalCursor);
    cursor = QCursor(rtb, xHotspot, yHotspot);
    return cursor;
}

class DrawingBrush : public Drawing
{
public:
    DrawingBrush(Drawing* d, Rect const & r) : d_(d), r_(r) {}

    virtual Rect Bounds()
    {
        return r_;
    }

    virtual void Draw(QPainter& painer)
    {
        painer.translate(QRectF(r_).center());
        d_->Draw(painer);
    }

private:
    Drawing* d_;
    Rect r_;
};

/// <summary>
/// Create a DrawingVisual from a Drawing
/// </summary>
/// <param name="drawing"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
DrawingVisual* PenCursorManager::CreateCursorDrawingVisual(Drawing& drawing, int width, int height)
{
    // Create a drawing brush with the drawing as its content.
    //DrawingBrush db = new DrawingBrush(drawing);
    //db.Stretch = Stretch.None;
    //db.AlignmentX = AlignmentX.Center;
    //db.AlignmentY = AlignmentY.Center;

    //Rect bound(drawing.Bounds());
    //QPixmap rtb(bound.size().toSize());
    //QPainter painter(&rtb);
    //painter.translate(-bound.topLeft());
    //drawing.Draw(painter);
    //QBrush db(rtb);

    // Create a drawing visual with our drawing brush.
    std::unique_ptr<DrawingVisual> drawingVisual(new DrawingVisual());
    std::unique_ptr<DrawingContext> dc = nullptr;
    //try
    {
        FinallyHelper final([&dc]() {
            if ( dc != nullptr )
            {
                dc->Close();
            }
        });
        dc.reset(drawingVisual->RenderOpen());
        //dc->DrawRectangle(db, Qt::NoPen, Rect(0, 0, width, height));
        dc->DrawDrawing(new DrawingBrush(&drawing, Rect(0, 0, width, height)));
    }
    //finally
    //{
    //    if ( dc != null )
    //    {
    //        dc.Close();
    //    }
    //}
    //drawingVisual->setAttribute(Qt::WA_TranslucentBackground);
    return drawingVisual.release();
}

/// <summary>
/// Custom Pen Drawing
/// </summary>
Drawing* PenCursorManager::CreatePenDrawing(SharedPointer<DrawingAttributes> drawingAttributes, bool isHollow, bool isRightToLeft, double dpiScaleX, double dpiScaleY)
{
    // Create a single point stroke.
    SharedPointer<StylusPointCollection> stylusPoints(new StylusPointCollection());
    StylusPoint point(0, 0);
    stylusPoints->Add(point);

    SharedPointer<DrawingAttributes> da(new DrawingAttributes);
    da->SetColor(drawingAttributes->Color());
    da->SetWidth(drawingAttributes->Width());
    da->SetHeight(drawingAttributes->Height());
    da->SetStylusTipTransform(drawingAttributes->StylusTipTransform());
    da->SetIsHighlighter(drawingAttributes->IsHighlighter());
    da->SetStylusTip(drawingAttributes->GetStylusTip());

    SharedPointer<Stroke> singleStroke(new Stroke(stylusPoints, da));
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
    Rect strokeBounds = singleStroke->GetBounds();
    bool outOfBounds = false;

    // Make sure that the cursor won't exceed the minimum or the maximum boundary.
    if ( DoubleUtil::LessThan(strokeBounds.Width(), 1.0) )
    {
        singleStroke->GetDrawingAttributes()->SetWidth(1.0);
        outOfBounds = true;
    }
    else if ( DoubleUtil::GreaterThan(strokeBounds.Width(), maxLength) )
    {
        singleStroke->GetDrawingAttributes()->SetWidth(maxLength);
        outOfBounds = true;
    }

    if ( DoubleUtil::LessThan(strokeBounds.Height(), 1.0) )
    {
        singleStroke->GetDrawingAttributes()->SetHeight(1.0);
        outOfBounds = true;
    }
    else if ( DoubleUtil::GreaterThan(strokeBounds.Height(), maxLength) )
    {
        singleStroke->GetDrawingAttributes()->SetHeight(maxLength);
        outOfBounds = true;
    }

    //drop the StylusTipTransform if we're out of bounds.  we might
    //consider trying to preserve any transform but this is such a rare
    //case (scaling over or under with a STT) that we don't care.
    if (outOfBounds)
    {
        singleStroke->GetDrawingAttributes()->SetStylusTipTransform(Matrix());
    }

    if (isRightToLeft)
    {
        //reverse left to right to right to left
        Matrix xf = singleStroke->GetDrawingAttributes()->StylusTipTransform();
        xf.Scale(-1, 1);

        //only set the xf if it has an inverse or the STT will throw
        if (xf.HasInverse())
        {
            singleStroke->GetDrawingAttributes()->SetStylusTipTransform(xf);
        }
    }

    std::unique_ptr<DrawingGroup> penDrawing(new DrawingGroup());
    std::unique_ptr<DrawingContext> dc = nullptr;

    //try
    {
        FinallyHelper final([&dc]() {
            if ( dc != nullptr )
            {
                dc->Close();
            }
        });
        dc.reset(penDrawing->Open());

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
        if (strokeBounds.Width() < 8) {
            QPen p(da->Color(), 2);
            dc->DrawLine(p, {-8, 0}, {-strokeBounds.Width() - 1, 0});
            dc->DrawLine(p, {8, 0}, {strokeBounds.Width() + 1, 0});
            dc->DrawLine(p, {0, -8}, {0, -strokeBounds.Height() - 1});
            dc->DrawLine(p, {0, 8}, {0, strokeBounds.Height() + 1});
        }
    }
    //finally
    //{
    //    if ( dc != nullptr )
    //    {
    //        dc->Close();
    //    }
    //}
    singleStroke->releaseGeometry();
    return penDrawing.release();
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
        //path.StartPoint = Point(5, 5);
        QPainterPath path(Point(5, 5));

        //LineSegment segment = new LineSegment(Point(16, 5), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(16, 5));

        //segment = new LineSegment(Point(26, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(26, 15));

        //segment = new LineSegment(Point(15, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(15, 15));

        //segment = new LineSegment(Point(5, 5), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(5, 5));

        //path.IsClosed = true;
        //path.Freeze();
        path.closeSubpath();

        pathGeometry->Add(path);

        //path = new PathFigure();
        //path.StartPoint = Point(5, 5);
        path = QPainterPath(Point(5, 5));

        //segment = new LineSegment(Point(5, 10), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(5, 10));

        //segment = new LineSegment(Point(15, 19), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(15, 19));

        //segment = new LineSegment(Point(15, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(15, 15));

        //segment = new LineSegment(Point(5, 5), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        //path.IsClosed = true;
        //path.Freeze();
        path.lineTo(Point(5, 5));
        path.closeSubpath();

        pathGeometry->Add(path);
        //pathGeometry.Freeze();

        PathGeometry* pathGeometry1 = new PathGeometry;
        //path = new PathFigure();
        //path.StartPoint = Point(15, 15);
        path = QPainterPath(Point(15, 15));

        //segment = new LineSegment(Point(15, 19), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(15, 19));

        //segment = new LineSegment(Point(26, 19), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(26, 19));

        //segment = new LineSegment(Point(26, 15), true);
        //segment.Freeze();
        //path.Segments.Add(segment);
        path.lineTo(Point(26, 15));
        //segment.Freeze();
        //segment = new LineSegment(Point(15, 15), true);

        //path.Segments.Add(segment);
        //path.IsClosed = true;
        //path.Freeze();
        path.lineTo(Point(15, 15));
        path.closeSubpath();

        pathGeometry1->Add(path);
        //pathGeometry1.Freeze();

        dc->DrawGeometry(brush1, pen1, pathGeometry);
        dc->DrawGeometry(brush2, pen1, pathGeometry1);
        dc->DrawLine(pen1, Point(5, 5), Point(5, 0));
        dc->DrawLine(pen1, Point(5, 5), Point(0, 5));
        dc->DrawLine(pen1, Point(5, 5), Point(2, 2));
        dc->DrawLine(pen1, Point(5, 5), Point(8, 2));
        dc->DrawLine(pen1, Point(5, 5), Point(2, 8));
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
    return ((static_cast<int>(ceil(original)) + (nBitsCount - 1)) / nBitsCount) * nBitsCount;
}

QCursor PenCursorManager::s_StrokeEraserCursor;

INKCANVAS_END_NAMESPACE
