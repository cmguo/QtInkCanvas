#include "Internal/Ink/strokenodeoperations.h"
#include "Internal/Ink/ellipticalnodeoperations.h"

/// <summary>
///
/// </summary>
/// <param name="nodeShape"></param>
/// <returns></returns>
StrokeNodeOperations * StrokeNodeOperations::CreateInstance(StylusShape & nodeShape)
{
    if (nodeShape.IsEllipse())
    {
        return new EllipticalNodeOperations(nodeShape);
    }
    return new StrokeNodeOperations(nodeShape);
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="nodeShape">shape of the nodes</param>
StrokeNodeOperations::StrokeNodeOperations(StylusShape & nodeShape)
{
    ////System.Diagnostics.Debug.Assert(nodeShape != null);
    _vertices = nodeShape.GetVerticesAsVectors();
}

StrokeNodeOperations::~StrokeNodeOperations()
{

}

/// <summary>
/// Computes the bounds of a node
/// </summary>
/// <param name="node">node to compute bounds of</param>
/// <returns>bounds of the node</returns>
QRectF StrokeNodeOperations::GetNodeBounds(StrokeNodeData & node)
{
    if (_shapeBounds.isEmpty())
    {
        int i;
        for (i = 0; (i + 1) < _vertices.size(); i += 2)
        {
            _shapeBounds = _shapeBounds.united(QRectF(_vertices[i], _vertices[i + 1]));
        }
        if (i < _vertices.size())
        {
            _shapeBounds = _shapeBounds.united(QRectF(_vertices[i],_vertices[i]));
        }
    }

    QRectF boundingBox = _shapeBounds;
    //System.Diagnostics.Debug.Assert((boundingBox.x() <= 0) && (boundingBox.y() <= 0));

    double pressureFactor = node.PressureFactor();
    if (!DoubleUtil::AreClose(pressureFactor,1))
    {
        boundingBox = QRectF(
            _shapeBounds.x() * pressureFactor,
            _shapeBounds.y() * pressureFactor,
            _shapeBounds.width() * pressureFactor,
            _shapeBounds.height() * pressureFactor);
    }

    boundingBox.translate(node.Position());

    return boundingBox;
}

void StrokeNodeOperations::GetNodeContourPoints(StrokeNodeData node, QList<QPointF> & pointBuffer)
{
    double pressureFactor = node.PressureFactor();
    if (DoubleUtil::AreClose(pressureFactor, 1))
    {
        for (int i = 0; i < _vertices.size(); i++)
        {
            pointBuffer.append(node.Position() + _vertices[i]);
        }
    }
    else
    {
        for (int i = 0; i < _vertices.size(); i++)
        {
            pointBuffer.append(node.Position() + (_vertices[i] * pressureFactor));
        }
    }
}

/// <summary>
/// Returns an enumerator for edges of the contour comprised by a given node
/// and its connecting quadrangle.
/// Used for hit-testing a stroke against an other stroke (stroke and point erasing)
/// </summary>
/// <param name="node">node</param>
/// <param name="quad">quadrangle connecting the node to the preceeding node</param>
/// <returns>contour segments enumerator</returns>
QList<ContourSegment> StrokeNodeOperations::GetContourSegments(StrokeNodeData node, Quad & quad)
{
    //System.Diagnostics.Debug.Assert(node.IsEmpty() == false);

    QList<ContourSegment> result;
    if (quad.IsEmpty())
    {
        QPointF vertex = node.Position() + (_vertices[_vertices.size() - 1] * node.PressureFactor());
        for (int i = 0; i < _vertices.size(); i++)
        {
            QPointF nextVertex = node.Position() + (_vertices[i] * node.PressureFactor());
            result.append(ContourSegment(vertex, nextVertex));
            vertex = nextVertex;
        }
    }
    else
    {
        result.append(ContourSegment(quad.A(), quad.B()));

        for (int i = 0, count = _vertices.size(); i < count; i++)
        {
            QPointF vertex = node.Position() + (_vertices[i] * node.PressureFactor());
            if (vertex == quad.B())
            {
                for (int j = 0; (j < count) && (vertex != quad.C()); j++)
                {
                    i = (i + 1) % count;
                    QPointF nextVertex = node.Position() + (_vertices[i] * node.PressureFactor());
                    result.append(ContourSegment(vertex, nextVertex));
                    vertex = nextVertex;
                }
                break;
            }
        }

        result.append(ContourSegment(quad.C(), quad.D()));
        result.append(ContourSegment(quad.D(), quad.A()));
    }
    return result;
}

/// <summary>
/// ISSUE-2004/06/15- temporary workaround to avoid hit-testing ellipses with ellipses
/// </summary>
/// <param name="beginNode"></param>
/// <param name="endNode"></param>
/// <returns></returns>
QList<ContourSegment> StrokeNodeOperations::GetNonBezierContourSegments(StrokeNodeData beginNode, StrokeNodeData endNode)
{
    Quad quad = beginNode.IsEmpty() ? Quad::Empty() : GetConnectingQuad(beginNode, endNode);
    return GetContourSegments(endNode, quad);
}


/// <summary>
/// Finds connecting points for a pair of stroke nodes (of a polygonal shape)
/// </summary>
/// <param name="beginNode">a node to connect</param>
/// <param name="endNode">another node, next to beginNode</param>
/// <returns>connecting quadrangle, that can be empty if one node is inside the other</returns>
Quad StrokeNodeOperations::GetConnectingQuad(StrokeNodeData & beginNode, StrokeNodeData & endNode)
{
    // Return an empty quad if either of the nodes is empty (not a node)
    // or if both nodes are at the same position.
    if (beginNode.IsEmpty() || endNode.IsEmpty() || DoubleUtil::AreClose(beginNode.Position(), endNode.Position()))
    {
        return Quad::Empty();
    }

    // By definition, Quad's vertices (A,B,C,D) are ordered clockwise with points A and D located
    // on the beginNode and B and C on the endNode. Basically, we're looking for segments AB and CD.
    // We iterate through the vertices of the beginNode, at each vertex we analyze location of the
    // connecting segment relative to the node's edges at the vertex, and enforce these rules:
    //  - if the QPointF of the connecting segment at a vertex V[i] is on the left from QPointF V[i]V[i+1]
    //    and not on the left from QPointF V[i-1]V[i], then it's the AB segment of the quad (V[i] == A).
    //  - if the QPointF of the connecting segment at a vertex V[i] is on the left from QPointF V[i-1]V[i]
    //    and not on the left from QPointF V[i]V[i+1], then it's the CD segment of the quad (V[i] == D).
    //

    Quad quad = Quad::Empty();
    bool foundAB = false, foundCD = false;

    // There's no need to build shapes of the two nodes in order to find their connecting quad.
    // It's the spine QPointF between the nodes and their scaling diff (pressure delta) is all
    // that matters here.
    QPointF spine = endNode.Position() - beginNode.Position();
    double pressureDelta = endNode.PressureFactor() - beginNode.PressureFactor();

    // Iterate through the vertices of the default shape
    int count = _vertices.size();
    for (int i = 0, j = count - 1; i < count; i++, j = ((j + 1) % count))
    {
        // Compute QPointF of the connecting segment at the vertex [i]
        QPointF connection = spine + _vertices[i] * pressureDelta;
        if ((pressureDelta != 0) && (connection.x() == 0) && (connection.y() == 0))
        {
            // One of the nodes,                       |----|
            // as well as the connecting quad,         |__  |
            // is entirely inside the other node.      |  | |
            //                                 [i] --> |__|_|
            return Quad::Empty();
        }

        // Find out where this QPointF is about the node edge [i][i+1]
        // (The vars names "goingTo" and "comingFrom" refer direction of the line defined
        // by the connecting QPointF applied at vertex [i], relative to the contour of the node shape.
        // Using these terms, (comingFrom != Right && goingTo == Left) corresponds to the segment AB,
        // and (comingFrom == Right && goingTo != Left) describes the DC.
        HitResult goingTo = WhereIsVectorAboutVector(connection, _vertices[(i + 1) % count] - _vertices[i]);

        if (goingTo == HitResult::Left)
        {
            if (false == foundAB)
            {
                // Find out where the node edge [i-1][i] is about the connecting QPointF
                HitResult comingFrom = WhereIsVectorAboutVector(_vertices[i] - _vertices[j], connection);
                if (HitResult::Right != comingFrom)
                {
                    foundAB = true;
                    quad.SetA(beginNode.Position() + _vertices[i] * beginNode.PressureFactor());
                    quad.SetB(endNode.Position() + _vertices[i] * endNode.PressureFactor());
                    if (true == foundCD)
                    {
                        // Found all 4 points. Break out from the 'for' loop.
                        break;
                    }
                }
            }
        }
        else
        {
            if (false == foundCD)
            {
                // Find out where the node edge [i-1][i] is about the connecting QPointF
                HitResult comingFrom = WhereIsVectorAboutVector(_vertices[i] - _vertices[j], connection);
                if (HitResult::Right == comingFrom)
                {
                    foundCD = true;
                    quad.SetC(endNode.Position() + _vertices[i] * endNode.PressureFactor());
                    quad.SetD(beginNode.Position() + _vertices[i] * beginNode.PressureFactor());
                    if (true == foundAB)
                    {
                        // Found all 4 points. Break out from the 'for' loop.
                        break;
                    }
                }
            }
        }
    }

    if (!foundAB || !foundCD ||   // (2)
        ((pressureDelta != 0) && Determinant(quad.B() - quad.A(), quad.D() - quad.A()) == 0)) // (1)
    {
        //                                          _____        _______
        // One of the nodes,                    (1) |__  |   (2) | ___  |
        // as well as the connecting quad,          |  | |       | |  | |
        // is entirely inside the other node.       |__| |       | |__| |
        //                                          |____|       |___ __|
        return Quad::Empty();
    }

    return quad;
}

/// <summary>
/// Hit-tests ink segment defined by two nodes against a linear segment.
/// </summary>
/// <param name="beginNode">Begin node of the ink segment</param>
/// <param name="endNode">End node of the ink segment</param>
/// <param name="quad">Pre-computed quadrangle connecting the two ink nodes</param>
/// <param name="hitBeginPoint">Begin point of the hitting segment</param>
/// <param name="hitEndPoint">End point of the hitting segment</param>
/// <returns>true if there's intersection, false otherwise</returns>
bool StrokeNodeOperations::HitTest(
    StrokeNodeData& beginNode, StrokeNodeData& endNode, Quad& quad, QPointF &hitBeginPoint, QPointF &hitEndPoint)
{
    // Check for special cases when the endNode is the very first one (beginNode.IsEmpty())
    // or one node is completely inside the other. In either case the connecting quad
    // would be Empty and we need to hit-test against the biggest node (the one with
    // the greater PressureFactor)
    if (quad.IsEmpty())
    {
        QPointF position;
        double pressureFactor;
        if (beginNode.IsEmpty() || (endNode.PressureFactor() > beginNode.PressureFactor()))
        {
            position = endNode.Position();
            pressureFactor = endNode.PressureFactor();
        }
        else
        {
            position = beginNode.Position();
            pressureFactor = beginNode.PressureFactor();
        }

        // Find the coordinates of the hitting segment relative to the ink node
        QPointF hitBegin = hitBeginPoint - position, hitEnd = hitEndPoint - position;
        if (pressureFactor != 1)
        {
            // Instead of applying pressure to the node, do reverse scaling on
            // the hitting segment. This allows us use the original array of vertices
            // in hit-testing.
            //System.Diagnostics.Debug.Assert(DoubleUtil::IsZero(pressureFactor) == false);
            hitBegin /= pressureFactor;
            hitEnd /= pressureFactor;
        }
        return HitTestPolygonSegment(_vertices, hitBegin, hitEnd);
    }
    else
    {
        // Iterate through the vertices of the contour of the ink segment
        // check where the hitting segment is about them, return false if it's
        // on the outer (left) side of the ink contour. This implementation might
        // look more complex than straightforward separated hit-testing of three
        // polygons (beginNode, quad, endNode), but it's supposed to be more optimal
        // because the number of edges it hit-tests is approximately twice less
        // than with the straightforward implementation.

        // Start with the segment quad.C->quad.D
        QPointF hitBegin = hitBeginPoint - beginNode.Position();
        QPointF hitEnd = hitEndPoint - beginNode.Position();
        HitResult hitResult = WhereIsSegmentAboutSegment(
            hitBegin, hitEnd, quad.C() - beginNode.Position(), quad.D() - beginNode.Position());
        if (HitResult::Left == hitResult)
        {
            return false;
        }

        // Continue clockwise from quad.D to quad.C

        HitResult firstResult = hitResult, lastResult = hitResult;
        double pressureFactor = beginNode.PressureFactor();

        // Find the index of the vertex that is quad.D
        // Use count var to avoid infinite loop, normally it shouldn't
        // happen but it doesn't hurt to check it just in case.
        int i = 0, count = _vertices.size();
        QPointF vertex;
        for (i = 0; i < count; i++)
        {
            vertex = _vertices[i] * pressureFactor;
            // Here and in a few more places down the code, when comparing
            // a quad's vertex vs a scaled shape vertex, it's important to
            // compute them the same way as in GetConnectingQuad, so that not
            // hit that double's computation error. For instance, sometimes the
            // expression (vertex == quad.D - beginNode.Position()) gives 'false'
            // while the expression below gives 'true'. (Another workaround is to
            // use DoubleUtil::AreClose but that;d be less performant)
            if ((beginNode.Position() + vertex) == quad.D())
            {
                break;
            }
        }
        //System.Diagnostics.Debug.Assert(count > 0);
        // This loop does the iteration thru the edges of the ink segment
        // clockwise from quad.D to quad.C.
        for (int node = 0; node < 2; node++)
        {
            QPointF nodePosition = (node == 0) ? beginNode.Position() : endNode.Position();
            QPointF end = (node == 0) ? quad.A() : quad.C();

            count = _vertices.size();
            while (((nodePosition + vertex) != end) && (count != 0))
            {
                i = (i + 1) % _vertices.size();
                QPointF nextVertex = (pressureFactor == 1) ? _vertices[i] : (_vertices[i] * pressureFactor);
                hitResult = WhereIsSegmentAboutSegment(hitBegin, hitEnd, vertex, nextVertex);
                if (HitResult::Hit == hitResult)
                {
                    return true;
                }
                if (true == IsOutside(hitResult, lastResult))
                {
                    return false;
                }
                lastResult = hitResult;
                vertex = nextVertex;
                count--;
            }
            //System.Diagnostics.Debug.Assert(count > 0);

            if (node == 0)
            {
                // The first iteration is done thru the outer segments of beginNode
                // and ends at quad.A, for the second one make some adjustments
                // to continue iterating through quad.AB and the outer segments of
                // endNode up to quad.C
                pressureFactor = endNode.PressureFactor();

                QPointF spineQPointF = endNode.Position() - beginNode.Position();
                vertex -= spineQPointF;
                hitBegin -= spineQPointF;
                hitEnd -= spineQPointF;

                // Find the index of the vertex that is quad.B
                count = _vertices.size();
                while (((endNode.Position() + _vertices[i] * pressureFactor) != quad.B()) && (count != 0))
                {
                    i = (i + 1) % _vertices.size();
                    count--;
                }
                //System.Diagnostics.Debug.Assert(count > 0);
                i--;
            }
        }
        return (false == IsOutside(firstResult, hitResult));
    }
}

/// <summary>
/// Hit-tests a stroke segment defined by two nodes against another stroke segment.
/// </summary>
/// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
/// <param name="endNode">End node of the stroke segment</param>
/// <param name="quad">Pre-computed quadrangle connecting the two nodes.
/// Can be empty if the begion node is empty or when one node is entirely inside the other</param>
/// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
/// <returns>true if the contours intersect or overlap</returns>
bool StrokeNodeOperations::HitTest(
    StrokeNodeData& beginNode, StrokeNodeData& endNode, Quad& quad, QList<ContourSegment> hitContour)
{
    // Check for special cases when the endNode is the very first one (beginNode.IsEmpty())
    // or one node is completely inside the other. In either case the connecting quad
    // would be Empty and we need to hittest against the biggest node (the one with
    // the greater PressureFactor)
    if (quad.IsEmpty())
    {
        // NTRAID#Window OS bug-1029694-2004/10/15-xiaotu, refactor the code to make it a method
        // to increase the maintainability of the program. FxCop bug.

        // Make a call to hit-test the biggest node the hitting contour.
        return HitTestPolygonContourSegments(hitContour, beginNode, endNode);
    }
    else
    {
        // NTRAID#Window OS bug-1029694-2004/10/15-xiaotu, refactor the code to make it a method
        // to increase the maintainability of the program. FxCop bug.

        // HitTest the the hitting contour against the inking contour
        return HitTestInkContour(hitContour, quad, beginNode, endNode);
    }
}

/// <summary>
/// Hit-tests ink segment defined by two nodes against a linear segment.
/// </summary>
/// <param name="beginNode">Begin node of the ink segment</param>
/// <param name="endNode">End node of the ink segment</param>
/// <param name="quad">Pre-computed quadrangle connecting the two ink nodes</param>
/// <param name="hitBeginPoint">Begin point of the hitting segment</param>
/// <param name="hitEndPoint">End point of the hitting segment</param>
/// <returns>Exact location to cut at represented by StrokeFIndices</returns>
StrokeFIndices StrokeNodeOperations::CutTest(
    StrokeNodeData& beginNode, StrokeNodeData& endNode, Quad& quad, QPointF &hitBeginPoint, QPointF &hitEndPoint)
{
    StrokeFIndices result = StrokeFIndices::Empty();

    // First, find out if the hitting segment intersects with either of the ink nodes
    for (int node = (beginNode.IsEmpty() ? 1 : 0); node < 2; node++)
    {
        QPointF position = (node == 0) ? beginNode.Position() : endNode.Position();
        double pressureFactor = (node == 0) ? beginNode.PressureFactor() : endNode.PressureFactor();

        // Adjust the segment for the node's pressure factor
        QPointF hitBegin = hitBeginPoint - position;
        QPointF hitEnd = hitEndPoint - position;
        if (pressureFactor != 1)
        {
            //System.Diagnostics.Debug.Assert(DoubleUtil::IsZero(pressureFactor) == false);
            hitBegin /= pressureFactor;
            hitEnd /= pressureFactor;
        }
        // Hit-test the node against the segment
        if (true == HitTestPolygonSegment(_vertices, hitBegin, hitEnd))
        {
            if (node == 0)
            {
                result.SetBeginFIndex(StrokeFIndices::BeforeFirst);
                result.SetEndFIndex(0);
            }
            else
            {
                result.SetEndFIndex(StrokeFIndices::AfterLast);
                if (beginNode.IsEmpty())
                {
                    result.SetBeginFIndex(StrokeFIndices::BeforeFirst);
                }
                else if (result.BeginFIndex() != StrokeFIndices::BeforeFirst)
                {
                    result.SetBeginFIndex(1);
                }
            }
        }
    }

    // If both nodes are hit, return.
    if (result.IsFull())
    {
        return result;
    }
    // If there's no hit at all, return.
    if (result.IsEmpty() && (quad.IsEmpty() || !HitTestQuadSegment(quad, hitBeginPoint, hitEndPoint)))
    {
        return result;
    }

    // The segments do intersect. Find findices on the ink segment to cut it at.
    if (result.BeginFIndex() != StrokeFIndices::BeforeFirst)
    {
        // The begin node is not hit, i.e. the begin findex is on this spine segment, find it.
        result.SetBeginFIndex(ClipTest(
            (endNode.Position() - beginNode.Position()) / beginNode.PressureFactor(),
            (endNode.PressureFactor() / beginNode.PressureFactor()) - 1,
            (hitBeginPoint - beginNode.Position()) / beginNode.PressureFactor(),
            (hitEndPoint - beginNode.Position()) / beginNode.PressureFactor()));
    }

    if (result.EndFIndex() != StrokeFIndices::AfterLast)
    {
        // The end node is not hit, i.e. the end findex is on this spine segment, find it.
        result.SetEndFIndex(1 - ClipTest(
            (beginNode.Position() - endNode.Position()) / endNode.PressureFactor(),
            (beginNode.PressureFactor() / endNode.PressureFactor()) - 1,
            (hitBeginPoint - endNode.Position()) / endNode.PressureFactor(),
            (hitEndPoint - endNode.Position()) / endNode.PressureFactor()));
    }

    if (IsInvalidCutTestResult(result))
    {
        return StrokeFIndices::Empty();
    }

    return result;
}

/// <summary>
/// CutTest
/// </summary>
/// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
/// <param name="endNode">End node of the stroke segment</param>
/// <param name="quad">Pre-computed quadrangle connecting the two nodes.
/// Can be empty if the begion node is empty or when one node is entirely inside the other</param>
/// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
/// <returns></returns>
StrokeFIndices StrokeNodeOperations::CutTest(
    StrokeNodeData& beginNode, StrokeNodeData& endNode, Quad& quad, QList<ContourSegment>& hitContour)
{
    if (beginNode.IsEmpty())
    {
        if (HitTest(beginNode, endNode, quad, hitContour) == true)
        {
            return StrokeFIndices::Full();
        }
        return StrokeFIndices::Empty();
    }

    StrokeFIndices result = StrokeFIndices::Empty();
    bool isInside = true;
    QPointF spineQPointF = (endNode.Position() - beginNode.Position()) / beginNode.PressureFactor();
    QPointF spineQPointFReversed = (beginNode.Position() - endNode.Position()) / endNode.PressureFactor();
    double pressureDelta = (endNode.PressureFactor() / beginNode.PressureFactor()) - 1;
    double pressureDeltaReversed = (beginNode.PressureFactor() / endNode.PressureFactor()) - 1;

    for (ContourSegment& hitSegment : hitContour)
    {
        // NTRAID#Window OS bug-1029694-2004/10/19-xiaotu, refactor the code to make it a method
        // to increase the maintainability of the program. FxCop bug.

        // First, find out if hitSegment intersects with either of the ink nodes
        bool isHit = HitTestStrokeNodes(hitSegment,beginNode,endNode, result);

        // If both nodes are hit, return.
        if (result.IsFull())
        {
            return result;
        }

        // If neither of the nodes is hit, hit-test the connecting quad
        if (isHit == false)
        {
            // If neither of the nodes is hit and the contour of one node is entirely
            // inside the contour of the other node, then done with this hitting segment
            if (!quad.IsEmpty())
            {
                isHit = hitSegment.IsArc()
                     ? HitTestQuadCircle(quad, hitSegment.Begin() + hitSegment.Radius(), hitSegment.Radius())
                     : HitTestQuadSegment(quad, hitSegment.Begin(), hitSegment.End());
            }

            if (isHit == false)
            {
                if (isInside == true)
                {
                    isInside = hitSegment.IsArc()
                        ? (WhereIsVectorAboutArc(endNode.Position() - hitSegment.Begin() - hitSegment.Radius(),
                            -hitSegment.Radius(), hitSegment.Vector() - hitSegment.Radius()) != HitResult::Hit)
                        : (WhereIsVectorAboutVector(
                            endNode.Position() - hitSegment.Begin(), hitSegment.Vector()) == HitResult::Right);
                }
                continue;
            }
        }

        isInside = false;

        // NTRAID#Window OS bug-1029694-2004/10/15-xiaotu, refactor the code to make it a new method
        // CalculateClipLocation to increase the maintainability of the program. FxCop bug.

        // If the begin node is not hit, find the begin findex on the ink segment to cut it at
        if (!DoubleUtil::AreClose(result.BeginFIndex(), StrokeFIndices::BeforeFirst))
        {
            double findex = CalculateClipLocation(hitSegment, beginNode, spineQPointF, pressureDelta);
            if (findex != StrokeFIndices::BeforeFirst)
            {
                //System.Diagnostics.Debug.Assert(findex >= 0 && findex <= 1);
                if (result.BeginFIndex() > findex)
                {
                    result.SetBeginFIndex(findex);
                }
            }
        }

        // If the end node is not hit, find the end findex on the ink segment to cut it at
        if (!DoubleUtil::AreClose(result.EndFIndex(), StrokeFIndices::AfterLast))
        {
            double findex = CalculateClipLocation(hitSegment, endNode, spineQPointFReversed, pressureDeltaReversed);
            if (findex != StrokeFIndices::BeforeFirst)
            {
                //System.Diagnostics.Debug.Assert(findex >= 0 && findex <= 1);
                findex = 1 - findex;
                if (result.EndFIndex() < findex)
                {
                    result.SetEndFIndex(findex);
                }
            }
        }
    }

    if (DoubleUtil::AreClose(result.BeginFIndex(), StrokeFIndices::AfterLast))
    {
        if (!DoubleUtil::AreClose(result.EndFIndex(), StrokeFIndices::BeforeFirst))
        {
            result.SetBeginFIndex(StrokeFIndices::BeforeFirst);
        }
    }
    else if (DoubleUtil::AreClose(result.EndFIndex(), StrokeFIndices::BeforeFirst))
    {
        result.SetEndFIndex(StrokeFIndices::AfterLast);
    }

    if (IsInvalidCutTestResult(result))
    {
        return StrokeFIndices::Empty();
    }

    return (result.IsEmpty() && isInside) ? StrokeFIndices::Full() : result;
}

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
double StrokeNodeOperations::ClipTest(QPointF const  &spineQPointF, double pressureDelta, QPointF const & hitBegin, QPointF const & hitEnd)
{
    // Let's represent the vertices for the startNode are N1, N2, ..., Ni and for the endNode, M1, M2,
    // ..., Mi.
    // When ink tip shape is a convex polygon, one may iterate in a constant direction
    // (for instance, clockwise) through the edges of the polygon P1 and hit test the cutting segment
    // against quadrangles NIMIMI+1NI+1 with MI on the left side off the QPointF NINI+1.
    // If the cutting segment intersects the quadrangle, on the intersected part of the segment,
    // one may then find point Q (the nearest to the line NINI+1) and point QP
    // (the point of the intersection of the segment NIMI and QPointF NI+1NI started at Q).
    // Next,
    //                      QP = NI + s * LengthOf(MI - NI)                         (1)
    //                      s = LengthOf(QP - NI ) / LengthOf(MI - NI).                     (2)
    // If the cutting segment intersects more than one quadrant, one may then use the smallest s
    // to find the split point:
    //                      S = P1 + s * LengthOf(P2 - P1)                          (3)
    double findex = StrokeFIndices::AfterLast;
    QPointF hitQPointF = hitEnd - hitBegin;
    QPointF lastVertex = _vertices[_vertices.size() - 1];

    // Note the definition of pressureDelta = (endNode.PressureFactor() / beginNode.PressureFactor()) - 1
    // So the equation below gives
    //   nextNode = spineQPointF + (endNode.PressureFactor() / beginNode.PressureFactor())*lastVertex - lastVertex
    // As a result, nextNode is a QPointF pointing from lastVertex of the beginNode to the correspoinding "lastVertex"
    // of the endNode.
    QPointF nextNode = spineQPointF + lastVertex * pressureDelta;
    bool testNextEdge = false;

    for (int k = 0, count = _vertices.size(); k < count || (k == count && testNextEdge); k++)
    {
        QPointF vertex = _vertices[k % count];
        QPointF nextVertex = vertex - lastVertex;

        // Point from vertex in beginNode to the corresponding "vertex" in endNode
        QPointF nextVertexNextNode = spineQPointF + (vertex * pressureDelta);

        // Find out a "nextNode" on the endNode (nextNode) that is on the left side off the QPointF
        // (lastVertex, vertex).
        if ((DoubleUtil::IsZero(nextNode.x()) && DoubleUtil::IsZero(nextNode.y())) ||
            (!testNextEdge && (HitResult::Left != WhereIsVectorAboutVector(nextNode, nextVertex))))
        {
            lastVertex = vertex;
            nextNode = nextVertexNextNode;
            continue;
        }

        // Now we need to do hit testing of the hitting segment against quarangle (NI, MI, MI+1, NI+1),
        // that is, (lastVertex, nextNode, nextVertexNextNode, vertex)

        testNextEdge = false;
        HitResult hit = HitResult::Left;
        int side = 0;
        for (int i = 0; i < 2; i++)
        {
            QPointF hitPoint = ((0 == i) ? hitBegin : hitEnd) - lastVertex;

            hit = WhereIsVectorAboutVector(hitPoint, nextNode);
            if (hit == HitResult::Hit)
            {
                double r = (qAbs(nextNode.x()) < qAbs(nextNode.y())) //DoubleUtil::IsZero(nextNode.x())
                    ? (hitPoint.y() / nextNode.y())
                    : (hitPoint.x() / nextNode.x());
                if ((findex > r) && DoubleUtil::IsBetweenZeroAndOne(r))
                {
                    findex = r;
                }
            }
            else if (hit == HitResult::Right)
            {
                 side++;
                if (HitResult::Left == WhereIsVectorAboutVector(
                    hitPoint - nextVertex, nextVertexNextNode))
                {
                    double r = GetPositionBetweenLines(nextVertex, nextNode, hitPoint);
                    if ((findex > r) && DoubleUtil::IsBetweenZeroAndOne(r))
                    {
                        findex = r;
                    }
                }
                else
                {
                    testNextEdge = true;
                }
            }
            else
            {
                side--;
            }
        }

        //
        if (0 == side)
        {
            if (hit == HitResult::Hit)
            {
                // This segment is collinear with the edge connecting the nodes,
                // no need to hit-test the other edges.
                //System.Diagnostics.Debug.Assert(true == DoubleUtil::IsBetweenZeroAndOne(findex));
                break;
            }
            // The hitting segment intersects the line of the edge connecting
            // the nodes. Find the findex of the intersection point.
            double det = -Determinant(nextNode, hitQPointF);
            if (DoubleUtil::IsZero(det) == false)
            {
                double s = Determinant(hitQPointF, hitBegin - lastVertex) / det;
                if ((findex > s) && DoubleUtil::IsBetweenZeroAndOne(s))
                {
                    findex = s;
                }
            }
        }
        //
        lastVertex = vertex;
        nextNode = nextVertexNextNode;
    }
    return AdjustFIndex(findex);
}

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
double StrokeNodeOperations::ClipTestArc(QPointF const & spineQPointF, double pressureDelta, QPointF const & hitCenter, QPointF const & hitRadius)
{
    (void)spineQPointF;
    (void)pressureDelta;
    (void)hitCenter;
    (void)hitRadius;
    // NTRAID#WINDOWS-1448096-2006/1/9-SAMGEO, this code is not called, but will be in VNext
    throw std::exception();
    /*
    double findex = StrokeFIndices::AfterLast;

    double radiusSquared = hitRadius.size()Squared;
    QPointF vertex, lastVertex = _vertices[_vertices.size() - 1];
    QPointF nextVertexNextNode, nextNode = spineQPointF + lastVertex * pressureDelta;
    bool testNextEdge = false;

    for (int k = 0, count = _vertices.size();
        k < count || (k == count && testNextEdge);
        k++, lastVertex = vertex, nextNode = nextVertexNextNode)
    {
        vertex = _vertices[k % count];
        QPointF nextVertex = vertex - lastVertex;
        nextVertexNextNode = spineQPointF + (vertex * pressureDelta);

        if (DoubleUtil::IsZero(nextNode.x()) && DoubleUtil::IsZero(nextNode.y()))
        {
            continue;
        }

        bool testConnectingEdge = false;

        if (HitResult::Left == WhereIsVectorAboutVector(nextNode, nextVertex))
        {
            testNextEdge = false;

            QPointF normal = GetProjection(lastVertex - hitCenter, vertex - hitCenter);
            if (radiusSquared <= normal.size()Squared)
            {
                if (WhereIsVectorAboutVector(hitCenter - lastVertex, nextVertex) == HitResult::Left)
                {
                    QPointF hitPoint = hitCenter + (normal * sqrt(radiusSquared / normal.size()Squared));
                    if (HitResult::Right == WhereIsVectorAboutVector(hitPoint - vertex, nextVertexNextNode))
                    {
                        testNextEdge = true;
                    }
                    else if (HitResult::Left == WhereIsVectorAboutVector(hitPoint - lastVertex, nextNode))
                    {
                        testConnectingEdge = true;
                    }
                    else
                    {
                        // this is it
                        findex = GetPositionBetweenLines(nextVertex, nextNode, hitPoint - lastVertex);
                        //System.Diagnostics.Debug.Assert(DoubleUtil::IsBetweenZeroAndOne(findex));
                        break;
                    }
                }
            }
            else if (HitResult::Right == WhereIsVectorAboutVector(hitCenter + normal - lastVertex, nextNode))
            {
                testNextEdge = true;
            }
            else
            {
                testConnectingEdge = true;
            }
        }
        else if (testNextEdge == true)
        {
            testNextEdge = false;
            testConnectingEdge = true;
        }

        if (testConnectingEdge)
        {
            // Find out the projection of hitCenter on nextNode
            QPointF v = lastVertex - hitCenter;
            double findexNearest = GetProjectionFIndex(v, v + nextNode);

            if (findexNearest > 0)
            {
                QPointF nearest = nextNode * findexNearest;
                double squaredDistanceFromNearestToHitPoint = radiusSquared - (nearest + v).size()Squared;
                if (DoubleUtil::IsZero(squaredDistanceFromNearestToHitPoint) && (findexNearest <= 1))
                {
                    if (findexNearest < findex)
                    {
                        findex = findexNearest;
                    }
                }
                else if ((squaredDistanceFromNearestToHitPoint > 0)
                    && (nearest.size()Squared >= squaredDistanceFromNearestToHitPoint))
                {
                    double hitPointFIndex = findexNearest - sqrt(
                        squaredDistanceFromNearestToHitPoint / nextNode.size()Squared);
                    //System.Diagnostics.Debug.Assert(DoubleUtil::GreaterThanOrClose(hitPointFIndex, 0));
                    if (hitPointFIndex < findex)
                    {
                        findex = hitPointFIndex;
                    }
                }
            }
        }
    }

    return AdjustFIndex(findex);
    */
}


/// <summary>
/// Helper function to hit-test the biggest node against hitting contour segments
/// </summary>
/// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
/// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
/// <param name="endNode">End node of the stroke segment</param>
/// <returns>true if hit; false otherwise</returns>
bool StrokeNodeOperations::HitTestPolygonContourSegments(
    QList<ContourSegment>& hitContour, StrokeNodeData& beginNode, StrokeNodeData& endNode)
{
    bool isHit = false;

    // The bool variable isInside is used here to track that case. It answers to
    // 'Is ink contour inside if the hitting contour?'. It's initialized to 'true"
    // and then verified for each edge of the hitting contour until there's a hit or
    // until it's false.
    bool isInside = true;

    QPointF position;
    double pressureFactor;
    if (beginNode.IsEmpty() || endNode.PressureFactor() > beginNode.PressureFactor())
    {
        position = endNode.Position();
        pressureFactor = endNode.PressureFactor();
    }
    else
    {
        position = beginNode.Position();
        pressureFactor = beginNode.PressureFactor();
    }

    // Enumerate through the segments of the hitting contour and test them
    // one by one against the contour of the ink node.
    for (ContourSegment & hitSegment : hitContour)
    {
        if (hitSegment.IsArc())
        {
            // Adjust the arc for the node' pressure factor.
            QPointF hitCenter = hitSegment.Begin() + hitSegment.Radius() - position;
            QPointF hitRadius = hitSegment.Radius();
            if (!DoubleUtil::AreClose(pressureFactor, 1))
            {
                //System.Diagnostics.Debug.Assert(DoubleUtil::IsZero(pressureFactor) == false);
                hitCenter /= pressureFactor;
                hitRadius /= pressureFactor;
            }
            // If the segment is an arc, hit-test against the entire circle the arc is part of.
            if (true == HitTestPolygonCircle(_vertices, hitCenter, hitRadius))
            {
                isHit = true;
                break;
            }
            //
            if (isInside && (WhereIsVectorAboutArc(
                position - hitSegment.Begin() - hitSegment.Radius(),
                -hitSegment.Radius(), hitSegment.Vector() - hitSegment.Radius()) == HitResult::Hit))
            {
                isInside = false;
            }
        }
        else
        {
            // Adjust the segment for the node's pressure factor
            QPointF hitBegin = hitSegment.Begin() - position;
            QPointF hitEnd = hitBegin + hitSegment.Vector();
            if (!DoubleUtil::AreClose(pressureFactor, 1))
            {
                //System.Diagnostics.Debug.Assert(DoubleUtil::IsZero(pressureFactor) == false);
                hitBegin /= pressureFactor;
                hitEnd /= pressureFactor;
            }
            // Hit-test the node against the segment
            if (true == HitTestPolygonSegment(_vertices, hitBegin, hitEnd))
            {
                isHit = true;
                break;
            }
            //
            if (isInside && WhereIsVectorAboutVector(
                position - hitSegment.Begin(), hitSegment.Vector()) != HitResult::Right)
            {
                isInside = false;
            }
        }
    }
    return (isInside || isHit);
}

/// <summary>
/// Helper function to HitTest the the hitting contour against the inking contour
/// </summary>
/// <param name="hitContour">a collection of basic segments outlining the hitting contour</param>
/// <param name="quad">A connecting quad</param>
/// <param name="beginNode">Begin node of the stroke segment to hit-test. Can be empty (none)</param>
/// <param name="endNode">End node of the stroke segment</param>
/// <returns>true if hit; false otherwise</returns>
bool StrokeNodeOperations::HitTestInkContour(
    QList<ContourSegment>& hitContour, Quad& quad, StrokeNodeData& beginNode, StrokeNodeData &endNode)
{
    //System.Diagnostics.Debug.Assert(!quad.IsEmpty());
    bool isHit = false;

    // When hit-testing a contour against another contour, like in this case,
    // the default implementation checks whether any edge (segment) of the hitting
    // contour intersects with the contour of the ink segment. But this doesn't cover
    // the case when the ink segment is entirely inside of the hitting segment.
    // The bool variable isInside is used here to track that case. It answers to
    // 'Is ink contour inside if the hitting contour?'. It's initialized to 'true"
    // and then verified for each edge of the hitting contour until there's a hit or
    // until it's false.
    bool isInside = true;

    // The ink connecting quad is not empty, enumerate through the segments of the
    // hitting contour and hit-test them one by one against the ink contour.
    for (ContourSegment& hitSegment : hitContour)
    {
        // Iterate through the vertices of the contour of the ink segment
        // check where the hit segment is about them, return false if it's
        // on the left side off either of the ink contour segments.

        QPointF hitBegin, hitEnd;
        HitResult hitResult;

        // Start with the segment quad.C->quad.D
        if (hitSegment.IsArc())
        {
            hitBegin = hitSegment.Begin() + hitSegment.Radius() - beginNode.Position();
            hitEnd = hitSegment.Radius();
            hitResult = WhereIsCircleAboutSegment(
                hitBegin, hitEnd, quad.C() - beginNode.Position(), quad.D() - beginNode.Position());
        }
        else
        {
            hitBegin = hitSegment.Begin() - beginNode.Position();
            hitEnd = hitBegin + hitSegment.Vector();
            hitResult = WhereIsSegmentAboutSegment(
                hitBegin, hitEnd, quad.C() - beginNode.Position(), quad.D() - beginNode.Position());
        }
        if (HitResult::Left == hitResult)
        {
            if (isInside)
            {
                isInside = hitSegment.IsArc()
                    ? (WhereIsVectorAboutArc(-hitBegin, -hitSegment.Radius(), hitSegment.Vector() - hitSegment.Radius()) != HitResult::Hit)
                    : (WhereIsVectorAboutVector(-hitBegin, hitSegment.Vector()) == HitResult::Right);
            }
            // This hitSegment is completely outside of the ink contour,
            // continue with the next one.
            continue;
        }

        // Continue clockwise from quad.D to quad.A, then to quad.B, ..., quad.C

        HitResult firstResult = hitResult, lastResult = hitResult;
        double pressureFactor = beginNode.PressureFactor();

        // Find the index of the vertex that is quad.D
        // Use count var to avoid infinite loop, normally this shouldn't
        // happen but it doesn't hurt to check it just in case.
        int i = 0, count = _vertices.size();
        QPointF vertex;
        for (i = 0; i < count; i++)
        {
            vertex = _vertices[i] * pressureFactor;
            if (DoubleUtil::AreClose((beginNode.Position() + vertex), quad.D()))
            {
                break;
            }
        }
        //System.Diagnostics.Debug.Assert(i < count);

        int k;
        for (k = 0; k < 2; k++)
        {
            count = _vertices.size();
            QPointF nodePosition = (k == 0) ? beginNode.Position() : endNode.Position();
            QPointF end = (k == 0) ? quad.A() : quad.C();

            // Iterate over the vertices on
            //          beginNode(k=0)from quad.D to quad.A
            //    or
            //          endNode(k=1)from quad.A to quad.B ... to quad.C
            while (((nodePosition + vertex) != end) && (count != 0))
            {
                // Find out the next vertex
                i = (i + 1) % _vertices.size();
                QPointF nextVertex = _vertices[i] * pressureFactor;

                // Hit-test the hitting segment against the current edge
                hitResult = hitSegment.IsArc()
                    ? WhereIsCircleAboutSegment(hitBegin, hitEnd, vertex, nextVertex)
                    : WhereIsSegmentAboutSegment(hitBegin, hitEnd, vertex, nextVertex);

                if (HitResult::Hit == hitResult)
                {
                    return true;  //Got a hit
                }
                if (true == IsOutside(hitResult, lastResult))
                {
                    // This hitSegment is definitely outside the ink contour, drop it.
                    // Change k to something > 2 to leave the for loop and skip
                    // IsOutside at the bottom
                    k = 3;
                    break;
                }
                lastResult = hitResult;
                vertex = nextVertex;
                count--;
            }
            //System.Diagnostics.Debug.Assert(count > 0);

            if (k == 0)
            {
                // Make some adjustments for the second one to continue iterating through
                // quad.AB and the outer segments of endNode up to quad.C
                pressureFactor = endNode.PressureFactor();
                QPointF spineQPointF = endNode.Position() - beginNode.Position();
                vertex -= spineQPointF; // now vertex = quad.A - spineQPointF
                hitBegin -= spineQPointF; // adjust hitBegin to the space of endNode
                if (hitSegment.IsArc() == false)
                {
                    hitEnd -= spineQPointF;
                }

                // Find the index of the vertex that is quad.B
                count = _vertices.size();
                while (!DoubleUtil::AreClose((endNode.Position() + _vertices[i] * pressureFactor), quad.B()) && (count != 0))
                {
                    i = (i + 1) % _vertices.size();
                    count--;
                }
                //System.Diagnostics.Debug.Assert(count > 0);
                i--;
            }
        }
        if ((k == 2) && (false == IsOutside(firstResult, hitResult)))
        {
            isHit = true;
            break;
        }
        //
        if (isInside)
        {
            isInside = hitSegment.IsArc()
                ? (WhereIsVectorAboutArc(-hitBegin, -hitSegment.Radius(), hitSegment.Vector() - hitSegment.Radius()) != HitResult::Hit)
                : (WhereIsVectorAboutVector(-hitBegin, hitSegment.Vector()) == HitResult::Right);
        }
    }
    return (isHit||isInside);
}


/// <summary>
/// Helper function to Hit-test against the two stroke nodes only (excluding the connecting quad).
/// </summary>
/// <param name="hitSegment"></param>
/// <param name="beginNode"></param>
/// <param name="endNode"></param>
/// <param name="result"></param>
/// <returns></returns>
bool StrokeNodeOperations::HitTestStrokeNodes(
    ContourSegment& hitSegment, StrokeNodeData &beginNode, StrokeNodeData &endNode, StrokeFIndices& result)
{
    // First, find out if hitSegment intersects with either of the ink nodes
    bool isHit = false;
    for (int node = 0; node < 2; node++)
    {
        QPointF position;
        double pressureFactor;
        if (node == 0)
        {
            if (isHit && DoubleUtil::AreClose(result.BeginFIndex(), StrokeFIndices::BeforeFirst))
            {
                continue;
            }
            position = beginNode.Position();
            pressureFactor = beginNode.PressureFactor();
        }
        else
        {
            if (isHit && DoubleUtil::AreClose(result.EndFIndex(), StrokeFIndices::AfterLast))
            {
                continue;
            }
            position = endNode.Position();
            pressureFactor = endNode.PressureFactor();
        }

        QPointF hitBegin, hitEnd;

        // Adjust the segment for the node's pressure factor
        if (hitSegment.IsArc())
        {
            hitBegin = hitSegment.Begin() - position + hitSegment.Radius();
            hitEnd = hitSegment.Radius();
        }
        else
        {
            hitBegin = hitSegment.Begin() - position;
            hitEnd = hitBegin + hitSegment.Vector();
        }

        if (pressureFactor != 1)
        {
            //System.Diagnostics.Debug.Assert(DoubleUtil::IsZero(pressureFactor) == false);
            hitBegin /= pressureFactor;
            hitEnd /= pressureFactor;
        }
        // Hit-test the node against the segment
        if (hitSegment.IsArc()
            ? HitTestPolygonCircle(_vertices, hitBegin, hitEnd)
            : HitTestPolygonSegment(_vertices, hitBegin, hitEnd))
        {
            isHit = true;
            if (node == 0)
            {
                result.SetBeginFIndex(StrokeFIndices::BeforeFirst);
                if (DoubleUtil::AreClose(result.EndFIndex(), StrokeFIndices::AfterLast))
                {
                    break;
                }
            }
            else
            {
                result.SetEndFIndex(StrokeFIndices::AfterLast);
                if (beginNode.IsEmpty())
                {
                    result.SetBeginFIndex(StrokeFIndices::BeforeFirst);
                    break;
                }
                if (DoubleUtil::AreClose(result.BeginFIndex(), StrokeFIndices::BeforeFirst))
                {
                    break;
                }
            }
        }
    }
    return isHit;
}

/// <summary>
///  Calculate the clip location
/// </summary>
/// <param name="hitSegment">the hitting segment</param>
/// <param name="beginNode">begin node</param>
/// <param name="spineQPointF"></param>
/// <param name="pressureDelta"></param>
/// <returns>the clip location. not-clip if return StrokeFIndices::BeforeFirst</returns>
double StrokeNodeOperations::CalculateClipLocation(
    ContourSegment & hitSegment, StrokeNodeData& beginNode, QPointF& spineQPointF, double pressureDelta)
{
    double findex = StrokeFIndices::BeforeFirst;
    bool clipIt = hitSegment.IsArc() ? true
        //? (WhereIsVectorAboutArc(beginNode.Position() - hitSegment.Begin() - hitSegment.Radius(),
        //            -hitSegment.Radius(), hitSegment.QPointF - hitSegment.Radius()) == HitResult::Hit)
        : (WhereIsVectorAboutVector(
                           beginNode.Position() - hitSegment.Begin(), hitSegment.Vector()) == HitResult::Left);
    if (clipIt)
    {
        findex = hitSegment.IsArc()
            ? ClipTestArc(spineQPointF, pressureDelta,
                (hitSegment.Begin() + hitSegment.Radius() - beginNode.Position()) / beginNode.PressureFactor(),
                hitSegment.Radius() / beginNode.PressureFactor())
            : ClipTest(spineQPointF, pressureDelta,
                (hitSegment.Begin() - beginNode.Position()) / beginNode.PressureFactor(),
                (hitSegment.End() - beginNode.Position()) / beginNode.PressureFactor());

        // NTRAID#WINDOWS-1384646-2005/11/17-WAYNEZEN,
        // ClipTest returns StrokeFIndices::AfterLast to indicate a false hit test.
        // But the caller CutTest expects StrokeFIndices::BeforeFirst when there is no hit.
        if ( findex == StrokeFIndices::AfterLast )
        {
            findex = StrokeFIndices::BeforeFirst;
        }
        else
        {
            //System.Diagnostics.Debug.Assert(findex >= 0 && findex <= 1);
        }
    }
    return findex;
}

/// <summary>
/// Helper method used to determine if we came up with a bogus result during hit testing
/// </summary>
bool StrokeNodeOperations::IsInvalidCutTestResult(StrokeFIndices & result)
{
    //
    // check for three invalid states
    // 1) BeforeFirst == AfterLast
    // 2) BeforeFirst, < 0
    // 3) > 1, AfterLast
    //
    if (DoubleUtil::AreClose(result.BeginFIndex(), result.EndFIndex()) ||
        DoubleUtil::AreClose(result.BeginFIndex(), StrokeFIndices::BeforeFirst) && result.EndFIndex() < 0.0f ||
        result.BeginFIndex() > 1.0f && DoubleUtil::AreClose(result.EndFIndex(), StrokeFIndices::AfterLast))
    {
        return true;
    }
    return false;
}

/// <summary>
/// Hit-tests a linear segment against a convex polygon.
/// </summary>
/// <param name="vertices">Vertices of the polygon (in clockwise order)</param>
/// <param name="hitBegin">an end point of the hitting segment</param>
/// <param name="hitEnd">an end point of the hitting segment</param>
/// <returns>true if hit; false otherwise</returns>
bool StrokeNodeOperations::HitTestPolygonSegment(QVector<QPointF> & vertices, QPointF& hitBegin, QPointF& hitEnd)
{
    //System.Diagnostics.Debug.Assert((null != vertices) && (2 < vertices.Length));

    HitResult hitResult = HitResult::Right, firstResult = HitResult::Right, prevResult = HitResult::Right;
    int count = vertices.size();
    QPointF vertex = vertices[count - 1];
    for (int i = 0; i < count; i++)
    {
        QPointF nextVertex = vertices[i];
        hitResult = WhereIsSegmentAboutSegment(hitBegin, hitEnd, vertex, nextVertex);
        if (HitResult::Hit == hitResult)
        {
            return true;
        }
        if (IsOutside(hitResult, prevResult))
        {
            return false;
        }
        if (i == 0)
        {
            firstResult = hitResult;
        }
        prevResult = hitResult;
        vertex = nextVertex;
    }
    return (false == IsOutside(firstResult, hitResult));
}

/// <summary>
/// This is a specialized version of HitTestPolygonSegment that takes
/// a Quad for a polygon. This method is called very intensively by
/// hit-testing API and we don't want to create Vector[] for every quad it hit-tests.
/// </summary>
/// <param name="quad">the connecting quad to test against</param>
/// <param name="hitBegin">begin point of the hitting segment</param>
/// <param name="hitEnd">end point of the hitting segment</param>
/// <returns>true if hit, false otherwise</returns>
bool StrokeNodeOperations::HitTestQuadSegment(Quad & quad, QPointF const& hitBegin, QPointF const& hitEnd)
{
    //System.Diagnostics.Debug.Assert(quad.IsEmpty == false);

    HitResult hitResult = HitResult::Right, firstResult = HitResult::Right, prevResult = HitResult::Right;
    int count = 4;
    QPointF zeroVector(0, 0);
    QPointF hitVector = hitEnd - hitBegin;
    QPointF vertex = quad[count - 1] - hitBegin;

    for (int i = 0; i < count; i++)
    {
        QPointF nextVertex = quad[i] - hitBegin;
        hitResult = WhereIsSegmentAboutSegment(zeroVector, hitVector, vertex, nextVertex);
        if (HitResult::Hit == hitResult)
        {
            return true;
        }
        if (true == IsOutside(hitResult, prevResult))
        {
            return false;
        }
        if (i == 0)
        {
            firstResult = hitResult;
        }
        prevResult = hitResult;
        vertex = nextVertex;
    }
    return (false == IsOutside(firstResult, hitResult));
}

/// <summary>
/// Hit-test a polygin against a circle
/// </summary>
/// <param name="vertices">Vectors representing the vertices of the polygon, ordered in clockwise order</param>
/// <param name="center">QPointF &representing the center of the circle</param>
/// <param name="radius">QPointF &representing the radius of the circle</param>
/// <returns>true if hit, false otherwise</returns>
bool StrokeNodeOperations::HitTestPolygonCircle(QVector<QPointF>& vertices, QPointF& center, QPointF& radius)
{
    (void) vertices;
    (void) center;
    (void) radius;
    // NTRAID#WINDOWS-1448096-2006/1/9-SAMGEO, this code is not called, but will be in VNext
    throw new std::exception();
    /*
    //System.Diagnostics.Debug.Assert((null != vertices) && (2 < vertices.Length));

    HitResult hitResult = HitResult::Right, firstResult = HitResult::Right, prevResult = HitResult::Right;
    int count = vertices.Length;
    QPointF vertex = vertices[count - 1];

    for (int i = 0; i < count; i++)
    {
        QPointF nextVertex = vertices[i];
        hitResult = WhereIsCircleAboutSegment(center, radius, vertex, nextVertex);
        if (HitResult::Hit == hitResult)
        {
            return true;
        }
        if (true == IsOutside(hitResult, prevResult))
        {
            return false;
        }
        if (i == 0)
        {
            firstResult = hitResult;
        }
        prevResult = hitResult;
        vertex = nextVertex;
    }
    return (false == IsOutside(firstResult, hitResult));
    */
}

/// <summary>
/// This is a specialized version of HitTestPolygonCircle that takes
/// a Quad for a polygon. This method is called very intensively by
/// hit-testing API and we don't want to create Vector[] for every quad it hit-tests.
/// </summary>
/// <param name="quad">the connecting quad</param>
/// <param name="center">center of the circle</param>
/// <param name="radius">radius of the circle </param>
/// <returns>true if hit; false otherwise</returns>
bool StrokeNodeOperations::HitTestQuadCircle(Quad const& quad, QPointF const& center, QPointF const& radius)
{
    (void) quad;
    (void) center;
    (void) radius;
    // NTRAID#WINDOWS-1448096-2006/1/9-SAMGEO, this code is not called, but will be in VNext
    throw new std::exception();
    /*
    //System.Diagnostics.Debug.Assert(quad.IsEmpty == false);

    QPointF centerQPointF &= (Vector)center;
    HitResult hitResult = HitResult::Right, firstResult = HitResult::Right, prevResult = HitResult::Right;
    int count = 4;
    QPointF vertex = (Vector)quad[count - 1];

    for (int i = 0; i < count; i++)
    {
        QPointF nextVertex = (Vector)quad[i];
        hitResult = WhereIsCircleAboutSegment(centerVector, radius, vertex, nextVertex);
        if (HitResult::Hit == hitResult)
        {
            return true;
        }
        if (true == IsOutside(hitResult, prevResult))
        {
            return false;
        }
        if (i == 0)
        {
            firstResult = hitResult;
        }
        prevResult = hitResult;
        vertex = nextVertex;
    }
    return (false == IsOutside(firstResult, hitResult));
    */
}


/// <summary>
/// Finds out where the segment [hitBegin, hitEnd]
/// is about the segment [orgBegin, orgEnd].
/// </summary>
StrokeNodeOperations::HitResult StrokeNodeOperations::WhereIsSegmentAboutSegment(
    QPointF const & hitBegin, QPointF const & hitEnd, QPointF const & orgBegin, QPointF const & orgEnd)
{
    if (hitEnd == hitBegin)
    {
        return WhereIsCircleAboutSegment(hitBegin, QPointF(0, 0), orgBegin, orgEnd);
    }

    //----------------------------------------------------------------------
    // Source: http://isc.faqs.org/faqs/graphics/algorithms-faq/
    // Subject 1.03: How do I find intersections of 2 2D line segments?
    //
    // Let A,B,C,D be 2-space position vectors.  Then the directed line
    // segments AB & CD are given by:
    //
    // AB=A+r(B-A), r in [0,1]
    // CD=C+s(D-C), s in [0,1]
    //
    // If AB & CD intersect, then
    //
    // A+r(B-A)=C+s(D-C), or  Ax+r(Bx-Ax)=Cx+s(Dx-Cx)
    // Ay+r(By-Ay)=Cy+s(Dy-Cy)  for some r,s in [0,1]
    //
    // Solving the above for r and s yields
    //
    //      (Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy)
    //  r = -----------------------------  (eqn 1)
    //      (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
    //
    //      (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
    //  s = -----------------------------  (eqn 2)
    //      (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
    //
    // Let P be the position QPointF of the intersection point, then
    //
    //  P=A+r(B-A) or Px=Ax+r(Bx-Ax) and Py=Ay+r(By-Ay)
    //
    // By examining the values of r & s, you can also determine some
    // other limiting conditions:
    //  If 0 <= r <= 1 && 0 <= s <= 1, intersection exists
    //  r < 0 or r > 1 or s < 0 or s > 1 line segments do not intersect
    //  If the denominator in eqn 1 is zero, AB & CD are parallel
    //  If the numerator in eqn 1 is also zero, AB & CD are collinear.
    //  If they are collinear, then the segments may be projected to the x-
    //  or y-axis, and overlap of the projected intervals checked.
    //
    // If the intersection point of the 2 lines are needed (lines in this
    // context mean infinite lines) regardless whether the two line
    // segments intersect, then
    //  If r > 1, P is located on extension of AB
    //  If r < 0, P is located on extension of BA
    //  If s > 1, P is located on extension of CD
    //  If s < 0, P is located on extension of DC
    // Also note that the denominators of eqn 1 & 2 are identical.
    //
    // References:
    // [O'Rourke (C)] pp. 249-51
    // [Gems III] pp. 199-202 "Faster Line Segment Intersection,"
    //----------------------------------------------------------------------
    // The result tells where the segment CD is about the QPointF AB.
    // Return "Right" if either C or D is not on the left from AB.
    HitResult result = HitResult::Right;

    // Calculate the vectors.
    QPointF AB = orgEnd - orgBegin;          // B - A
    QPointF CA = orgBegin - hitBegin;        // A - C
    QPointF CD = hitEnd - hitBegin;          // D - C
    double det = Determinant(AB, CD);

    if (DoubleUtil::IsZero(det))
    {
        // The segments are parallel.
        /*if (DoubleUtil::IsZero(Determinant(CD, CA)))
        {
            // The segments are collinear.
            // Check if their X and Y projections overlap.
            if ((Math.Max(orgBegin.X, orgEnd.x()) >= Math.Min(hitBegin.X, hitEnd.x())) &&
                (Math.Min(orgBegin.X, orgEnd.x()) <= Math.Max(hitBegin.X, hitEnd.x())) &&
                (Math.Max(orgBegin.Y, orgEnd.y()) >= Math.Min(hitBegin.Y, hitEnd.y())) &&
                (Math.Min(orgBegin.Y, orgEnd.y()) <= Math.Max(hitBegin.Y, hitEnd.y())))
            {
                // The segments overlap.
                result = HitResult::Hit;
            }
            else if (false == DoubleUtil::IsZero(AB.x()))
            {
                result = ((AB.X * CA.x()) > 0) ? HitResult::Behind : HitResult::InFront;
            }
            else
            {
                result = ((AB.Y * CA.y()) > 0) ? HitResult::Behind : HitResult::InFront;
            }
        }
        else */
        if (DoubleUtil::IsZero(Determinant(CD, CA)) || DoubleUtil::GreaterThan(Determinant(AB, CA), 0))
        {
            // C is on the left from AB, and, since the segments are parallel, D is also on the left.
            result = HitResult::Left;
        }
    }
    else
    {
        double r = AdjustFIndex(Determinant(AB, CA) / det);

        if (r > 0 && r < 1)
        {
            // The line defined AB does cross the segment CD.
            double s = AdjustFIndex(Determinant(CD, CA) / det);
            if (s > 0 && s < 1)
            {
                // The crossing point is on the segment AB as well.
                result = HitResult::Hit;
            }
            else
            {
                result = (0 < s) ? HitResult::InFront : HitResult::Behind;
            }
        }
        else if ((WhereIsVectorAboutVector(hitBegin - orgBegin, AB) == HitResult::Left)
            || (WhereIsVectorAboutVector(hitEnd - orgBegin, AB) == HitResult::Left))
        {
            // The line defined AB doesn't cross the segment CD, and neither C nor D
            // is on the right from AB
            result = HitResult::Left;
        }
    }

    return result;
}

/// <summary>
/// Find out the relative location of a circle relative to a line segment
/// </summary>
/// <param name="center">center of the circle</param>
/// <param name="radius">radius of the circle. center.radius is a point on the circle</param>
/// <param name="segBegin">begin point of the line segment</param>
/// <param name="segEnd">end point of the line segment</param>
/// <returns>test result</returns>
StrokeNodeOperations::HitResult StrokeNodeOperations::WhereIsCircleAboutSegment(
    QPointF center, QPointF radius, QPointF segBegin, QPointF segEnd)
{
    segBegin -= center;
    segEnd -= center;
    double radiusSquared = LengthSquared(radius);

    // This will find out the nearest path from center to a point on the segment
    double distanceSquared = LengthSquared(GetNearest(segBegin, segEnd));

    // The segment must cross the circle, hit
    if (radiusSquared > distanceSquared)
    {
        return HitResult::Hit;
    }

    QPointF segVector = segEnd - segBegin;
    HitResult result = HitResult::Right;

    // NTRAID#Tablet PC bugs-26556-2004/10/19-xiaotu,resolved two issues with the original code:
    // 1. The local varial "normal" is assigned a value but it is never used afterwards. \
    // 2.  the code indicates that that only case result is HitResult::InFront or HitResult::Behind is
    //  when WhereIsVectorAboutVector(-segBegin, segVector) == HitResult::Left.

    HitResult vResult = WhereIsVectorAboutVector(-segBegin, segVector);

    //either front or behind
    if (vResult == HitResult::Hit)
    {
        result = DoubleUtil::LessThan(LengthSquared(segBegin), LengthSquared(segEnd)) ? HitResult::InFront :
            HitResult::Behind;
    }
    else
    {
        // Find the projection of center on the segment.
        double findex = GetProjectionFIndex(segBegin, segEnd);

        // Get the normal vector, pointing from center to the projection point
        QPointF normal = segBegin + (segVector * findex);

        // recalculate distanceSquared using normal
        distanceSquared = QPointF::dotProduct(normal, normal);

        // The extension of the segment won't hit the circle
        if (radiusSquared <= distanceSquared)
        {
            // either left or right
            result = vResult;
        }
        else
        {
            result = (findex > 0) ? HitResult::InFront : HitResult::Behind;
        }
    }

    return result;
}

/// <summary>
/// Finds out where the vector1 is about the vector2.
/// </summary>
StrokeNodeOperations::HitResult StrokeNodeOperations::WhereIsVectorAboutVector(QPointF const &vector1, QPointF const & vector2)
{
    double determinant = Determinant(vector1, vector2);
    if (DoubleUtil::IsZero(determinant))
    {
        return HitResult::Hit;   // collinear
    }
    return (0 < determinant) ? HitResult::Left : HitResult::Right;
}

/// <summary>
/// Tells whether the hitQPointF &intersects the arc defined by two vectors.
/// </summary>
StrokeNodeOperations::HitResult StrokeNodeOperations::WhereIsVectorAboutArc(QPointF const &hitVector, QPointF const & arcBegin, QPointF const & arcEnd)
{
    //HitResult result = HitResult::Right;
    if (arcBegin == arcEnd)
    {
        // full circle
        return HitResult::Hit;
    }

    if (HitResult::Right == WhereIsVectorAboutVector(arcEnd, arcBegin))
    {
        // small arc
        if ((HitResult::Left != WhereIsVectorAboutVector(hitVector, arcBegin)) &&
            (HitResult::Right != WhereIsVectorAboutVector(hitVector, arcEnd)))
        {
            return HitResult::Hit;
        }
    }
    else if ((HitResult::Left != WhereIsVectorAboutVector(hitVector, arcBegin)) ||
            (HitResult::Right != WhereIsVectorAboutVector(hitVector, arcEnd)))
    {
        return HitResult::Hit;
    }

    if ((WhereIsVectorAboutVector(hitVector - arcBegin, TurnLeft(arcBegin)) != HitResult::Left) ||
        (WhereIsVectorAboutVector(hitVector - arcEnd, TurnRight(arcEnd)) != HitResult::Right))
    {
        return HitResult::Left;
    }

    return HitResult::Right;
}

/// <summary>
///
/// </summary>
/// <param name="vector"></param>
/// <returns></returns>
QPointF StrokeNodeOperations::TurnLeft(QPointF const &vector)
{
    (void) vector;
    // NTRAID#WINDOWS-1448096-2006/1/9-SAMGEO, this code is not called, but will be in VNext
    throw std::exception();
    //return new Vector(-vector.Y, vector.x());
}

/// <summary>
///
/// </summary>
/// <param name="vector"></param>
/// <returns></returns>
QPointF StrokeNodeOperations::TurnRight(QPointF const &vector)
{
    (void) vector;
    // NTRAID#WINDOWS-1448096-2006/1/9-SAMGEO, this code is not called, but will be in VNext
    throw std::exception();
    //return new Vector(vector.Y, -vector.x());
}

/// <summary>
///
/// </summary>
/// <param name="hitResult"></param>
/// <param name="prevHitResult"></param>
/// <returns></returns>
bool StrokeNodeOperations::IsOutside(HitResult hitResult, HitResult prevHitResult)
{
    // ISSUE-2004/10/08-XiaoTu For Polygon and Circle, ((HitResult::Behind == hitResult) && (HitResult::InFront == prevHitResult))
    // cannot be true.
    return ((HitResult::Left == hitResult)
        || ((HitResult::Behind == hitResult) && (HitResult::InFront == prevHitResult)));
}

/// <summary>
/// helper function to find out the ratio of the distance from hitpoint to lineVector
/// and the distance from lineQPointF &to (lineVector+nextLine)
/// </summary>
/// <param name="linesVector">This is one edge of a polygonal node</param>
/// <param name="nextLine">The connection QPointF between the same edge on biginNode and ednNode</param>
/// <param name="hitPoint">a point</param>
/// <returns>the relative position of hitPoint</returns>
double StrokeNodeOperations::GetPositionBetweenLines(QPointF &linesVector, QPointF nextLine, QPointF hitPoint)
{
    QPointF nearestOnFirst = GetProjection(-hitPoint, linesVector - hitPoint);

    hitPoint = nextLine - hitPoint;
    QPointF nearestOnSecond = GetProjection(hitPoint, hitPoint + linesVector);

    QPointF shortest = nearestOnFirst - nearestOnSecond;
    //System.Diagnostics.Debug.Assert((false == DoubleUtil::IsZero(shortest.x())) || (false == DoubleUtil::IsZero(shortest.y())));

    //return DoubleUtil::IsZero(shortest.x()) ? (nearestOnFirst.Y / shortest.y()) : (nearestOnFirst.X / shortest.x());
    return sqrt(LengthSquared(nearestOnFirst) / LengthSquared(shortest));
}

/// <summary>
/// On a line defined buy two points finds the findex of the point
/// nearest to the origin (0,0). Same as FindNearestOnLine just
/// different output.
/// </summary>
/// <param name="begin">A point on the line.</param>
/// <param name="end">Another point on the line.</param>
/// <returns></returns>
double StrokeNodeOperations::GetProjectionFIndex(QPointF const &begin, QPointF const& end)
{
    QPointF segment = end - begin;
    double lengthSquared = QPointF::dotProduct(segment, segment);

    if (DoubleUtil::IsZero(lengthSquared))
    {
        return 0;
    }

    double dotProduct = -QPointF::dotProduct(begin, segment);
    return AdjustFIndex(dotProduct / lengthSquared);
}

/// <summary>
/// On a line defined buy two points finds the point nearest to the origin (0,0).
/// </summary>
/// <param name="begin">A point on the line.</param>
/// <param name="end">Another point on the line.</param>
/// <returns></returns>
QPointF StrokeNodeOperations::GetProjection(QPointF const &begin, QPointF const& end)
{
    double findex = GetProjectionFIndex(begin, end);
    return (begin + (end - begin) * findex);
}

/// <summary>
/// On a given segment finds the point nearest to the origin (0,0).
/// </summary>
/// <param name="begin">The segment's begin point.</param>
/// <param name="end">The segment's end point.</param>
/// <returns></returns>
QPointF StrokeNodeOperations::GetNearest(QPointF const &begin, QPointF const & end)
{
    double findex = GetProjectionFIndex(begin, end);
    if (findex <= 0)
    {
        return begin;
    }
    if (findex >= 1)
    {
        return end;
    }
    return (begin + ((end - begin) * findex));
}

/// <summary>
/// Clears double's computation fuzz around 0 and 1
/// </summary>
double StrokeNodeOperations::AdjustFIndex(double findex)
{
    return DoubleUtil::IsZero(findex) ? 0 : (DoubleUtil::IsOne(findex) ? 1 : findex);
}
