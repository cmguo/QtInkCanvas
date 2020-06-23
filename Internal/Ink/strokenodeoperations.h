#ifndef STROKENODEOPERATIONS_H
#define STROKENODEOPERATIONS_H

#include "Windows/Ink/stylusshape.h"
#include "strokenodedata.h"
#include "quad.h"
#include "strokefindices.h"
#include "contoursegment.h"

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QVector>

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// The base operations class that implements polygonal node operations by default.
/// </summary>
class StrokeNodeOperations
{
public:
    enum HitResult
    {
        Hit,
        Left,
        Right,
        InFront,
        Behind
    };

    /// <summary>
    ///
    /// </summary>
    /// <param name="nodeShape"></param>
    /// <returns></returns>
    static StrokeNodeOperations * CreateInstance(StylusShape & nodeShape);


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeShape">shape of the nodes</param>
    StrokeNodeOperations(StylusShape & nodeShape);

    virtual ~StrokeNodeOperations();

    /// <summary>
    /// This is probably not the best (design-wise) but the cheapest way to tell
    /// EllipticalNodeOperations from all other implementations of node operations.
    /// </summary>
    virtual bool IsNodeShapeEllipse() { return false; }

    /// <summary>
    /// Computes the bounds of a node
    /// </summary>
    /// <param name="node">node to compute bounds of</param>
    /// <returns>bounds of the node</returns>
    QRectF GetNodeBounds(StrokeNodeData const & node);

    void GetNodeContourPoints(StrokeNodeData const & node, QList<QPointF> & pointBuffer);

    /// <summary>
    /// Returns an enumerator for edges of the contour comprised by a given node
    /// and its connecting quadrangle.
    /// Used for hit-testing a stroke against an other stroke (stroke and point erasing)
    /// </summary>
    /// <param name="node">node</param>
    /// <param name="quad">quadrangle connecting the node to the preceeding node</param>
    /// <returns>contour segments enumerator</returns>
    virtual QList<ContourSegment> GetContourSegments(StrokeNodeData const & node, Quad & quad);

    /// <summary>
    /// ISSUE-2004/06/15- temporary workaround to avoid hit-testing ellipses with ellipses
    /// </summary>
    /// <param name="beginNode"></param>
    /// <param name="endNode"></param>
    /// <returns></returns>
    virtual QList<ContourSegment> GetNonBezierContourSegments(StrokeNodeData const & beginNode, StrokeNodeData const & endNode);


    /// <summary>
    /// Finds connecting points for a pair of stroke nodes (of a polygonal shape)
    /// </summary>
    /// <param name="beginNode">a node to connect</param>
    /// <param name="endNode">another node, next to beginNode</param>
    /// <returns>connecting quadrangle, that can be empty if one node is inside the other</returns>
    virtual Quad GetConnectingQuad(StrokeNodeData const & beginNode, StrokeNodeData const & endNode);

    /// <summary>
    /// Hit-tests ink segment defined by two nodes against a linear segment.
    /// </summary>
    /// <param name="beginNode">Begin node of the ink segment</param>
    /// <param name="endNode">End node of the ink segment</param>
    /// <param name="quad">Pre-computed quadrangle connecting the two ink nodes</param>
    /// <param name="hitBeginPoint">Begin point of the hitting segment</param>
    /// <param name="hitEndPoint">End point of the hitting segment</param>
    /// <returns>true if there's intersection, false otherwise</returns>
    virtual bool HitTest(
        StrokeNodeData const & beginNode, StrokeNodeData const & endNode, Quad& quad, QPointF const &hitBeginPoint, QPointF const &hitEndPoint);

    /// <summary>
    /// Hit-tests a stroke segment defined by two nodes against another stroke segment.
    /// </summary>
    /// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
    /// <param name="endNode">End node of the stroke segment</param>
    /// <param name="quad">Pre-computed quadrangle connecting the two nodes.
    /// Can be empty if the begion node is empty or when one node is entirely inside the other</param>
    /// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
    /// <returns>true if the contours intersect or overlap</returns>
    virtual bool HitTest(
        StrokeNodeData const & beginNode, StrokeNodeData const & endNode, Quad& quad, QList<ContourSegment> const & hitContour);

    /// <summary>
    /// Hit-tests ink segment defined by two nodes against a linear segment.
    /// </summary>
    /// <param name="beginNode">Begin node of the ink segment</param>
    /// <param name="endNode">End node of the ink segment</param>
    /// <param name="quad">Pre-computed quadrangle connecting the two ink nodes</param>
    /// <param name="hitBeginPoint">Begin point of the hitting segment</param>
    /// <param name="hitEndPoint">End point of the hitting segment</param>
    /// <returns>Exact location to cut at represented by StrokeFIndices</returns>
    virtual StrokeFIndices CutTest(
        StrokeNodeData const & beginNode, StrokeNodeData const & endNode, Quad& quad, QPointF const &hitBeginPoint, QPointF const &hitEndPoint);

    /// <summary>
    /// CutTest
    /// </summary>
    /// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
    /// <param name="endNode">End node of the stroke segment</param>
    /// <param name="quad">Pre-computed quadrangle connecting the two nodes.
    /// Can be empty if the begion node is empty or when one node is entirely inside the other</param>
    /// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
    /// <returns></returns>
    virtual StrokeFIndices CutTest(
        StrokeNodeData const & beginNode, StrokeNodeData const & endNode, Quad& quad, QList<ContourSegment> const & hitContour);

    /// <summary>
    /// Cutting ink with polygonal tip shapes with a linear segment
    /// </summary>
    /// <param name="spineQPointF">QPointF representing the starting and ending point for the inking
    ///             segment</param>
    /// <param name="pressureDelta">Represents the difference in the node size for startNode and endNode.
    ///              pressureDelta = (endNode.PressureFactor() / beginNode.PressureFactor()) - 1</param>
    /// <param name="hitBegin">Start point of the hitting segment</param>
    /// <param name="hitEnd">End point of the hitting segment</param>
    /// <returns>a double representing the point of clipping</returns>
    double ClipTest(QPointF const & spineQPointF, double pressureDelta, QPointF const & hitBegin, QPointF const & hitEnd);

    /// <summary>
    /// Clip-Testing a polygonal inking segment against an arc (circle)
    /// </summary>
    /// <param name="spineQPointF">QPointF representing the starting and ending point for the inking
    ///             segment</param>
    /// <param name="pressureDelta">Represents the difference in the node size for startNode and endNode.
    ///              pressureDelta = (endNode.PressureFactor() / beginNode.PressureFactor()) - 1</param>
    /// <param name="hitCenter">The center of the hitting circle</param>
    /// <param name="hitRadius">The radius of the hitting circle</param>
    /// <returns>a double representing the point of clipping</returns>
    double ClipTestArc(QPointF const & spineQPointF, double pressureDelta, QPointF const & hitCenter, QPointF const & hitRadius);

    /// <summary>
    /// access to __vertices
    /// </summary>
    /// <returns></returns>
    QVector<QPointF> & GetVertices()
    {
        return _vertices;
    }

    /// <summary>
    /// Helper function to hit-test the biggest node against hitting contour segments
    /// </summary>
    /// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
    /// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
    /// <param name="endNode">End node of the stroke segment</param>
    /// <returns>true if hit; false otherwise</returns>
    bool HitTestPolygonContourSegments(
        QList<ContourSegment> const & hitContour, StrokeNodeData const & beginNode, StrokeNodeData const & endNode);

    /// <summary>
    /// Helper function to HitTest the the hitting contour against the inking contour
    /// </summary>
    /// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
    /// <param name="quad">A connecting quad</param>
    /// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
    /// <param name="endNode">End node of the stroke segment</param>
    /// <returns>true if hit; false otherwise</returns>
    bool HitTestInkContour(
        QList<ContourSegment> const & hitContour, Quad& quad, StrokeNodeData const & beginNode, StrokeNodeData const &endNode);


    /// <summary>
    /// Helper function to Hit-test against the two stroke nodes only (excluding the connecting quad).
    /// </summary>
    /// <param name="hitSegment"></param>
    /// <param name="beginNode"></param>
    /// <param name="endNode"></param>
    /// <param name="result"></param>
    /// <returns></returns>
    bool HitTestStrokeNodes(
        ContourSegment const & hitSegment, StrokeNodeData const &beginNode, StrokeNodeData const &endNode, StrokeFIndices& result);

    /// <summary>
    ///  Calculate the clip location
    /// </summary>
    /// <param name="hitSegment">the hitting segment</param>
    /// <param name="beginNode">begin node</param>
    /// <param name="spineQPointF"></param>
    /// <param name="pressureDelta"></param>
    /// <returns>the clip location. not-clip if return StrokeFIndices::BeforeFirst</returns>
    double CalculateClipLocation(
        ContourSegment const & hitSegment, StrokeNodeData const & beginNode, QPointF const & spineQPointF, double pressureDelta);

    /// <summary>
    /// Helper method used to determine if we came up with a bogus result during hit testing
    /// </summary>
    bool IsInvalidCutTestResult(StrokeFIndices const & result);


    // Shape parameters
private:
    QRectF        _shapeBounds;
    QVector<QPointF>    _vertices;

public:

    /// <summary>
    /// Hit-tests a linear segment against a convex polygon.
    /// </summary>
    /// <param name="vertices">Vertices of the polygon (in clockwise order)</param>
    /// <param name="hitBegin">an end point of the hitting segment</param>
    /// <param name="hitEnd">an end point of the hitting segment</param>
    /// <returns>true if hit; false otherwise</returns>
    static bool HitTestPolygonSegment(QVector<QPointF> & vertices, QPointF& hitBegin, QPointF& hitEnd);

    /// <summary>
    /// This is a specialized version of HitTestPolygonSegment that takes
    /// a Quad for a polygon. This method is called very intensively by
    /// hit-testing API and we don't want to create Vector[] for every quad it hit-tests.
    /// </summary>
    /// <param name="quad">the connecting quad to test against</param>
    /// <param name="hitBegin">begin point of the hitting segment</param>
    /// <param name="hitEnd">end point of the hitting segment</param>
    /// <returns>true if hit, false otherwise</returns>
    static bool HitTestQuadSegment(Quad & quad, QPointF const& hitBegin, QPointF const& hitEnd);

    /// <summary>
    /// Hit-test a polygin against a circle
    /// </summary>
    /// <param name="vertices">Vectors representing the vertices of the polygon, ordered in clockwise order</param>
    /// <param name="center">QPointF &representing the center of the circle</param>
    /// <param name="radius">QPointF &representing the radius of the circle</param>
    /// <returns>true if hit, false otherwise</returns>
    static bool HitTestPolygonCircle(QVector<QPointF>& vertices, QPointF& center, QPointF& radius);

    /// <summary>
    /// This is a specialized version of HitTestPolygonCircle that takes
    /// a Quad for a polygon. This method is called very intensively by
    /// hit-testing API and we don't want to create Vector[] for every quad it hit-tests.
    /// </summary>
    /// <param name="quad">the connecting quad</param>
    /// <param name="center">center of the circle</param>
    /// <param name="radius">radius of the circle </param>
    /// <returns>true if hit; false otherwise</returns>
    static bool HitTestQuadCircle(Quad const& quad, QPointF const& center, QPointF const& radius);


    /// <summary>
    /// Finds out where the segment [hitBegin, hitEnd]
    /// is about the segment [orgBegin, orgEnd].
    /// </summary>
    static HitResult WhereIsSegmentAboutSegment(
        QPointF const & hitBegin, QPointF const & hitEnd, QPointF const & orgBegin, QPointF const & orgEnd);

    /// <summary>
    /// Find out the relative location of a circle relative to a line segment
    /// </summary>
    /// <param name="center">center of the circle</param>
    /// <param name="radius">radius of the circle. center.radius is a point on the circle</param>
    /// <param name="segBegin">begin point of the line segment</param>
    /// <param name="segEnd">end point of the line segment</param>
    /// <returns>test result</returns>
    static HitResult WhereIsCircleAboutSegment(
        QPointF center, QPointF radius, QPointF segBegin, QPointF segEnd);

    /// <summary>
    /// Finds out where the vector1 is about the vector2.
    /// </summary>
    static HitResult WhereIsVectorAboutVector(QPointF const &vector1, QPointF const & vector2);

    /// <summary>
    /// Tells whether the hitQPointF &intersects the arc defined by two vectors.
    /// </summary>
    static HitResult WhereIsVectorAboutArc(QPointF const &hitVector, QPointF const & arcBegin, QPointF const & arcEnd);

    /// <summary>
    ///
    /// </summary>
    /// <param name="vector"></param>
    /// <returns></returns>
    static QPointF TurnLeft(QPointF const &vector);

    /// <summary>
    ///
    /// </summary>
    /// <param name="vector"></param>
    /// <returns></returns>
    static QPointF TurnRight(QPointF const &vector);

    /// <summary>
    ///
    /// </summary>
    /// <param name="hitResult"></param>
    /// <param name="prevHitResult"></param>
    /// <returns></returns>
    static bool IsOutside(HitResult hitResult, HitResult prevHitResult);

    /// <summary>
    /// helper function to find out the ratio of the distance from hitpoint to lineVector
    /// and the distance from lineQPointF &to (lineVector+nextLine)
    /// </summary>
    /// <param name="linesVector">This is one edge of a polygonal node</param>
    /// <param name="nextLine">The connection QPointF between the same edge on biginNode and ednNode</param>
    /// <param name="hitPoint">a point</param>
    /// <returns>the relative position of hitPoint</returns>
    static double GetPositionBetweenLines(QPointF &linesVector, QPointF nextLine, QPointF hitPoint);

    /// <summary>
    /// On a line defined buy two points finds the findex of the point
    /// nearest to the origin (0,0). Same as FindNearestOnLine just
    /// different output.
    /// </summary>
    /// <param name="begin">A point on the line.</param>
    /// <param name="end">Another point on the line.</param>
    /// <returns></returns>
    static double GetProjectionFIndex(QPointF const &begin, QPointF const& end);

    /// <summary>
    /// On a line defined buy two points finds the point nearest to the origin (0,0).
    /// </summary>
    /// <param name="begin">A point on the line.</param>
    /// <param name="end">Another point on the line.</param>
    /// <returns></returns>
    static QPointF GetProjection(QPointF const &begin, QPointF const& end);

    /// <summary>
    /// On a given segment finds the point nearest to the origin (0,0).
    /// </summary>
    /// <param name="begin">The segment's begin point.</param>
    /// <param name="end">The segment's end point.</param>
    /// <returns></returns>
    static QPointF GetNearest(QPointF const &begin, QPointF const & end);

    /// <summary>
    /// Clears double's computation fuzz around 0 and 1
    /// </summary>
    static double AdjustFIndex(double findex);
};

INKCANVAS_END_NAMESPACE

#endif // STROKENODEOPERATIONS_H
