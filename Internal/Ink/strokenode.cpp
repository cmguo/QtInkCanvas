#include "Internal/Ink/strokenode.h"
#include "Internal/Ink/strokefindices.h"
#include "Internal/Ink/strokerenderer.h"
#include "Windows/Media/drawingcontext.h"
#include "Internal/debug.h"

#define pink darkYellow
#define wheat magenta
#define orange darkCyan

StrokeNode::StrokeNode()
    : _connectingQuad(Quad::Empty())
{

}

/// <summary>
/// Constructor.
/// </summary>
/// <param name="operations">StrokeNodeOperations object created for particular rendering</param>
/// <param name="index">Index of the node on the stroke spine</p aram>
/// <param name="nodeData">StrokeNodeData for this node</param>q
/// <param name="lastNodeData">StrokeNodeData for the precedeng node</param>
/// <param name="isLastNode">Whether the current node is the last node</param>
StrokeNode::StrokeNode(
    StrokeNodeOperations* operations,
    int index,
    StrokeNodeData& nodeData,
    StrokeNodeData& lastNodeData,
    bool isLastNode)
    : valid_(true)
    , _operations(operations)
    , _connectingQuad(Quad::Empty())
{
    Debug::Assert(operations != nullptr);
    Debug::Assert((nodeData.IsEmpty() == false) && (index >= 0));


    //_operations = operations;
    _index = index;
    _thisNode = nodeData;
    _lastNode = lastNodeData;
    //_isQuadCached = lastNodeData.IsEmpty;
    //_connectingQuad = Quad.Empty;
    _isLastNode = isLastNode;
}

void StrokeNode::GetPointsAtStartOfSegment(QList<QPointF> & abPoints,
                                        QList<QPointF> & dcPoints
#if DEBUG_RENDERING_FEEDBACK
                                        , DrawingContext& debugDC, double feedbackSize, bool showFeedback
#endif
                                        )
{
    if (IsValid())
    {
        Quad quad = ConnectingQuad();
        if (IsEllipse())
        {
            QRectF startNodeBounds = _operations->GetNodeBounds(_lastNode);

            //add instructions to arc from D to A
            abPoints.append(quad.D());
            abPoints.append(StrokeRenderer::ArcToMarker);
            abPoints.append(QPointF(startNodeBounds.width(), startNodeBounds.height()));
            abPoints.append(quad.A());

            //simply start at D
            dcPoints.append(quad.D());

#if DEBUG_RENDERING_FEEDBACK
            if (showFeedback)
            {
                debugDC.DrawEllipse(QBrush(), QPen(Qt::pink, feedbackSize / 2), _lastNode.Position(), startNodeBounds.width() / 2, startNodeBounds.height() / 2);
                debugDC.DrawEllipse(QBrush(Qt::red), Qt::NoPen, quad.A(), feedbackSize, feedbackSize);
                debugDC.DrawEllipse(QBrush(Qt::blue), Qt::NoPen, quad.D(), feedbackSize, feedbackSize);
            }
#endif
        }
        else
        {
            //we're interested in the A, D points as well as the
            //nodecountour points between them
            QRectF endNodeRect = _operations->GetNodeBounds(_thisNode);

#if DEBUG_RENDERING_FEEDBACK
            if (showFeedback)
            {
                debugDC.DrawRectangle(QBrush(), QPen(Qt::pink , feedbackSize / 2), _operations->GetNodeBounds(_lastNode));
            }
#endif
            QVector<QPointF> vertices = _operations->GetVertices();
            double pressureFactor = _lastNode.PressureFactor();
            int maxCount = vertices.size() * 2;
            int i = 0;
            bool dIsInEndNode = true;
            for (; i < maxCount; i++)
            {
                //look for the d point first
                QPointF point = _lastNode.Position() + (vertices[i % vertices.size()] * pressureFactor);
                if (point == quad.D())
                {
                    //ab always starts with the D position (only add if it's not in endNode's bounds)
                    if (!endNodeRect.contains(quad.D()))
                    {
                        dIsInEndNode = false;
                        abPoints.append(quad.D());
                        dcPoints.append(quad.D());
                    }
#if DEBUG_RENDERING_FEEDBACK
                    if (showFeedback)
                    {
                        debugDC.DrawEllipse(QBrush(Qt::blue), Qt::NoPen, quad.D(), feedbackSize, feedbackSize);
                    }
#endif
                    break;
                }
            }

            if (i == maxCount)
            {
                Debug::Assert(false, "StrokeNodeOperations.GetPointsAtStartOfSegment failed to find the D position");
                //we didn't find the d point, return
                return;
            }


            //now look for the A position
            //advance i
            i++;
            for (int j = 0; i < maxCount && j < vertices.size(); i++, j++)
            {
                //look for the A point now
                QPointF point = _lastNode.Position() + (vertices[i % vertices.size()] * pressureFactor);
                //add everything in between to ab as long as it's not already in endNode's bounds
                if (!endNodeRect.contains(point))
                {
                    abPoints.append(point);
#if DEBUG_RENDERING_FEEDBACK
                if (showFeedback)
                {
                    debugDC.DrawEllipse(QBrush(Qt::wheat), Qt::NoPen, point, feedbackSize, feedbackSize);
                }
#endif
                }
                if (dIsInEndNode)
                {
                    Debug::Assert(!endNodeRect.contains(point));

                    //add the first point after d, clockwise
                    dIsInEndNode = false;
                    dcPoints.append(point);
                }
                if (point == quad.A())
                {
#if DEBUG_RENDERING_FEEDBACK
                    if (showFeedback)
                    {
                        debugDC.DrawEllipse(QBrush(Qt::red), Qt::NoPen, point, feedbackSize, feedbackSize);
                    }
#endif
                    break;
                }
            }
        }
    }
}


/// <summary>
/// GetPointsAtEndOfSegment
/// </summary>
void StrokeNode::GetPointsAtEndOfSegment(  QList<QPointF> &abPoints,
                                        QList<QPointF>& dcPoints
#if DEBUG_RENDERING_FEEDBACK
                                        , DrawingContext& debugDC, double feedbackSize, bool showFeedback
#endif
                                        )
{
    if (IsValid())
    {
        Quad quad = ConnectingQuad();
        if (IsEllipse())
        {
            QRectF bounds = GetBounds();
            //add instructions to arc from B to C
            abPoints.append(quad.B());
            abPoints.append(StrokeRenderer::ArcToMarker);
            abPoints.append(QPointF(bounds.width(), bounds.height()));
            abPoints.append(quad.C());

            //don't add to the dc points
#if DEBUG_RENDERING_FEEDBACK
            if (showFeedback)
            {
                debugDC.DrawEllipse(QBrush(), QPen(Qt::pink, feedbackSize / 2), _thisNode.Position(), bounds.width() / 2, bounds.height() / 2);
                debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad.B(), feedbackSize, feedbackSize);
                debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad.C(), feedbackSize, feedbackSize);
            }
#endif
        }
        else
        {
#if DEBUG_RENDERING_FEEDBACK
            if (showFeedback)
            {
                debugDC.DrawRectangle(QBrush(), QPen(Qt::pink, feedbackSize / 2), GetBounds());
            }
#endif
            //we're interested in the B, C points as well as the
            //nodecountour points between them
            double pressureFactor = _thisNode.PressureFactor();
            QVector<QPointF> vertices = _operations->GetVertices();
            int maxCount = vertices.size() * 2;
            int i = 0;
            for (; i < maxCount; i++)
            {
                //look for the d point first
                QPointF point = _thisNode.Position() + (vertices[i % vertices.size()] * pressureFactor);
                if (point == quad.B())
                {
                    abPoints.append(quad.B());
#if DEBUG_RENDERING_FEEDBACK
                    if (showFeedback)
                    {
                        debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, point, feedbackSize, feedbackSize);
                    }
#endif
                    break;
                }
            }

            if (i == maxCount)
            {
                Debug::Assert(false, "StrokeNodeOperations.GetPointsAtEndOfSegment failed to find the B position");
                //we didn't find the d point, return
                return;
            }

            //now look for the C position
            //advance i
            i++;
            for (int j = 0; i < maxCount && j < vertices.size(); i++, j++)
            {
                //look for the c point last
                QPointF point = _thisNode.Position() + (vertices[i % vertices.size()] * pressureFactor);
                if (point == quad.C())
                {
                    break;
                }
                //only add to ab if we didn't find C
                abPoints.append(point);

#if DEBUG_RENDERING_FEEDBACK
                if (showFeedback)
                {
                    debugDC.DrawEllipse(QBrush(Qt::wheat), Qt::NoPen, quad.C(), feedbackSize, feedbackSize);
                }
#endif
            }
            //finally, add the C point
            dcPoints.append(quad.C());

#if DEBUG_RENDERING_FEEDBACK
            if (showFeedback)
            {
                debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad.C(), feedbackSize, feedbackSize);
            }
#endif
        }
    }
}

/// <summary>
/// GetPointsAtMiddleSegment
/// </summary>
void StrokeNode::GetPointsAtMiddleSegment( StrokeNode & previous,
                                        double angleBetweenNodes,
                                        QList<QPointF> & abPoints,
                                        QList<QPointF> & dcPoints,
                                        bool & missingIntersection
#if DEBUG_RENDERING_FEEDBACK
                                        , DrawingContext& debugDC, double feedbackSize, bool showFeedback
#endif
                                        )
{
    missingIntersection = false;
    if (IsValid() && previous.IsValid())
    {
        Quad quad1 = previous.ConnectingQuad();
        if (!quad1.IsEmpty())
        {
            Quad quad2 = ConnectingQuad();
            if (!quad2.IsEmpty())
            {
                if (IsEllipse())
                {
                    QRectF node1Bounds = _operations->GetNodeBounds(previous._lastNode);
                    QRectF node2Bounds = _operations->GetNodeBounds(_lastNode);
                    QRectF node3Bounds = _operations->GetNodeBounds(_thisNode);
#if DEBUG_RENDERING_FEEDBACK
                    if (showFeedback)
                    {
                        debugDC.DrawEllipse(QBrush(), QPen(Qt::pink, feedbackSize / 2), _lastNode.Position(), node2Bounds.width() / 2, node2Bounds.height() / 2);
                    }
#endif
                    if (angleBetweenNodes == 0.0 || ((quad1.B() == quad2.A()) && (quad1.C() == quad2.D())))
                    {
                        //quads connections are the same, just add them
                        abPoints.append(quad1.B());
                        dcPoints.append(quad1.C());
#if DEBUG_RENDERING_FEEDBACK
                        if (showFeedback)
                        {
                            debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad1.B(), feedbackSize, feedbackSize);
                            debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad1.C(), feedbackSize, feedbackSize);
                        }
#endif
                    }
                    else if (angleBetweenNodes > 0.0)
                    {
                        //the stroke angled towards the AB side
                        //this part is easy
                        if (quad1.B() == quad2.A())
                        {
                            abPoints.append(quad1.B());
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad1.B(), feedbackSize, feedbackSize);
                            }
#endif
                        }
                        else
                        {
                            QPointF intersection = GetIntersection(quad1.A(), quad1.B(), quad2.A(), quad2.B());
                            QRectF union_ = node1Bounds | node2Bounds;
                            //union_.Inflate(1.0, 1.0);
                            union_.adjust(-1.0, -1.0, 1.0, 1.0);
                            //make sure we're not off in space

#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad1.B(), feedbackSize * 1.5, feedbackSize * 1.5);
                                debugDC.DrawEllipse(QBrush(Qt::red), Qt::NoPen, quad2.A(), feedbackSize, feedbackSize);
                            }
#endif

                            if (!DoubleUtil::IsNaN(intersection.x()) && union_.contains(intersection))
                            {
                                abPoints.append(intersection);
#if DEBUG_RENDERING_FEEDBACK
                                if (showFeedback)
                                {
                                    debugDC.DrawEllipse(QBrush(Qt::orange), Qt::NoPen, intersection, feedbackSize, feedbackSize);
                                }
#endif
                            }
                            else
                            {
                                //if we missed the intersection we'll need to close the stroke segment
                                //this work is done in StrokeRenderer
                                missingIntersection = true;
                                return; //we're done.
                            }
                        }

                        if (quad1.C() == quad2.D())
                        {
                            dcPoints.append(quad1.C());
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad1.C(), feedbackSize, feedbackSize);
                            }
#endif
                        }
                        else
                        {
                            //add instructions to arc from quad1.C to quad2.D in reverse order (since we walk this array backwards to render)
                            dcPoints.append(quad1.C());
                            dcPoints.append(QPointF(node2Bounds.width(), node2Bounds.height()));
                            dcPoints.append(StrokeRenderer::ArcToMarker);
                            dcPoints.append(quad2.D());
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad1.C(), feedbackSize, feedbackSize);
                                debugDC.DrawEllipse(QBrush(Qt::blue), Qt::NoPen, quad2.D(), feedbackSize, feedbackSize);
                            }
#endif
                        }
                    }
                    else
                    {
                        //the stroke angled towards the CD side
                        //this part is easy
                        if (quad1.C() == quad2.D())
                        {
                            dcPoints.append(quad1.C());
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad1.C(), feedbackSize, feedbackSize);
                            }
#endif
                        }
                        else
                        {
                            QPointF intersection = GetIntersection(quad1.D(), quad1.C(), quad2.D(), quad2.C());
                            QRectF union_ = node1Bounds | node2Bounds;
                            //union_.Inflate(1.0, 1.0);
                            union_.adjust(-1.0, -1.0, 1.0, 1.0);
                            //make sure we're not off in space

#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad1.C(), feedbackSize * 1.5, feedbackSize * 1.5);
                                debugDC.DrawEllipse(QBrush(Qt::blue), Qt::NoPen, quad2.D(), feedbackSize, feedbackSize);
                            }
#endif

                            if (!DoubleUtil::IsNaN(intersection.x()) && union_.contains(intersection))
                            {
                                dcPoints.append(intersection);
#if DEBUG_RENDERING_FEEDBACK
                                if (showFeedback)
                                {
                                    debugDC.DrawEllipse(QBrush(Qt::orange), Qt::NoPen, intersection, feedbackSize, feedbackSize);
                                }
#endif
                            }
                            else
                            {
                                //if we missed the intersection we'll need to close the stroke segment
                                //this work is done in StrokeRenderer
                                missingIntersection = true;
                                return; //we're done.
                            }
                        }

                        if (quad1.B() == quad2.A())
                        {
                            abPoints.append(quad1.B());
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad1.B(), feedbackSize, feedbackSize);
                            }
#endif

                        }
                        else
                        {
                            //we need to arc between quad1.B and quad2.A along node2
                            abPoints.append(quad1.B());
                            abPoints.append(StrokeRenderer::ArcToMarker);
                            abPoints.append(QPointF(node2Bounds.width(), node2Bounds.height()));
                            abPoints.append(quad2.A());
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad1.B(), feedbackSize, feedbackSize);
                                debugDC.DrawEllipse(QBrush(Qt::red), Qt::NoPen, quad2.A(), feedbackSize, feedbackSize);
                            }
#endif
                        }
                    }
                }
                else
                {
                    //rectangle
                    int indexA = -1;
                    int indexB = -1;
                    int indexC = -1;
                    int indexD = -1;

                    QVector<QPointF> vertices = _operations->GetVertices();
                    double pressureFactor = _lastNode.PressureFactor();
                    for (int i = 0; i < vertices.size(); i++)
                    {
                        QPointF point = _lastNode.Position() + (vertices[i % vertices.size()] * pressureFactor);
                        if (point == quad2.A())
                        {
                            indexA = i;
                        }
                        if (point == quad1.B())
                        {
                            indexB = i;
                        }
                        if (point == quad1.C())
                        {
                            indexC = i;
                        }
                        if (point == quad2.D())
                        {
                            indexD = i;
                        }
                    }

                    if (indexA == -1 || indexB == -1 || indexC == -1 || indexD == -1)
                    {
                        Debug::Assert(false, "Couldn't find all 4 indexes in StrokeNodeOperations.GetPointsAtMiddleSegment");
                        return;
                    }

#if DEBUG_RENDERING_FEEDBACK
                    if (showFeedback)
                    {

                        debugDC.DrawRectangle(QBrush(), QPen(Qt::pink, feedbackSize / 2), _operations->GetNodeBounds(_lastNode));
                        debugDC.DrawEllipse(QBrush(Qt::red), Qt::NoPen, quad2.A(), feedbackSize, feedbackSize);
                        debugDC.DrawEllipse(QBrush(Qt::green), Qt::NoPen, quad1.B(), feedbackSize, feedbackSize);
                        debugDC.DrawEllipse(QBrush(Qt::yellow), Qt::NoPen, quad1.C(), feedbackSize, feedbackSize);
                        debugDC.DrawEllipse(QBrush(Qt::blue), Qt::NoPen, quad2.D(), feedbackSize, feedbackSize);
                    }
#endif

                    QRectF node3Rect = _operations->GetNodeBounds(_thisNode);
                    //take care of a-b first
                    if (indexA == indexB)
                    {
                        //quad connection is the same, just add it
                        if (!node3Rect.contains(quad1.B()))
                        {
                            abPoints.append(quad1.B());
                        }
                    }
                    else if ((indexA == 0 && indexB == 3) || ((indexA != 3 || indexB != 0) && (indexA > indexB)))
                    {
                        if (!node3Rect.contains(quad1.B()))
                        {
                            abPoints.append(quad1.B());
                        }
                        if (!node3Rect.contains(quad2.A()))
                        {
                            abPoints.append(quad2.A());
                        }
                    }
                    else
                    {
                        QPointF intersection = GetIntersection(quad1.A(), quad1.B(), quad2.A(), quad2.B());
                        QRectF node12 = _operations->GetNodeBounds(previous._lastNode) | _operations->GetNodeBounds(_lastNode);
                        node12.adjust(-1.0, -1.0, 1.0, 1.0);
                        //make sure we're not off in space
                        if (!DoubleUtil::IsNaN(intersection.x()) && node12.contains(intersection))
                        {
                            abPoints.append(intersection);
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::orange), Qt::NoPen, intersection, feedbackSize, feedbackSize * 1.5);
                            }
#endif
                        }
                        else
                        {
                            //if we missed the intersection we'll need to close the stroke segment
                            //this work is done in StrokeRenderer.
                            missingIntersection = true;
                            return; //we're done.
                        }
                    }

                    // now take care of c-d.
                    if (indexC == indexD)
                    {
                        //quad connection is the same, just add it
                        if (!node3Rect.contains(quad1.C()))
                        {
                            dcPoints.append(quad1.C());
                        }
                    }
                    else if ((indexC == 0 && indexD == 3) || ((indexC != 3 || indexD != 0) && (indexC > indexD)))
                    {
                        if (!node3Rect.contains(quad1.C()))
                        {
                            dcPoints.append(quad1.C());
                        }
                        if (!node3Rect.contains(quad2.D()))
                        {
                            dcPoints.append(quad2.D());
                        }
                    }
                    else
                    {
                        QPointF intersection = GetIntersection(quad1.D(), quad1.C(), quad2.D(), quad2.C());
                        QRectF node12 = _operations->GetNodeBounds(previous._lastNode) | _operations->GetNodeBounds(_lastNode);
                        node12.adjust(-1.0, -1.0, 1.0, 1.0);
                        //make sure we're not off in space
                        if (!DoubleUtil::IsNaN(intersection.x()) && node12.contains(intersection))
                        {
                            dcPoints.append(intersection);
#if DEBUG_RENDERING_FEEDBACK
                            if (showFeedback)
                            {
                                debugDC.DrawEllipse(QBrush(Qt::orange), Qt::NoPen, intersection, feedbackSize, feedbackSize * 1.5);
                            }
#endif
                        }
                        else
                        {
                            //if we missed the intersection we'll need to close the stroke segment
                            //this work is done in StrokeRenderer.
                            missingIntersection = true;
                            return; //we're done.
                        }
                    }
                }
            }
        }
    }
}

/// <summary>
/// Returns the intersection between two lines.  This code assumes there is an intersection
/// and should only be called if that assumption is valid
/// </summary>
/// <returns></returns>
QPointF StrokeNode::GetIntersection(QPointF line1Start, QPointF line1End, QPointF line2Start, QPointF line2End)
{
    double a1 = line1End.y() - line1Start.y();
    double b1 = line1Start.x() - line1End.x();
    double c1 = (line1End.x() * line1Start.y()) - (line1Start.x() * line1End.y());
    double a2 = line2End.y() - line2Start.y();
    double b2 = line2Start.x() - line2End.x();
    double c2 = (line2End.x() * line2Start.y()) - (line2Start.x() * line2End.y());

    double d = (a1 * b2) - (a2 * b1);
    if (d != 0.0)
    {
        double x = ((b1 * c2) - (b2 * c1)) / d;
        double y = ((a2 * c1) - (a1 * c2)) / d;

        //capture the min and max points
        double line1XMin, line1XMax, line1YMin, line1YMax, line2XMin, line2XMax, line2YMin, line2YMax;
        if (line1Start.x() < line1End.x())
        {
            line1XMin = floor(line1Start.x());
            line1XMax = ceil(line1End.x());
        }
        else
        {
            line1XMin = floor(line1End.x());
            line1XMax = ceil(line1Start.x());
        }

        if (line2Start.x() < line2End.x())
        {
            line2XMin = floor(line2Start.x());
            line2XMax = ceil(line2End.x());
        }
        else
        {
            line2XMin = floor(line2End.x());
            line2XMax = ceil(line2Start.x());
        }

        if (line1Start.y() < line1End.y())
        {
            line1YMin = floor(line1Start.y());
            line1YMax = ceil(line1End.y());
        }
        else
        {
            line1YMin = floor(line1End.y());
            line1YMax = ceil(line1Start.y());
        }

        if (line2Start.y() < line2End.y())
        {
            line2YMin = floor(line2Start.y());
            line2YMax = ceil(line2End.y());
        }
        else
        {
            line2YMin = floor(line2End.y());
            line2YMax = ceil(line2Start.y());
        }


        // now see if we have an intersection between the lines
        // and not just the projection of the lines
        if ((line1XMin <= x && x <= line1XMax) &&
            (line1YMin <= y && y <= line1YMax) &&
            (line2XMin <= x && x <= line2XMax) &&
            (line2YMin <= y && y <= line2YMax))
        {
            return QPointF(x, y);
        }
    }

    if ((long)line1End.x() == (long)line2Start.x() &&
        (long)line1End.y() == (long)line2Start.y())
    {
        return QPointF(line1End.x(), line1End.y());
    }

    return QPointF(nan(""), nan(""));
}

/// <summary>
/// This method tells whether the contour of a given stroke node
/// intersects with the contour of this node. The contours of both nodes
/// include their connecting quadrangles.
/// </summary>
/// <param name="hitNode"></param>
/// <returns></returns>
bool StrokeNode::HitTest(StrokeNode hitNode)
{
    if (!IsValid() || !hitNode.IsValid())
    {
        return false;
    }

    QList<ContourSegment> hittingContour = hitNode.GetContourSegments();

    return _operations->HitTest(_lastNode, _thisNode, ConnectingQuad(), hittingContour);
}

/// <summary>
/// Finds out if a given node intersects with this one,
/// and returns findices of the intersection.
/// </summary>
/// <param name="hitNode"></param>
/// <returns></returns>
StrokeFIndices StrokeNode::CutTest(StrokeNode hitNode)
{
    if ((IsValid() == false) || (hitNode.IsValid() == false))
    {
        return StrokeFIndices::Empty();
    }

    QList<ContourSegment> hittingContour = hitNode.GetContourSegments();

    // If the node contours intersect, the result is a pair of findices
    // this segment should be cut at to let the hitNode's contour through it.
    StrokeFIndices cutAt = _operations->CutTest(_lastNode, _thisNode, ConnectingQuad(), hittingContour);

    return (_index == 0) ? cutAt : BindFIndices(cutAt);
}

/// <summary>
/// Finds out if a given linear segment intersects with the contour of this node
/// (including connecting quadrangle), and returns findices of the intersection.
/// </summary>
/// <param name="begin"></param>
/// <param name="end"></param>
/// <returns></returns>
StrokeFIndices StrokeNode::CutTest(QPointF begin, QPointF end)
{
    if (IsValid() == false)
    {
        return StrokeFIndices::Empty();
    }

    // If the node contours intersect, the result is a pair of findices
    // this segment should be cut at to let the hitNode's contour through it.
    StrokeFIndices cutAt = _operations->CutTest(_lastNode, _thisNode, ConnectingQuad(), begin, end);

    Debug::Assert(!qIsNaN(cutAt.BeginFIndex()) && !qIsNaN(cutAt.EndFIndex()));

    // Bind the found findices to the node and return the result
    return BindFIndicesForLassoHitTest(cutAt);
}

/// <summary>
/// Binds a local fragment to this node by setting the integer part of the
/// fragment findices equal to the index of the previous node
/// </summary>
/// <param name="fragment"></param>
/// <returns></returns>
StrokeFIndices StrokeNode::BindFIndices(StrokeFIndices & fragment)
{
    Debug::Assert(IsValid() && (_index >= 0));

    if (fragment.IsEmpty() == false)
    {
        // Adjust only findices which are on this segment of thew spine (i.e. between 0 and 1)
        if (!DoubleUtil::AreClose(fragment.BeginFIndex(), StrokeFIndices::BeforeFirst))
        {
            Debug::Assert(fragment.BeginFIndex() >= 0 && fragment.BeginFIndex() <= 1);
            fragment.SetBeginFIndex(fragment.BeginFIndex() + _index - 1);
        }
        if (!DoubleUtil::AreClose(fragment.EndFIndex(), StrokeFIndices::AfterLast))
        {
            Debug::Assert(fragment.EndFIndex() >= 0 && fragment.EndFIndex() <= 1);
            fragment.SetEndFIndex(fragment.EndFIndex() + _index - 1);
        }
    }
    return fragment;
}


/// <summary>
/// Bind the StrokeFIndices for lasso hit test results.
/// </summary>
/// <param name="fragment"></param>
/// <returns></returns>
StrokeFIndices StrokeNode::BindFIndicesForLassoHitTest(StrokeFIndices&fragment)
{

    Debug::Assert(IsValid());
    if (!fragment.IsEmpty())
    {
        // Adjust BeginFIndex
        if (DoubleUtil::AreClose(fragment.BeginFIndex(), StrokeFIndices::BeforeFirst))
        {
            // set it to be the index of the previous node, indicating intersection start from previous node
             fragment.SetBeginFIndex(_index == 0 ? StrokeFIndices::BeforeFirst:_index - 1);
        }
        else
        {
            // Adjust findices which are on this segment of the spine (i.e. between 0 and 1)
            Debug::Assert(DoubleUtil::GreaterThanOrClose(fragment.BeginFIndex(), 0));

            Debug::Assert(DoubleUtil::LessThanOrClose(fragment.BeginFIndex(), 1));

            // Adjust the value to consider index, say from 0.75 to 3.75 (for _index = 4)
            fragment.SetBeginFIndex(fragment.BeginFIndex() + _index - 1);
        }

        //Adjust EndFIndex
        if (DoubleUtil::AreClose(fragment.EndFIndex(), StrokeFIndices::AfterLast))
        {
            // set it to be the index of the current node, indicating the intersection cover the end of the node
            fragment.SetEndFIndex(_isLastNode ? StrokeFIndices::AfterLast:_index);
        }
        else
        {
            Debug::Assert(DoubleUtil::GreaterThanOrClose(fragment.EndFIndex(), 0));

            Debug::Assert(DoubleUtil::LessThanOrClose(fragment.EndFIndex(), 1));
            // Ajust the value to consider the index
            fragment.SetEndFIndex(fragment.EndFIndex() + _index - 1);
        }
    }
    return fragment;
}


/// <summary>
/// The quadrangle that connects this and the previous node.
/// Can be empty if this node is the first one or if one of the nodes is
/// completely inside the other.
/// The type Quad is supposed to be internal even if we surface StrokeNode.
/// External users of StrokeNode should use GetConnectionPoints instead.
/// </summary>
Quad & StrokeNode::ConnectingQuad()
{
    Debug::Assert(IsValid());

    if (_isQuadCached == false)
    {
        _connectingQuad = _operations->GetConnectingQuad(_lastNode, _thisNode);
        _isQuadCached = true;
    }
    return _connectingQuad;
}

/// <summary>
/// Returns an enumerator for edges of the contour comprised by the node
/// and connecting quadrangle (_lastNode is excluded)
/// Used for hit-testing a stroke against an other stroke (stroke and point erasing)
/// </summary>
QList<ContourSegment> StrokeNode::GetContourSegments()
{
    Debug::Assert(IsValid());

    // Calls thru to the StrokeNodeOperations object
    if (IsEllipse())
    {
        // ISSUE-2004/06/15- temporary workaround to avoid hit-testing with ellipses
        return _operations->GetNonBezierContourSegments(_lastNode, _thisNode);
    }
    return  _operations->GetContourSegments(_thisNode, ConnectingQuad());
}

/// <summary>
/// Returns the spine point that corresponds to the given findex.
/// </summary>
/// <param name="findex">A local findex between the previous index and this one (ex: between 2.0 and 3.0)</param>
/// <returns>Point on the spine</returns>
QPointF StrokeNode::GetPointAt(double findex)
{
    Debug::Assert(IsValid());

    if (_lastNode.IsEmpty())
    {
        Debug::Assert(findex == 0);
        return _thisNode.Position();
    }

    Debug::Assert((findex >= _index - 1) && (findex <= _index));

    if (DoubleUtil::AreClose(findex, (double)_index))
    {
        //
        // we're being asked for this exact point
        // if we don't return it here, our algorithm
        // below doesn't work
        //
        return _thisNode.Position();
    }

    //
    // get the spare change to the left of the decimal point
    // eg turn 2.75 into .75
    //
    double floor = ::floor(findex);
    findex = findex - floor;

    double xDiff = (_thisNode.Position().x() - _lastNode.Position().x()) * findex;
    double yDiff = (_thisNode.Position().y() - _lastNode.Position().y()) * findex;

    //
    // return the previous point plus the delta's
    //
    return QPointF(   _lastNode.Position().x() + xDiff,
                        _lastNode.Position().y() + yDiff);
}
