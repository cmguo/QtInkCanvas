#ifndef ELLIPTICALNODEOPERATIONS_H
#define ELLIPTICALNODEOPERATIONS_H

#include "strokenodeoperations.h"

class EllipticalNodeOperations : public StrokeNodeOperations
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="nodeShape"></param>
    EllipticalNodeOperations(StylusShape nodeShape);

    /// <summary>
    /// This is probably not the best (design-wise) but the cheapest way to tell
    /// EllipticalNodeOperations from all other implementations of node operations.
    /// </summary>
    virtual bool IsNodeShapeEllipse() { return true; }

    /// <summary>
    /// Finds connecting points for a pair of stroke nodes
    /// </summary>
    /// <param name="beginNode">a node to connect</param>
    /// <param name="endNode">another node, next to beginNode</param>
    /// <returns>connecting quadrangle</returns>
    virtual Quad GetConnectingQuad(StrokeNodeData& beginNode, StrokeNodeData endNode);

    /// <summary>
    ///
    /// </summary>
    /// <param name="node"></param>
    /// <param name="quad"></param>
    /// <returns></returns>
    virtual QList<ContourSegment> GetContourSegments(StrokeNodeData& node, Quad& quad);

    /// <summary>
    /// ISSUE-2004/06/15- temporary workaround to avoid hit-testing ellipses with ellipses
    /// </summary>
    /// <param name="beginNode"></param>
    /// <param name="endNode"></param>
    /// <returns></returns>
    virtual QList<ContourSegment> GetNonBezierContourSegments(StrokeNodeData& beginNode, StrokeNodeData& endNode);


    /// <summary>
    /// Hit-tests a stroke segment defined by two nodes against a linear segment.
    /// </summary>
    /// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
    /// <param name="endNode">End node of the stroke segment</param>
    /// <param name="quad">Pre-computed quadrangle connecting the two nodes.
    /// Can be empty if the begion node is empty or when one node is entirely inside the other</param>
    /// <param name="hitBeginPoint">an end point of the hitting linear segment</param>
    /// <param name="hitEndPoint">an end point of the hitting linear segment</param>
    /// <returns>true if the hitting segment intersect the contour comprised of the two stroke nodes</returns>
    virtual bool HitTest(
        StrokeNodeData& beginNode, StrokeNodeData& endNode, Quad& quad, QPointF const& hitBeginPoint, QPointF const& hitEndPoint);

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
        StrokeNodeData beginNode, StrokeNodeData endNode, Quad quad, QList<ContourSegment> hitContour);

    /// <summary>
    /// Cut-test ink segment defined by two nodes and a connecting quad against a linear segment
    /// </summary>
    /// <param name="beginNode">Begin node of the ink segment</param>
    /// <param name="endNode">End node of the ink segment</param>
    /// <param name="quad">Pre-computed quadrangle connecting the two ink nodes</param>
    /// <param name="hitBeginPoint">egin point of the hitting segment</param>
    /// <param name="hitEndPoint">End point of the hitting segment</param>
    /// <returns>Exact location to cut at represented by StrokeFIndices</returns>
    virtual StrokeFIndices CutTest(
        StrokeNodeData beginNode, StrokeNodeData endNode, Quad quad, QPointF const& hitBeginPoint, QPointF const& hitEndPoint);

    /// <summary>
    /// CutTest an inking StrokeNode segment (two nodes and a connecting quadrangle) against a hitting contour
    /// (represented by an enumerator of Contoursegments).
    /// </summary>
    /// <param name="beginNode">The begin StrokeNodeData</param>
    /// <param name="endNode">The end StrokeNodeData</param>
    /// <param name="quad">Connecing quadrangle between the begin and end inking node</param>
    /// <param name="hitContour">The hitting ContourSegments</param>
    /// <returns>StrokeFIndices representing the location for cutting</returns>
    virtual StrokeFIndices CutTest(
        StrokeNodeData& beginNode, StrokeNodeData& endNode, Quad& quad, QList<ContourSegment> hitContour);

    /// <summary>
    /// Clip-Testing a circluar inking segment against a linear segment.
    /// See http://tabletpc/longhorn/Specs/Rendering%20and%20Hit-Testing%20Ink%20in%20Avalon%20M11.doc section
    /// 5.4.4.14	Clip-Testing a Circular Inking Segment against a Linear Segment for details of the algorithm
    /// </summary>
    /// <param name="spineVector">Represent the spine of the inking segment pointing from the beginNode to endNode</param>
    /// <param name="beginRadius">Radius of the beginNode</param>
    /// <param name="endRadius">Radius of the endNode</param>
    /// <param name="hitBegin">Hitting segment start point</param>
    /// <param name="hitEnd">Hitting segment end point</param>
    /// <returns>A double which represents the location for cutting</returns>
    static double ClipTest(QPointF const &spineVector, double beginRadius, double endRadius, QPointF const &hitBegin, QPointF const &hitEnd);

    /// <summary>
    /// Clip-Testing a circular inking segment again a hitting point.
    ///
    /// What need to find out a doulbe value s, which is between 0 and 1, such that
    ///     DistanceOf(hit - s*spine) = beginRadius + s * (endRadius - beginRadius)
    /// That is
    ///     (hit.x()-s*spine.x())^2 + (hit.y()-s*spine.y())^2 = [beginRadius + s*(endRadius-beginRadius)]^2
    /// Rearrange
    ///     A*s^2 + B*s + C = 0
    /// where the value of A, B and C are described in the source code.
    /// Solving for s:
    ///             s = (-B + sqrt(B^2-4A*C))/(2A)  or s = (-B - sqrt(B^2-4A*C))/(2A)
    /// The smaller value between 0 and 1 is the one we want and discard the other one.
    /// </summary>
    /// <param name="spine">Represent the spine of the inking segment pointing from the beginNode to endNode</param>
    /// <param name="beginRadius">Radius of the beginNode</param>
    /// <param name="endRadius">Radius of the endNode</param>
    /// <param name="hit">The hitting point</param>
    /// <returns>A double which represents the location for cutting</returns>
    static double ClipTest(QPointF const &spine, double beginRadius, double endRadius, QPointF const &hit);

    /// <summary>
    /// Helper function to find out the relative location of a segment {segBegin, segEnd} against
    /// a strokeNode (spine).
    /// </summary>
    /// <param name="spine">the spineQPointF const &of the StrokeNode</param>
    /// <param name="segBegin">Start position of the line segment</param>
    /// <param name="segEnd">End position of the line segment</param>
    /// <returns>HitResult</returns>
    static HitResult WhereIsNodeAboutSegment(QPointF const &spine, QPointF const &segBegin, QPointF const &segEnd);

    /// <summary>
    /// Helper method to calculate the exact location to cut
    /// </summary>
    /// <param name="spineVector">QPointF the relative location of the two inking nodes</param>
    /// <param name="hitBegin">the begin point of the hitting segment</param>
    /// <param name="hitEnd">the end point of the hitting segment</param>
    /// <param name="endRadius">endNode radius</param>
    /// <param name="beginRadius">beginNode radius</param>
    /// <param name="result">StrokeFIndices representing the location for cutting</param>
    void CalculateCutLocations(
        QPointF const &spineVector, QPointF &hitBegin, QPointF &hitEnd, double endRadius, double beginRadius, StrokeFIndices& result);

private:
    double _radius = 0;
    QSizeF   _radii;
    QMatrix _transform;
    QMatrix _nodeShapeToCircle;
    QMatrix _circleToNodeShape;
};

#endif // ELLIPTICALNODEOPERATIONS_H
