#ifndef STROKENODE_H
#define STROKENODE_H

#include "quad.h"
#include "strokenodedata.h"
#include "strokenodeoperations.h"

#include <QList>
#include <QPointF>
#include <QRectF>

class ContourSegment;
class StrokeRenderer;

class StrokeNodeOperations;

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
        bool isLastNode)
        : valid_(true)
        , _operations(operations)
        , _connectingQuad(Quad::Empty())
    {
        //System.Diagnostics.//Debug.Assert(operations != null);
        //System.Diagnostics.//Debug.Assert((nodeData.IsEmpty == false) && (index >= 0));


        //_operations = operations;
        _index = index;
        _thisNode = nodeData;
        _lastNode = lastNodeData;
        //_isQuadCached = lastNodeData.IsEmpty;
        //_connectingQuad = Quad.Empty;
        _isLastNode = isLastNode;
    }

    /// <summary>
    /// Position of the node on the stroke spine.
    /// </summary>
    /// <value></value>
    QPointF & Position() { return _thisNode.Position(); }

    /// <summary>
    /// Position of the previous StrokeNode
    /// </summary>
    /// <value></value>
    QPointF & PreviousPosition() { return _lastNode.Position(); }

    /// <summary>
    /// PressureFactor of the node on the stroke spine.
    /// </summary>
    /// <value></value>
    double PressureFactor() { return _thisNode.PressureFactor(); }

    /// <summary>
    /// PressureFactor of the previous StrokeNode
    /// </summary>
    /// <value></value>
    double PreviousPressureFactor() { return _lastNode.PressureFactor(); }

    /// <summary>
    /// Tells whether the node shape (the stylus shape used in the rendering)
    /// is elliptical or polygonal. If the shape is an ellipse, GetContourPoints
    /// returns the control points for the quadratic Bezier that defines the ellipse.
    /// </summary>
    /// <value>true if the shape is ellipse, false otherwise</value>
    bool IsEllipse() { return IsValid() && _operations->IsNodeShapeEllipse(); }

    /// <summary>
    /// Returns true if this is the last node in the enumerator
    /// </summary>
    bool IsLastNode() { return _isLastNode; }

    /// <summary>
    /// Returns the bounds of the node shape w/o connecting quadrangle
    /// </summary>
    /// <returns></returns>
    QRectF GetBounds()
    {
        return IsValid() ? _operations->GetNodeBounds(_thisNode) : QRectF();
    }

    /// <summary>
    /// Returns the bounds of the node shape and connecting quadrangle
    /// </summary>
    /// <returns></returns>
    QRectF GetBoundsConnected()
    {
        return IsValid() ? _operations->GetNodeBounds(_thisNode).united(ConnectingQuad().Bounds()) : QRectF();
    }

    /// <summary>
    /// Returns the points that make up the stroke node shape (minus the connecting quad)
    /// </summary>
    void GetContourPoints(QList<QPointF> & pointBuffer)
    {
        if (IsValid())
        {
            _operations->GetNodeContourPoints(_thisNode, pointBuffer);
        }
    }

    /// <summary>
    /// Returns the points that make up the stroke node shape (minus the connecting quad)
    /// </summary>
    void GetPreviousContourPoints(QList<QPointF> & pointBuffer)
    {
        if (IsValid())
        {
            _operations->GetNodeContourPoints(_lastNode, pointBuffer);
        }
    }

    /// <summary>
    /// Returns the connecting quad
    /// </summary>
    Quad GetConnectingQuad()
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
    //        //determine what delta is required to move the QRectF to be
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
    void GetPointsAtStartOfSegment(QList<QPointF> & abPoints,
                                            QList<QPointF> & dcPoints
#if DEBUG_RENDERING_FEEDBACK
                                            , DrawingContext debugDC, double feedbackSize, bool showFeedback
#endif
                                            );


    /// <summary>
    /// GetPointsAtEndOfSegment
    /// </summary>
    void GetPointsAtEndOfSegment(  QList<QPointF> &abPoints,
                                            QList<QPointF>& dcPoints
#if DEBUG_RENDERING_FEEDBACK
                                            , DrawingContext debugDC, double feedbackSize, bool showFeedback
#endif
                                            );

    /// <summary>
    /// GetPointsAtMiddleSegment
    /// </summary>
    void GetPointsAtMiddleSegment( StrokeNode previous,
                                            double angleBetweenNodes,
                                            QList<QPointF> abPoints,
                                            QList<QPointF> dcPoints,
                                            bool & missingIntersection
#if DEBUG_RENDERING_FEEDBACK
                                            , DrawingContext debugDC, double feedbackSize, bool showFeedback
#endif
                                            );

    /// <summary>
    /// Returns the intersection between two lines.  This code assumes there is an intersection
    /// and should only be called if that assumption is valid
    /// </summary>
    /// <returns></returns>
    static QPointF GetIntersection(QPointF line1Start, QPointF line1End, QPointF line2Start, QPointF line2End);

    /// <summary>
    /// This method tells whether the contour of a given stroke node
    /// intersects with the contour of this node. The contours of both nodes
    /// include their connecting quadrangles.
    /// </summary>
    /// <param name="hitNode"></param>
    /// <returns></returns>
    bool HitTest(StrokeNode hitNode);

    /// <summary>
    /// Finds out if a given node intersects with this one,
    /// and returns findices of the intersection.
    /// </summary>
    /// <param name="hitNode"></param>
    /// <returns></returns>
    StrokeFIndices CutTest(StrokeNode hitNode);

    /// <summary>
    /// Finds out if a given linear segment intersects with the contour of this node
    /// (including connecting quadrangle), and returns findices of the intersection.
    /// </summary>
    /// <param name="begin"></param>
    /// <param name="end"></param>
    /// <returns></returns>
    StrokeFIndices CutTest(QPointF begin, QPointF end);

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
    StrokeFIndices BindFIndicesForLassoHitTest(StrokeFIndices&fragment);

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
    Quad & ConnectingQuad();

    /// <summary>
    /// Returns an enumerator for edges of the contour comprised by the node
    /// and connecting quadrangle (_lastNode is excluded)
    /// Used for hit-testing a stroke against an other stroke (stroke and point erasing)
    /// </summary>
    QList<ContourSegment> GetContourSegments();

    /// <summary>
    /// Returns the spine point that corresponds to the given findex.
    /// </summary>
    /// <param name="findex">A local findex between the previous index and this one (ex: between 2.0 and 3.0)</param>
    /// <returns>Point on the spine</returns>
    QPointF GetPointAt(double findex);

    // Internal objects created for particular rendering
private:
    bool valid_ = false;
    StrokeNodeOperations *   _operations;

    // Node's index on the stroke spine
    int             _index;

    // This and the previous node data that used by the StrokeNodeOperations object to build
    // and/or hit-test the contour of the node/segment
    StrokeNodeData  _thisNode;
    StrokeNodeData  _lastNode;

    // Calculating of the connecting quadrangle is not a cheap operations, therefore,
    // first, it's computed only by request, and second, once computed it's cached in the StrokeNode
    bool            _isQuadCached;
    Quad            _connectingQuad;

    // Is the current stroke node the last node?
    bool _isLastNode;
};

#endif // STROKENODE_H
