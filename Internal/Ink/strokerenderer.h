#ifndef STROKERENDERER_H
#define STROKERENDERER_H

#include "InkCanvas_global.h"

#include "Collections/Generic/list.h"
#include "Windows/rect.h"
#include "Windows/Media/matrix.h"

#ifdef INKCANVAS_QT
#include <QColor>
#endif

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class StrokeNodeIterator;
class DrawingAttributes;
class DrawingContext;
class Geometry;
class Stroke;
class StreamGeometryContext;
class StrokeNode;

class StrokeRenderer
{
public:
    /// <summary>
    /// Calculate the StreamGeometry for the StrokeNodes.
    /// This method is one of our most sensitive perf paths.  It has been optimized to
    /// create the minimum path figures in the StreamGeometry.  There are two structures
    /// we create for each point in a stroke, the strokenode and the connecting quad.  Adding
    /// strokenodes is very expensive later when MIL renders it, so this method has been optimized
    /// to only add strokenodes when either pressure changes, or the angle of the stroke changes.
    /// </summary>
    static void CalcGeometryAndBoundsWithTransform(StrokeNodeIterator& iterator,
                                                   DrawingAttributes& drawingAttributes,
                                                   MatrixTypes stylusTipMatrixType,
                                                   bool calculateBounds,
                                                   Geometry*& geometry,
                                                   Rect& bounds);


    /// <summary>
    /// Calculate the StreamGeometry for the StrokeNodes.
    /// This method is one of our most sensitive perf paths.  It has been optimized to
    /// create the minimum path figures in the StreamGeometry.  There are two structures
    /// we create for each point in a stroke, the strokenode and the connecting quad.  Adding
    /// strokenodes is very expensive later when MIL renders it, so this method has been optimized
    /// to only add strokenodes when either pressure changes, or the angle of the stroke changes.
    /// </summary>
    //[FriendAccessAllowed]
    static void CalcGeometryAndBounds(StrokeNodeIterator& iterator,
                                               DrawingAttributes& drawingAttributes,
#if DEBUG_RENDERING_FEEDBACK
                                               DrawingContext& debugDC,
                                               double feedbackSize,
                                               bool showFeedback,
#endif
                                               bool calculateBounds,
                                               Geometry*& geometry,
                                               Rect& bounds);

private:
    /// <summary>
    /// Helper routine to render two distinct stroke nodes
    /// </summary>
    static void RenderTwoStrokeNodes(   StreamGeometryContext& context,
                                                StrokeNode& strokeNodePrevious,
                                                Rect const & strokeNodePreviousBounds,
                                                StrokeNode& strokeNodeCurrent,
                                                Rect const & strokeNodeCurrentBounds,
                                                List<Point>& pointBuffer1,
                                                List<Point>& pointBuffer2,
                                                List<Point>& pointBuffer3
#if DEBUG_RENDERING_FEEDBACK
                                               ,DrawingContext& debugDC,
                                               double feedbackSize,
                                               bool showFeedback
#endif
                                                );

    /// <summary>
    /// Simple helper enum
    /// </summary>
    enum RectCompareResult
    {
        Rect1ContainsRect2,
        Rect2ContainsRect1,
        NoItersection,
    };

    /// <summary>
    /// ReverseDCPointsRenderAndClear
    /// </summary>
    static void ReverseDCPointsRenderAndClear(StreamGeometryContext& context, List<Point> & abPoints, List<Point> & dcPoints, List<Point> & polyLinePoints, bool isEllipse, bool clear);

    /// <summary>
    /// FuzzyContains for two rects
    /// </summary>
    static RectCompareResult FuzzyContains(Rect const & rect1, Rect const & rect2, double percentIntersect);

    /// <summary>
    /// Private helper to render a path figure to the SGC
    /// </summary>
    static void AddFigureToStreamGeometryContext(StreamGeometryContext& context, List<Point>& points, bool isBezierFigure);

    /// <summary>
    /// Private helper to render a path figure to the SGC
    /// </summary>
    static void AddPolylineFigureToStreamGeometryContext(StreamGeometryContext& context, List<Point>& abPoints, List<Point>& dcPoints);

    /// <summary>
    /// Private helper to render a path figure to the SGC
    /// </summary>
    static void AddArcToFigureToStreamGeometryContext(StreamGeometryContext& context, List<Point>& abPoints, List<Point>& dcPoints, List<Point>& polyLinePoints);

    /// <summary>
    /// calculates the angle between the previousPosition and the current one and then computes the delta between
    /// the lastAngle.  lastAngle is also updated
    /// </summary>
    static double GetAngleDeltaFromLast(Point const & previousPosition, Point const & currentPosition, double& lastAngle);

    /// <summary>
    /// calculates the angle between the previousPosition and the current one and then computes the delta between
    /// the lastAngle.  lastAngle is also updated
    /// </summary>
    static double GetAngleBetween(Point const & previousPosition, Point const & currentPosition);

public:
#ifdef INKCANVAS_QT
    /// <summary>
    /// Get the DrawingAttributes to use for a highlighter stroke. The return value is a copy of
    /// the DA passed in if color.A != 255 with color.A overriden to be 255. Otherwise it returns
    /// the DA passed in.
    /// </summary>
    static SharedPointer<DrawingAttributes> GetHighlighterAttributes(Stroke& stroke, SharedPointer<DrawingAttributes> da);

    /// <summary>
    /// Get the color used to draw a highlighter.
    /// </summary>
    static QColor GetHighlighterColor(QColor color)
    {
        // For a highlighter stroke, the color.A is overriden to be 255
        color.setAlpha(SolidStrokeAlpha);
        return color;
    }
#endif

    // Opacity for highlighter container visuals
    static constexpr double HighlighterOpacity = 0.5;
    static constexpr unsigned char SolidStrokeAlpha = 0xFF;
    static Point ArcToMarker;

};

INKCANVAS_END_NAMESPACE

#endif // STROKERENDERER_H
