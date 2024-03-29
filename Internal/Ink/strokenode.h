#ifndef STROKENODE_H
#define STROKENODE_H

#include "Internal/Ink/quad.h"
#include "Internal/Ink/strokenodedata.h"
#include "Internal/Ink/strokenodeoperations.h"
#include "Collections/Generic/list.h"
#include "Windows/rect.h"

INKCANVAS_BEGIN_NAMESPACE

class ContourSegment;
class StrokeRenderer;
class DrawingContext;

class StrokeNodeOperations;

// namespace MS.Internal.Ink

/// <summary>
/// StrokeNode represents a single segment on a stroke spine.
/// It's used in enumerating through basic geometries making a stroke contour.
/// </summary>
class StrokeNode
{
public:
    StrokeNode();

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="operations">StrokeNodeOperations object created for particular rendering</param>
    /// <param name="index">Index of the node on the stroke spine</param>
    /// <param name="nodeData">StrokeNodeData for this node</param>
    /// <param name="lastNodeData">StrokeNodeData for the precedeng node</param>
    /// <param name="isLastNode">Whether the current node is the last node</param>
    StrokeNode(
        StrokeNodeOperations* operations,
        int index,
        StrokeNodeData& nodeData,
        StrokeNodeData& lastNodeData,
        bool isLastNode);

    /// <summary>
    /// Position of the node on the stroke spine.
    /// </summary>
    /// <value></value>
    Point const & Position() const { return _thisNode.Position(); }

    /// <summary>
    /// Position of the previous StrokeNode
    /// </summary>
    /// <value></value>
    Point const & PreviousPosition() const { return _lastNode.Position(); }

    /// <summary>
    /// PressureFactor of the node on the stroke spine.
    /// </summary>
    /// <value></value>
    double PressureFactor() const { return _thisNode.PressureFactor(); }

    /// <summary>
    /// PressureFactor of the previous StrokeNode
    /// </summary>
    /// <value></value>
    double PreviousPressureFactor() const { return _lastNode.PressureFactor(); }

    /// <summary>
    /// Tells whether the node shape (the stylus shape used in the rendering)
    /// is elliptical or polygonal. If the shape is an ellipse, GetContourPoints
    /// returns the control points for the quadratic Bezier that defines the ellipse.
    /// </summary>
    /// <value>true if the shape is ellipse, false otherwise</value>
    bool IsEllipse() const { return IsValid() && _operations->IsNodeShapeEllipse(); }

    /// <summary>
    /// Returns true if this is the last node in the enumerator
    /// </summary>
    bool IsLastNode() const { return _isLastNode; }

    /// <summary>
    /// Returns the bounds of the node shape w/o connecting quadrangle
    /// </summary>
    /// <returns></returns>
    Rect GetBounds() const
    {
        return IsValid() ? _operations->GetNodeBounds(_thisNode) : Rect::Empty();
    }

    /// <summary>
    /// Returns the bounds of the node shape and connecting quadrangle
    /// </summary>
    /// <returns></returns>
    Rect GetBoundsConnected() const
    {
        return IsValid() ? Rect::Union(_operations->GetNodeBounds(_thisNode), ConnectingQuad().Bounds()) : Rect::Empty();
    }

    /// <summary>
    /// Returns the points that make up the stroke node shape (minus the connecting quad)
    /// </summary>
    void GetContourPoints(List<Point> & pointBuffer) const
    {
        if (IsValid())
        {
            _operations->GetNodeContourPoints(_thisNode, pointBuffer);
        }
    }

    /// <summary>
    /// Returns the points that make up the stroke node shape (minus the connecting quad)
    /// </summary>
    void GetPreviousContourPoints(List<Point> & pointBuffer) const
    {
        if (IsValid())
        {
            _operations->GetNodeContourPoints(_lastNode, pointBuffer);
        }
    }

    /// <summary>
    /// Returns the connecting quad
    /// </summary>
    Quad const & GetConnectingQuad() const
    {
        if (IsValid())
        {
            return ConnectingQuad();
        }
        return Quad::Empty();
    }

    ///// <summary>
    ///// IsPointWithinRectOrEllipse
    ///// </summary>
    //internal bool IsPointWithinRectOrEllipse(Point point, double xRadiusOrHalfWidth, double yRadiusOrHalfHeight, Point center, bool isEllipse)
    //{
    //    if (isEllipse)
    //    {
    //        //determine what delta is required to move the Rect to be
    //        //centered at 0,0
    //        double xDelta = center.x() + xRadiusOrHalfWidth;
    //        double yDelta = center.y() + yRadiusOrHalfHeight;

    //        //offset the point by that delta
    //        point.x() -= xDelta;
    //        point.y() -= yDelta;

    //        //formula for ellipse is x^2/a^2 + y^2/b^2 = 1
    //        double a = xRadiusOrHalfWidth;
    //        double b = yRadiusOrHalfHeight;
    //        double res = (((point.x() * point.x()) / (a * a)) +
    //                     ((point.y() * point.y()) / (b * b)));

    //        if (res <= 1)
    //        {
    //            return true;
    //        }
    //        return false;
    //    }
    //    else
    //    {
    //        if (point.x() >= (center.x() - xRadiusOrHalfWidth) &&
    //            point.x() <= (center.x() + xRadiusOrHalfWidth) &&
    //            point.y() >= (center.y() - yRadiusOrHalfHeight) &&
    //            point.y() <= (center.y() + yRadiusOrHalfHeight))
    //        {
    //            return true;
    //        }
    //        return false;
    //    }
    //}

    /// <summary>
    /// GetPointsAtStartOfSegment
    /// </summary>
    void GetPointsAtStartOfSegment(List<Point> & abPoints,
                                            List<Point> & dcPoints
#if DEBUG_RENDERING_FEEDBACK
                                            , DrawingContext& debugDC, double feedbackSize, bool showFeedback
#endif
                                            );


    /// <summary>
    /// GetPointsAtEndOfSegment
    /// </summary>
    void GetPointsAtEndOfSegment(  List<Point> &abPoints,
                                            List<Point>& dcPoints
#if DEBUG_RENDERING_FEEDBACK
                                            , DrawingContext& debugDC, double feedbackSize, bool showFeedback
#endif
                                            );

    /// <summary>
    /// GetPointsAtMiddleSegment
    /// </summary>
    void GetPointsAtMiddleSegment( StrokeNode & previous,
                                            double angleBetweenNodes,
                                            List<Point> & abPoints,
                                            List<Point> & dcPoints,
                                            bool & missingIntersection
#if DEBUG_RENDERING_FEEDBACK
                                            , DrawingContext& debugDC, double feedbackSize, bool showFeedback
#endif
                                            );

    /// <summary>
    /// Returns the intersection between two lines.  This code assumes there is an intersection
    /// and should only be called if that assumption is valid
    /// </summary>
    /// <returns></returns>
    static Point GetIntersection(Point const & line1Start, Point const & line1End, Point const & line2Start, Point const & line2End);

    /// <summary>
    /// This method tells whether the contour of a given stroke node
    /// intersects with the contour of this node. The contours of both nodes
    /// include their connecting quadrangles.
    /// </summary>
    /// <param name="hitNode"></param>
    /// <returns></returns>
    bool HitTest(StrokeNode const & hitNode);

    /// <summary>
    /// Finds out if a given node intersects with this one,
    /// and returns findices of the intersection.
    /// </summary>
    /// <param name="hitNode"></param>
    /// <returns></returns>
    StrokeFIndices CutTest(StrokeNode const & hitNode);

    /// <summary>
    /// Finds out if a given linear segment intersects with the contour of this node
    /// (including connecting quadrangle), and returns findices of the intersection.
    /// </summary>
    /// <param name="begin"></param>
    /// <param name="end"></param>
    /// <returns></returns>
    StrokeFIndices CutTest(Point const & begin, Point const & end);

    /// <summary>
    /// Binds a local fragment to this node by setting the integer part of the
    /// fragment findices equal to the index of the previous node
    /// </summary>
    /// <param name="fragment"></param>
    /// <returns></returns>
    StrokeFIndices BindFIndices(StrokeFIndices & fragment);

    int Index()
    {
        return _index;
    }

    /// <summary>
    /// Bind the StrokeFIndices for lasso hit test results.
    /// </summary>
    /// <param name="fragment"></param>
    /// <returns></returns>
    StrokeFIndices BindFIndicesForLassoHitTest(StrokeFIndices & fragment);

    /// <summary>
    /// Tells whether the StrokeNode instance is valid or not (created via the default ctor)
    /// </summary>
    bool IsValid() const { return valid_; }

    /// <summary>
    /// The quadrangle that connects this and the previous node.
    /// Can be empty if this node is the first one or if one of the nodes is
    /// completely inside the other.
    /// The type Quad is supposed to be internal even if we surface StrokeNode.
    /// External users of StrokeNode should use GetConnectionPoints instead.
    /// </summary>
    Quad const & ConnectingQuad() const;

    /// <summary>
    /// Returns an enumerator for edges of the contour comprised by the node
    /// and connecting quadrangle (_lastNode is excluded)
    /// Used for hit-testing a stroke against an other stroke (stroke and point erasing)
    /// </summary>
    List<ContourSegment> GetContourSegments() const;

    /// <summary>
    /// Returns the spine point that corresponds to the given findex.
    /// </summary>
    /// <param name="findex">A local findex between the previous index and this one (ex: between 2.0 and 3.0)</param>
    /// <returns>Point on the spine</returns>
    Point GetPointAt(double findex);

    // Internal objects created for particular rendering
private:
    bool valid_ = false;
    StrokeNodeOperations *   _operations = nullptr;

    // Node's index on the stroke spine
    int             _index = 0;

    // This and the previous node data that used by the StrokeNodeOperations object to build
    // and/or hit-test the contour of the node/segment
    StrokeNodeData  _thisNode;
    StrokeNodeData  _lastNode;

    // Calculating of the connecting quadrangle is not a cheap operations, therefore,
    // first, it's computed only by request, and second, once computed it's cached in the StrokeNode
    mutable bool            _isQuadCached = false;
    mutable Quad            _connectingQuad;

    // Is the current stroke node the last node?
    bool _isLastNode = false;
};

INKCANVAS_END_NAMESPACE

#endif // STROKENODE_H
