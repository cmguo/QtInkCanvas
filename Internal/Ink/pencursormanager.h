#ifndef PENCURSORMANAGER_H
#define PENCURSORMANAGER_H

#include "Windows/Controls/inkcanvasselectionhitresult.h"
#include "Windows/Media/matrix.h"
#include "sharedptr.h"

#include <QCursor>
#include <QtSvg/QSvgRenderer>

INKCANVAS_BEGIN_NAMESPACE

class DrawingAttributes;
class Drawing;
class DrawingVisual;
class StylusShape;

// namespace MS.Internal.Ink

/// <summary>
/// A static class which generates the cursors for InkCanvas
/// </summary>
class PenCursorManager
{
    static QSvgRenderer EraserImage;
    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods
public:
    /// <summary>
    /// Create a pen cursor from DrawingAttributes object
    /// </summary>
    /// <SecurityNote>
    ///     Critical: Call a SecurityCritical method - CreateCursorFromDrawing
    ///     TreatAsSafe: The method is safe because only safe arguments are passed to CreateCursorFromDrawing
    ///                  Also the size of the cursor will be limited to the half size of the current primary screen.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    static QCursor GetPenCursor(SharedPointer<DrawingAttributes> drawingAttributes, bool isHollow, bool isRightToLeft, double dpiScaleX, double dpiScaleY);

    /// <summary>
    /// Create a point eraser cursor from StylusShape
    /// </summary>
    /// <param name="stylusShape">Eraser Shape</param>
    /// <param name="tranform">Transform</param>
    /// <returns></returns>
    static QCursor GetPointEraserCursor(StylusShape& stylusShape, Matrix const & tranform, double dpiScaleX, double dpiScaleY);

    static QCursor GetPointEraserCursor1(double dpiScaleX, double dpiScaleY);

    static QCursor GetPointEraserCursor2(StylusShape& stylusShape, Matrix const & tranform, double dpiScaleX, double dpiScaleY);

    static Drawing* GetEraserImage(Rect const & bound);

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
    static QCursor GetStrokeEraserCursor();

    /// <summary>
    /// Retrieve selection cursor
    /// </summary>
    /// <param name="hitResult">hitResult</param>
    /// <param name="isRightToLeft">True if InkCanvas.FlowDirection is RightToLeft, false otherwise</param>
    /// <returns></returns>
    static QCursor GetSelectionCursor(InkCanvasSelectionHitResult hitResult, bool isRightToLeft);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------
private:

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
    static QCursor CreateCursorFromDrawing(Drawing& drawing, Point const & hotspot);

    /// <summary>
    /// Create a DrawingVisual from a Drawing
    /// </summary>
    /// <param name="drawing"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <returns></returns>
    static DrawingVisual* CreateCursorDrawingVisual(Drawing& drawing, int width, int height);

    /// <summary>
    /// Custom Pen Drawing
    /// </summary>
    static Drawing* CreatePenDrawing(SharedPointer<DrawingAttributes> drawingAttributes, bool isHollow, bool isRightToLeft, double dpiScaleX, double dpiScaleY);

    /// <summary>
    /// Custom StrokeEraser Drawing
    /// </summary>
    /// <returns></returns>
    static Drawing* CreateStrokeEraserDrawing();

    /// <summary>
    /// Convert values from Avalon unit to the current display unit.
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    static double ConvertToPixel(double value, double dpiScale)
    {
        if ( dpiScale != 0 )
        {
            return value * dpiScale;
        }

        return value;
    }

    // Also used by PenCursorManager
    /// <summary>
    /// Calculate the bits count aligned to N-Byte based on the input count
    /// </summary>
    /// <param name="original">The original value</param>
    /// <param name="nBytesCount">N-Byte</param>
    /// <returns>the nearest bit count which is aligned to N-Byte</returns>
    static int AlignToBytes(double original, int nBytesCount);


    //#endregion Methods

    //------------------------------------------------------
    //
    //  Fields
    //
    //------------------------------------------------------

    //#region Fields

    static QCursor s_StrokeEraserCursor;

    //#endregion Fields
};

INKCANVAS_END_NAMESPACE

#endif // PENCURSORMANAGER_H
