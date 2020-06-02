#include "Internal/Ink/strokerenderer.h"
#include "Internal/Ink/strokenode.h"
#include "Internal/Ink/strokenodeiterator.h"
#include "Windows/Media/streamgeometrycontext.h"
#include "Windows/Media/streamgeometry.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"


INKCANVAS_BEGIN_NAMESPACE

Point StrokeRenderer::ArcToMarker(-DBL_MAX, -DBL_MAX);

/// <summary>
/// Calculate the StreamGeometry for the StrokeNodes.
/// This method is one of our most sensitive perf paths.  It has been optimized to
/// create the minimum path figures in the StreamGeometry.  There are two structures
/// we create for each point in a stroke, the strokenode and the connecting quad.  Adding
/// strokenodes is very expensive later when MIL renders it, so this method has been optimized
/// to only add strokenodes when either pressure changes, or the angle of the stroke changes.
/// </summary>
void StrokeRenderer::CalcGeometryAndBoundsWithTransform(StrokeNodeIterator& iterator,
                                                   DrawingAttributes& drawingAttributes,
                                                   MatrixTypes stylusTipMatrixType,
                                                   bool calculateBounds,
                                                   Geometry*& geometry,
                                                   Rect& bounds)
{
    //Debug::Assert(iterator != nullptr);
    //Debug::Assert(drawingAttributes != nullptr);
    (void) drawingAttributes;

    StreamGeometry* streamGeometry= new StreamGeometry;
    streamGeometry->SetFillRule(FillRule::Nonzero);

    StreamGeometryContext& context = streamGeometry->Open();
    geometry = streamGeometry;
    bounds = Rect();

    //try
    {
        FinallyHelper final([&context](){
            context.Close();
            //geometry.Freeze();
        });

        List<Point> connectingQuadPoints; connectingQuadPoints.reserve(iterator.Count() * 4);

        //the index that the cb quad points are copied to
        int cdIndex = iterator.Count() * 2;
        //the index that the ab quad points are copied to
        int abIndex = 0;
        for (int x = 0; x < cdIndex; x++)
        {
            //initialize so we can start copying to cdIndex later
            connectingQuadPoints.Add(Point(0, 0));
        }

        List<Point> strokeNodePoints;
        double lastAngle = 0.0;
        bool previousPreviousNodeRendered = false;

        Rect lastRect(0, 0, 0, 0);

        for (int index = 0; index < iterator.Count(); index++)
        {
            StrokeNode strokeNode = iterator[index];
            //System.Diagnostics.Debug::Assert(true == strokeNode.IsValid());

            //the only code that calls this with !calculateBounds
            //is dynamic rendering, which already draws enough strokeNodes
            //to hide any visual artifacts.
            //static rendering calculatesBounds, and we use those
            //bounds below to figure out what angle to lay strokeNodes down for.
            Rect strokeNodeBounds = strokeNode.GetBounds();
            if (calculateBounds)
            {
                bounds.Union(strokeNodeBounds);
            }

            //if the angle between this and the last position has changed
            //too much relative to the angle between the last+1 position and the last position
            //we need to lay down stroke node
            double delta = Math::Abs(GetAngleDeltaFromLast(strokeNode.PreviousPosition(), strokeNode.Position(), lastAngle));

            double angleTolerance = 45;
            if (stylusTipMatrixType == MatrixTypes::TRANSFORM_IS_UNKNOWN)
            {
                //probably a skew is thrown in, we need to fall back to being very conservative
                //about how many strokeNodes we prune
                angleTolerance = 10;
            }
            else if (strokeNodeBounds.Height() > 40 || strokeNodeBounds.Width() > 40)
            {
                //if the strokeNode gets above a certain size, we need to lay down more strokeNodes
                //to prevent visual artifacts
                angleTolerance = 20;
            }
            bool directionChanged = delta > angleTolerance && delta < (360 - angleTolerance);

            double prevArea = lastRect.Height() * lastRect.Width();
            double currArea = strokeNodeBounds.Height() * strokeNodeBounds.Width();
            bool areaChangedOverThreshold = false;
            if ((Math::Min(prevArea, currArea) / Math::Max(prevArea, currArea)) <= 0.70)
            {
                //the min area is < 70% of the max area
                areaChangedOverThreshold = true;
            }

            lastRect = strokeNodeBounds;

            //render the stroke node for the first two nodes and last two nodes always
            if (index <= 1 || index >= iterator.Count() - 2 || directionChanged || areaChangedOverThreshold)
            {
                //special case... the direction has changed and we need to
                //insert a stroke node in the StreamGeometry before we render the current one
                if (directionChanged && !previousPreviousNodeRendered && index > 1 && index < iterator.Count() - 1)
                {
                    //insert a stroke node for the previous node
                    strokeNodePoints.Clear();
                    strokeNode.GetPreviousContourPoints(strokeNodePoints);
                    AddFigureToStreamGeometryContext(context, strokeNodePoints, strokeNode.IsEllipse()/*isBezierFigure*/);

                    previousPreviousNodeRendered = true;
                }

                //render the stroke node
                strokeNodePoints.Clear();
                strokeNode.GetContourPoints(strokeNodePoints);
                AddFigureToStreamGeometryContext(context, strokeNodePoints, strokeNode.IsEllipse()/*isBezierFigure*/);
            }

            if (!directionChanged)
            {
                previousPreviousNodeRendered = false;
            }

            //add the end points of the connecting quad
            Quad quad = strokeNode.GetConnectingQuad();
            if (!quad.IsEmpty())
            {
                connectingQuadPoints[abIndex++] = quad.A();
                connectingQuadPoints[abIndex++] = quad.B();
                connectingQuadPoints.Add(quad.D());
                connectingQuadPoints.Add(quad.C());
            }

            if (strokeNode.IsLastNode())
            {
                Debug::Assert(index == iterator.Count() - 1);
                if (abIndex > 0)
                {
                    //we added something to the connecting quad points.
                    //now we need to do three things
                    //1) Shift the dc points down to the ab points
                    int cbStartIndex = iterator.Count() * 2;
                    int cbEndIndex = connectingQuadPoints.Count() - 1;
                    for (int i = abIndex, j = cbStartIndex; j <= cbEndIndex; i++, j++)
                    {
                        connectingQuadPoints[i] = connectingQuadPoints[j];
                    }

                    //2) trim the exess off the end of the array
                    int countToRemove = cbStartIndex - abIndex;
                    connectingQuadPoints.RemoveRange((cbEndIndex - countToRemove) + 1, countToRemove);

                    //3) reverse the dc points to make them cd points
                    for (int i = abIndex, j = connectingQuadPoints.Count() - 1; i < j; i++, j--)
                    {
                        Point temp = connectingQuadPoints[i];
                        connectingQuadPoints[i] = connectingQuadPoints[j];
                        connectingQuadPoints[j] = temp;
                    }

                    //now render away!
                    AddFigureToStreamGeometryContext(context, connectingQuadPoints, false/*isBezierFigure*/);
                }
            }
        }
    }
    //finally
    //{
    //    context.Close();
    //    geometry.Freeze();
    //}
}


/// <summary>
/// Calculate the StreamGeometry for the StrokeNodes.
/// This method is one of our most sensitive perf paths.  It has been optimized to
/// create the minimum path figures in the StreamGeometry.  There are two structures
/// we create for each point in a stroke, the strokenode and the connecting quad.  Adding
/// strokenodes is very expensive later when MIL renders it, so this method has been optimized
/// to only add strokenodes when either pressure changes, or the angle of the stroke changes.
/// </summary>
//[FriendAccessAllowed]
void StrokeRenderer::CalcGeometryAndBounds(StrokeNodeIterator& iterator,
                                           DrawingAttributes& drawingAttributes,
#if DEBUG_RENDERING_FEEDBACK
                                           DrawingContext& debugDC,
                                           double feedbackSize,
                                           bool showFeedback,
#endif
                                           bool calculateBounds,
                                           Geometry*& geometry,
                                           Rect &bounds)
{

    Debug::Assert(iterator != nullptr /*&& drawingAttributes != nullptr*/);

    //we can use our new algorithm for identity only.
    Matrix stylusTipTransform(drawingAttributes.StylusTipTransform());
    if (stylusTipTransform != Matrix::Identity() && stylusTipTransform._type != MatrixTypes::TRANSFORM_IS_SCALING)
    {
        //second best optimization
        CalcGeometryAndBoundsWithTransform(iterator, drawingAttributes, stylusTipTransform._type, calculateBounds, geometry, bounds);
    }
    else
    {

        StreamGeometry * streamGeometry = new StreamGeometry;
        streamGeometry->SetFillRule(FillRule::Nonzero);

        StreamGeometryContext& context = streamGeometry->Open();
        geometry = streamGeometry;
        Rect empty;
        bounds = empty;
        //try
        {
            FinallyHelper final([&context](){
                context.Close();
                //geometry.Freeze();
            });
            //
            // We keep track of three StrokeNodes as we iterate across
            // the Stroke. Since these are structs, the default ctor will
            // be called and .IsValid will be false until we initialize them
            //
            StrokeNode emptyStrokeNode;
            StrokeNode prevPrevStrokeNode;
            StrokeNode prevStrokeNode;
            StrokeNode strokeNode;

            Rect prevPrevStrokeNodeBounds = empty;
            Rect prevStrokeNodeBounds = empty;
            Rect strokeNodeBounds = empty;

            //percentIntersect is a function of drawingAttributes height / width
            double percentIntersect = 95;
            double maxExtent = Math::Max(drawingAttributes.Height(), drawingAttributes.Width());
            percentIntersect += Math::Min(4.99999, ((maxExtent / 20) * 5));

            double prevAngle = -DBL_MAX;
            bool isStartOfSegment = true;
            bool isEllipse = drawingAttributes.GetStylusTip() == StylusTip::Ellipse;
            bool ignorePressure = drawingAttributes.IgnorePressure();
            //
            // Two List<Point>'s that get reused for adding figures
            // to the streamgeometry.
            //
            List<Point> pathFigureABSide;//don't prealloc.  It causes Gen2 collections to rise and doesn't help execution time
            List<Point> pathFigureDCSide;
            List<Point> polyLinePoints;//(4);

            int iteratorCount = iterator.Count();
            for (int index = 0, previousIndex = -1; index < iteratorCount; )
            {
                if (!prevPrevStrokeNode.IsValid())
                {
                    if (prevStrokeNode.IsValid())
                    {
                        //we're sliding our pointers forward
                        prevPrevStrokeNode = prevStrokeNode;
                        prevPrevStrokeNodeBounds = prevStrokeNodeBounds;
                        prevStrokeNode = emptyStrokeNode;
                    }
                    else
                    {
                        prevPrevStrokeNode = iterator.GetNode(index++, previousIndex++);
                        prevPrevStrokeNodeBounds = prevPrevStrokeNode.GetBounds();
                        continue; //so we always check if index < iterator.Count
                    }
                }

                //we know prevPrevStrokeNode is valid
                if (!prevStrokeNode.IsValid())
                {
                    if (strokeNode.IsValid())
                    {
                        //we're sliding our pointers forward
                        prevStrokeNode = strokeNode;
                        prevStrokeNodeBounds = strokeNodeBounds;
                        strokeNode = emptyStrokeNode;
                    }
                    else
                    {
                        //get the next strokeNode, but don't automatically update previousIndex
                        prevStrokeNode = iterator.GetNode(index++, previousIndex);
                        prevStrokeNodeBounds = prevStrokeNode.GetBounds();

                        RectCompareResult result =
                            FuzzyContains(  prevStrokeNodeBounds,
                                            prevPrevStrokeNodeBounds,
                                            isStartOfSegment ? 99.99999 : percentIntersect);

                        if (result == RectCompareResult::Rect1ContainsRect2)
                        {
                            // this node already contains the prevPrevStrokeNodeBounds (PP):
                            //
                            //  |------------|
                            //  | |----|     |
                            //  | | PP |  P  |
                            //  | |----|     |
                            //  |------------|
                            //
                            prevPrevStrokeNode = iterator.GetNode(index - 1, prevPrevStrokeNode.Index() - 1);
                            prevPrevStrokeNodeBounds.Union(prevStrokeNodeBounds);

                            // at this point prevPrevStrokeNodeBounds already contains this node
                            // we can just ignore this node
                            prevStrokeNode = emptyStrokeNode;

                            // update previousIndex to point to this node
                            previousIndex = index - 1;

                            // go back to our main loop
                            continue;
                        }
                        else if (result == RectCompareResult::Rect2ContainsRect1)
                        {
                            // this prevPrevStrokeNodeBounds (PP) already contains this node:
                            //
                            //  |------------|
                            //  |      |----||
                            //  |  PP  | P  ||
                            //  |      |----||
                            //  |------------|
                            //

                            //prevPrevStrokeNodeBounds already contains this node
                            //we can just ignore this node
                            prevStrokeNode = emptyStrokeNode;

                            // go back to our main loop, but do not update previousIndex
                            // because it should continue to point to previousPrevious
                            continue;
                        }

                        Debug::Assert(!prevStrokeNode.GetConnectingQuad().IsEmpty(), "prevStrokeNode.GetConnectingQuad() is Empty!");

                        // if neither was true, we now have two of our three nodes required to
                        // start our computation, we need to update previousIndex to point
                        // to our current, valid prevStrokeNode
                        previousIndex = index - 1;
                        continue; //so we always check if index < iterator.Count
                    }
                }

                //we know prevPrevStrokeNode and prevStrokeNode are both valid
                if (!strokeNode.IsValid())
                {
                    strokeNode = iterator.GetNode(index++, previousIndex);
                    strokeNodeBounds = strokeNode.GetBounds();

                    RectCompareResult result =
                            FuzzyContains(  strokeNodeBounds,
                                            prevStrokeNodeBounds,
                                            isStartOfSegment ? 99.99999 : percentIntersect);

                    RectCompareResult result2 =
                            FuzzyContains(  strokeNodeBounds,
                                            prevPrevStrokeNodeBounds,
                                            isStartOfSegment ? 99.99999 : percentIntersect);

                    if ( isStartOfSegment &&
                         result == RectCompareResult::Rect1ContainsRect2 &&
                         result2 == RectCompareResult::Rect1ContainsRect2)
                    {
                        if (pathFigureABSide.Count() > 0)
                        {
                            //we've started a stroke, we need to end it before resetting
                            //prevPrev
#if DEBUG_RENDERING_FEEDBACK
                            prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide, debugDC, feedbackSize, showFeedback);
#else
                            prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide);
#endif
                            //render
                            ReverseDCPointsRenderAndClear(context, pathFigureABSide, pathFigureDCSide, polyLinePoints, isEllipse, true/*clear the point collections*/);
                        }
                        //we're resetting
                        //prevPrevStrokeNode.  We need to gen one
                        //without a connecting quad
                        prevPrevStrokeNode = iterator.GetNode(index - 1, prevPrevStrokeNode.Index() - 1);
                        prevPrevStrokeNodeBounds = prevPrevStrokeNode.GetBounds();
                        prevStrokeNode = emptyStrokeNode;
                        strokeNode = emptyStrokeNode;

                        // increment previousIndex to to point to this node
                        previousIndex = index - 1;
                        continue;

                    }
                    else if (result == RectCompareResult::Rect1ContainsRect2)
                    {
                        // this node (C) already contains the prevStrokeNodeBounds (P):
                        //
                        //          |------------|
                        //  |----|  | |----|     |
                        //  | PP |  | | P  |  C  |
                        //  |----|  | |----|     |
                        //          |------------|
                        //
                        //we have to generate a new stroke node that points
                        //to pp since the connecting quad from C to P could be empty
                        //if they have the same point
                        strokeNode = iterator.GetNode(index - 1, prevStrokeNode.Index() - 1);
                        if (!strokeNode.GetConnectingQuad().IsEmpty())
                        {
                            //only update prevStrokeNode if we have a valid connecting quad
                            prevStrokeNode = strokeNode;
                            prevStrokeNodeBounds.Union(strokeNodeBounds);

                            // update previousIndex, since it should point to this node now
                            previousIndex = index - 1;
                        }

                        // at this point we can just ignore this node
                        strokeNode = emptyStrokeNode;
                        //strokeNodeBounds = empty;

                        prevAngle = -DBL_MAX; //invalidate

                        // go back to our main loop
                        continue;
                    }
                    else if (result == RectCompareResult::Rect2ContainsRect1)
                    {
                        // this prevStrokeNodeBounds (P) already contains this node (C):
                        //
                        //          |------------|
                        // |----|   |      |----||
                        // | PP |   |  P   | C  ||
                        // |----|   |      |----||
                        //          |------------|
                        //
                        //prevStrokeNodeBounds already contains this node
                        //we can just ignore this node
                        strokeNode = emptyStrokeNode;

                        // go back to our main loop, but do not update previousIndex
                        // because it should continue to point to previous
                        continue;
                    }

                    Debug::Assert(!strokeNode.GetConnectingQuad().IsEmpty(), "strokeNode.GetConnectingQuad was empty, this is unexpected");

                    //
                    // NOTE: we do not check if C contains PP, or PP contains C because
                    // that indicates a change in direction, which we handle below
                    //
                    // if neither was true P and C are separate,
                    // we now have all three nodes required to
                    // start our computation, we need to update previousIndex to point
                    // to our current, valid prevStrokeNode
                    previousIndex = index - 1;
                }


                // see if we have an overlap between the first and third node
                bool overlap = prevPrevStrokeNodeBounds.IntersectsWith(strokeNodeBounds);

                // prevPrevStrokeNode, prevStrokeNode and strokeNode are all
                // valid nodes now.  Now we need to figure out what do add to our
                // PathFigure.  First calc bounds on the strokeNode we know we need to render
                if (calculateBounds)
                {
                    bounds.Union(prevStrokeNodeBounds);
                }

                // determine what points to add to pathFigureABSide and pathFigureDCSide
                // from prevPrevStrokeNode
                if (pathFigureABSide.Count() == 0)
                {
                    Debug::Assert(pathFigureDCSide.Count() == 0);
                    if (calculateBounds)
                    {
                        bounds.Union(prevPrevStrokeNodeBounds);
                    }

                    if (isStartOfSegment && overlap)
                    {
                        //render a complete first stroke node or we can get artifacts
                        prevPrevStrokeNode.GetContourPoints(polyLinePoints);
                        AddFigureToStreamGeometryContext(context, polyLinePoints, prevPrevStrokeNode.IsEllipse()/*isBezierFigure*/);
                        polyLinePoints.Clear();
                    }

                    // we're starting a new pathfigure
                    // we need to add parts of the prevPrevStrokeNode contour
                    // to pathFigureABSide and pathFigureDCSide
#if DEBUG_RENDERING_FEEDBACK
                    prevStrokeNode.GetPointsAtStartOfSegment(pathFigureABSide, pathFigureDCSide, debugDC, feedbackSize, showFeedback);
#else
                    prevStrokeNode.GetPointsAtStartOfSegment(pathFigureABSide, pathFigureDCSide);
#endif

                    //set our marker, we're no longer at the start of the stroke
                    isStartOfSegment = false;
                }



                if (prevAngle == -DBL_MAX)
                {
                    //prevAngle is no longer valid
                    prevAngle = GetAngleBetween(prevPrevStrokeNode.Position(), prevStrokeNode.Position());
                }
                double delta = GetAngleDeltaFromLast(prevStrokeNode.Position(), strokeNode.Position(), prevAngle);
                bool directionChangedOverAbsoluteThreshold = Math::Abs(delta) > 90 && Math::Abs(delta) < (360 - 90);
                bool directionChangedOverOverlapThreshold = overlap && !(ignorePressure || strokeNode.PressureFactor() == 1) && Math::Abs(delta) > 30 && Math::Abs(delta) < (360 - 30);

                double prevArea = prevStrokeNodeBounds.Height() * prevStrokeNodeBounds.Width();
                double currArea = strokeNodeBounds.Height() * strokeNodeBounds.Width();

                bool areaChanged = !(prevArea == currArea && prevArea == (prevPrevStrokeNodeBounds.Height() * prevPrevStrokeNodeBounds.Width()));
                bool areaChangeOverThreshold = false;
                if (overlap && areaChanged)
                {
                    if ((Math::Min(prevArea, currArea) / Math::Max(prevArea, currArea)) <= 0.90)
                    {
                        //the min area is < 70% of the max area
                        areaChangeOverThreshold = true;
                    }
                }

                if (areaChanged || delta != 0.0 || index >= iteratorCount)
                {
                    //the area changed between the three nodes OR there was an angle delta OR we're at the end
                    //of the stroke...  either way, this is a significant node.  If not, we're going to drop it.
                    if ((overlap && (directionChangedOverOverlapThreshold || areaChangeOverThreshold)) ||
                        directionChangedOverAbsoluteThreshold)
                    {
                        //
                        // we need to stop the pathfigure at P
                        // and render the pathfigure
                        //
                        //  |--|      |--|    |--||--|   |------|
                        //  |PP|------|P |    |PP||P |   |PP P C|
                        //  |--|      |--|    |--||--|   |------|
                        //           /           |C |
                        //      |--|             |--|
                        //      |C |
                        //      |--|


#if DEBUG_RENDERING_FEEDBACK
                        prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide, debugDC, feedbackSize, showFeedback);
#else
                        //end the figure
                        prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide);
#endif
                        //render
                        ReverseDCPointsRenderAndClear(context, pathFigureABSide, pathFigureDCSide, polyLinePoints, isEllipse, true/*clear the point collections*/);

                        if (areaChangeOverThreshold)
                        {
                            //render a complete stroke node or we can get artifacts
                            prevStrokeNode.GetContourPoints(polyLinePoints);
                            AddFigureToStreamGeometryContext(context, polyLinePoints, prevStrokeNode.IsEllipse()/*isBezierFigure*/);
                            polyLinePoints.Clear();
                        }
                    }
                    else
                    {
                        //
                        // direction didn't change over the threshold, add the midpoint data
                        //  |--|      |--|
                        //  |PP|------|P |
                        //  |--|      |--|
                        //                \
                        //                  |--|
                        //                  |C |
                        //                  |--|
                        bool endSegment; //flag that tell us if we missed an intersection
#if DEBUG_RENDERING_FEEDBACK
                        strokeNode.GetPointsAtMiddleSegment(prevStrokeNode, delta, pathFigureABSide, pathFigureDCSide, endSegment, debugDC, feedbackSize, showFeedback);
#else
                        strokeNode.GetPointsAtMiddleSegment(prevStrokeNode, delta, pathFigureABSide, pathFigureDCSide, endSegment);
#endif
                        if (endSegment)
                        {
                            //we have a missing intersection, we need to end the
                            //segment at P
#if DEBUG_RENDERING_FEEDBACK
                            prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide, debugDC, feedbackSize, showFeedback);
#else
                            //end the figure
                            prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide);
#endif
                            //render
                            ReverseDCPointsRenderAndClear(context, pathFigureABSide, pathFigureDCSide, polyLinePoints, isEllipse, true/*clear the point collections*/);
                        }
                     }
                }

                //
                // either way... slide our pointers forward, to do this, we simply mark
                // our first pointer as 'empty'
                //
                prevPrevStrokeNode = emptyStrokeNode;
                prevPrevStrokeNodeBounds = empty;


            }

            //
            // anything left to render?
            //
            if (prevPrevStrokeNode.IsValid())
            {
                if (prevStrokeNode.IsValid())
                {
                    if (calculateBounds)
                    {
                        bounds.Union(prevPrevStrokeNodeBounds);
                        bounds.Union(prevStrokeNodeBounds);
                    }
                    Debug::Assert(!strokeNode.IsValid());
                    //
                    // we never made it to strokeNode, render two points, OR
                    // strokeNode was a dupe
                    //
                    if (pathFigureABSide.Count() > 0)
                    {
#if DEBUG_RENDERING_FEEDBACK
                        prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide, debugDC, feedbackSize, showFeedback);
#else
                        //
                        // strokeNode was a dupe, we just need to render the end of the stroke
                        // which is at prevStrokeNode
                        //
                        prevStrokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide);
#endif
                        //render
                        ReverseDCPointsRenderAndClear(context, pathFigureABSide, pathFigureDCSide, polyLinePoints, isEllipse, false/*clear the point collections*/);
                    }
                    else
                    {
                        // we've only seen two points to render
                        Debug::Assert(pathFigureDCSide.Count() == 0);
                        //contains all the logic to render two stroke nodes
                        RenderTwoStrokeNodes(   context,
                                                prevPrevStrokeNode,
                                                prevPrevStrokeNodeBounds,
                                                prevStrokeNode,
                                                prevStrokeNodeBounds,
                                                pathFigureABSide,
                                                pathFigureDCSide,
                                                polyLinePoints
#if DEBUG_RENDERING_FEEDBACK
                                               ,debugDC,
                                               feedbackSize,
                                               showFeedback
#endif
                                            );

                    }
                }
                else
                {
                    if (calculateBounds)
                    {
                        bounds.Union(prevPrevStrokeNodeBounds);
                    }

                    // we only have a single point to render
                    Debug::Assert(pathFigureABSide.Count() == 0);
                    prevPrevStrokeNode.GetContourPoints(pathFigureABSide);
                    AddFigureToStreamGeometryContext(context, pathFigureABSide, prevPrevStrokeNode.IsEllipse()/*isBezierFigure*/);

                }
            }
            else if (prevStrokeNode.IsValid() && strokeNode.IsValid())
            {
                if (calculateBounds)
                {
                    bounds.Union(prevStrokeNodeBounds);
                    bounds.Union(strokeNodeBounds);
                }

                // typical case, we hit the end of the stroke
                // see if we need to start a stroke, or just end one
                if (pathFigureABSide.Count() > 0)
                {
#if DEBUG_RENDERING_FEEDBACK
                    strokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide, debugDC, feedbackSize, showFeedback);
#else
                    strokeNode.GetPointsAtEndOfSegment(pathFigureABSide, pathFigureDCSide);
#endif

                    //render
                    ReverseDCPointsRenderAndClear(context, pathFigureABSide, pathFigureDCSide, polyLinePoints, isEllipse, false/*clear the point collections*/);

                    if (FuzzyContains(strokeNodeBounds, prevStrokeNodeBounds, 70) != RectCompareResult::NoItersection)
                    {
                        //render a complete stroke node or we can get artifacts
                        strokeNode.GetContourPoints(polyLinePoints);
                        AddFigureToStreamGeometryContext(context, polyLinePoints, strokeNode.IsEllipse()/*isBezierFigure*/);
                    }
                }
                else
                {
                    Debug::Assert(pathFigureDCSide.Count() == 0);
                    //contains all the logic to render two stroke nodes
                    RenderTwoStrokeNodes(   context,
                                            prevStrokeNode,
                                            prevStrokeNodeBounds,
                                            strokeNode,
                                            strokeNodeBounds,
                                            pathFigureABSide,
                                            pathFigureDCSide,
                                            polyLinePoints
#if DEBUG_RENDERING_FEEDBACK
                                           ,debugDC,
                                           feedbackSize,
                                           showFeedback
#endif
                                        );

                }
            }
        }
        //finally
        //{
        //    context.Close();
        //    geometry.Freeze();
        //}
    }
}


/// <summary>
/// Helper routine to render two distinct stroke nodes
/// </summary>
void StrokeRenderer::RenderTwoStrokeNodes(   StreamGeometryContext& context,
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
                                            )
{
    //Debug::Assert(pointBuffer1 != nullptr);
    //Debug::Assert(pointBuffer2 != nullptr);
    //Debug::Assert(pointBuffer3 != nullptr);
    //Debug::Assert(context != nullptr);


    //see if we need to render a quad - if there is not at least a 70% overlap
    if (FuzzyContains(strokeNodePreviousBounds, strokeNodeCurrentBounds, 70) != RectCompareResult::NoItersection)
    {
        //we're between 100% and 70% overlapped
        //just render two distinct figures with a connecting quad (if needed)
        strokeNodePrevious.GetContourPoints(pointBuffer1);
        AddFigureToStreamGeometryContext(context, pointBuffer1, strokeNodePrevious.IsEllipse()/*isBezierFigure*/);

        Quad quad = strokeNodeCurrent.GetConnectingQuad();
        if (!quad.IsEmpty())
        {
            pointBuffer3.Add(quad.A());
            pointBuffer3.Add(quad.B());
            pointBuffer3.Add(quad.C());
            pointBuffer3.Add(quad.D());
            AddFigureToStreamGeometryContext(context, pointBuffer3, false/*isBezierFigure*/);
        }

        strokeNodeCurrent.GetContourPoints(pointBuffer2);
        AddFigureToStreamGeometryContext(context, pointBuffer2, strokeNodeCurrent.IsEllipse()/*isBezierFigure*/);
    }
    else
    {
        //we're less than 70% overlapped, it's safe to run our optimization
#if DEBUG_RENDERING_FEEDBACK
        strokeNodeCurrent.GetPointsAtStartOfSegment(pointBuffer1, pointBuffer2, debugDC, feedbackSize, showFeedback);
        strokeNodeCurrent.GetPointsAtEndOfSegment(pointBuffer1, pointBuffer2, debugDC, feedbackSize, showFeedback);
#else
        strokeNodeCurrent.GetPointsAtStartOfSegment(pointBuffer1, pointBuffer2);
        strokeNodeCurrent.GetPointsAtEndOfSegment(pointBuffer1, pointBuffer2);
#endif
        //render
        ReverseDCPointsRenderAndClear(context, pointBuffer1, pointBuffer2, pointBuffer3, strokeNodeCurrent.IsEllipse(), false/*clear the point collections*/);
    }
}

/// <summary>
/// ReverseDCPointsRenderAndClear
/// </summary>
void StrokeRenderer::ReverseDCPointsRenderAndClear(StreamGeometryContext& context, List<Point>& abPoints, List<Point>& dcPoints, List<Point>& polyLinePoints, bool isEllipse, bool clear)
{
    //we need to reverse the cd side points
    Point temp;
    for (int i = 0, j = dcPoints.Count() - 1; i < j; i++, j--)
    {
        temp = dcPoints[i];
        dcPoints[i] = dcPoints[j];
        dcPoints[j] = temp;
    }
    if (isEllipse)
    {
        AddArcToFigureToStreamGeometryContext(context, abPoints, dcPoints, polyLinePoints);
    }
    else
    {
        //for rectangles, render a single path figure by combining both sides
        AddPolylineFigureToStreamGeometryContext(context, abPoints, dcPoints);
    }

    if (clear)
    {
        abPoints.Clear();
        dcPoints.Clear();
    }
}
/// <summary>
/// FuzzyContains for two rects
/// </summary>
StrokeRenderer::RectCompareResult StrokeRenderer::FuzzyContains(Rect const & rect1, Rect const & rect2, double percentIntersect)
{
    Debug::Assert(percentIntersect >= 0.0 && percentIntersect <= 100.0);


    double intersectLeft = Math::Max(rect1.Left(), rect2.Left());
    double intersectTop = Math::Max(rect1.Top(), rect2.Top());
    double intersectWidth = Math::Max((Math::Min(rect1.Right(), rect2.Right()) - intersectLeft), 0.0);
    double intersectHeight = Math::Max((Math::Min(rect1.Bottom(), rect2.Bottom()) - intersectTop), 0.0);

    if (intersectWidth == 0.0 || intersectHeight == 0.0)
    {
        return RectCompareResult::NoItersection;
    }

    //we have an intersection, see if it is enough
    double rect1Area = rect1.Height() * rect1.Width();
    double rect2Area = rect2.Height() * rect2.Width();
    double minArea = Math::Min(rect1Area, rect2Area);
    double intersectionArea = intersectWidth * intersectHeight;
    double intersect = (intersectionArea / minArea) * 100;
    if (intersect >= percentIntersect)
    {
        if (rect1Area >= rect2Area)
        {
            return RectCompareResult::Rect1ContainsRect2;
        }
        return RectCompareResult::Rect2ContainsRect1;
    }

    return RectCompareResult::NoItersection;
}

/// <summary>
/// Private helper to render a path figure to the SGC
/// </summary>
void StrokeRenderer::AddFigureToStreamGeometryContext(StreamGeometryContext& context, List<Point> & points, bool isBezierFigure)
{
    //Debug::Assert(context != nullptr);
    //Debug::Assert(points != nullptr);
    Debug::Assert(points.Count() > 0);

    context.BeginFigure(points[points.Count() - 1], //start point
                                true,   //isFilled
                                true);  //IsClosed

    if (isBezierFigure)
    {
        context.PolyBezierTo(points,
                             true,      //isStroked
                             true);     //isSmoothJoin
    }
    else
    {
        context.PolyLineTo(points,
                             true,      //isStroked
                             true);     //isSmoothJoin
    }
}


/// <summary>
/// Private helper to render a path figure to the SGC
/// </summary>
void StrokeRenderer::AddPolylineFigureToStreamGeometryContext(StreamGeometryContext& context, List<Point>& abPoints, List<Point>& dcPoints)
{
    //Debug::Assert(context != nullptr);
    //Debug::Assert(abPoints != nullptr && dcPoints != nullptr);
    Debug::Assert(abPoints.Count() > 0 && dcPoints.Count() > 0);

    context.BeginFigure(abPoints[0], //start point
                                true,   //isFilled
                                true);  //IsClosed

    context.PolyLineTo(abPoints,
                         true,      //isStroked
                         true);     //isSmoothJoin

    context.PolyLineTo(dcPoints,
                         true,      //isStroked
                         true);     //isSmoothJoin

}

/// <summary>
/// Private helper to render a path figure to the SGC
/// </summary>
void StrokeRenderer::AddArcToFigureToStreamGeometryContext(StreamGeometryContext& context, List<Point>& abPoints, List<Point>& dcPoints, List<Point>& polyLinePoints)
{
    //Debug::Assert(context != nullptr);
    //Debug::Assert(abPoints != nullptr && dcPoints != nullptr);
    //Debug::Assert(polyLinePoints != nullptr);
    Debug::Assert(abPoints.Count() > 0 && dcPoints.Count() > 0);
    if (abPoints.Count() == 0 || dcPoints.Count() == 0)
    {
        return;
    }

    context.BeginFigure(abPoints[0], //start point
                                true,   //isFilled
                                true);  //IsClosed

    for (int j = 0; j < 2; j++)
    {
        List<Point> points = j == 0 ? abPoints : dcPoints;
        int startIndex = j == 0 ? 1 : 0;
        for (int i = startIndex; i < points.Count(); )
        {
            Point next = points[i];
            if (next == StrokeRenderer::ArcToMarker)
            {
                if (polyLinePoints.Count() > 0)
                {
                    //polyline first
                    context.PolyLineTo(  polyLinePoints,
                                         true,      //isStroked
                                         true);     //isSmoothJoin
                    polyLinePoints.Clear();
                }
                //we're arcing, pull out height, width and the arc to point
                Debug::Assert(i + 2 < points.Count());
                if (i + 2 < points.Count())
                {
                    Point sizePoint = points[i + 1];
                    Size ellipseSize(sizePoint.X() / 2/*width*/, sizePoint.Y() / 2/*height*/);
                    Point arcToPoint = points[i + 2];

                    bool isLargeArc = false; //>= 180

                    context.ArcTo(  arcToPoint,
                                    ellipseSize,
                                    0,             //rotation
                                    isLargeArc,     //isLargeArc
                                    SweepDirection::Clockwise,
                                    true,           //isStroked
                                    true);          //isSmoothJoin
                }
                i += 3; //advance past this arcTo block
            }
            else
            {
                //walk forward until we find an arc marker or the end
                polyLinePoints.Add(next);
                i++;
            }
        }
        if (polyLinePoints.Count() > 0)
        {
            //polyline
            context.PolyLineTo(polyLinePoints,
                                 true,      //isStroked
                                 true);     //isSmoothJoin
            polyLinePoints.Clear();
        }
    }
}

/// <summary>
/// calculates the angle between the previousPosition and the current one and then computes the delta between
/// the lastAngle.  lastAngle is also updated
/// </summary>
double StrokeRenderer::GetAngleDeltaFromLast(Point const & previousPosition, Point const & currentPosition, double& lastAngle)
{
    double delta = 0.0;

    //input points typically come in very close to each other
    double dx = (currentPosition.X() * 1000) - (previousPosition.X() * 1000);
    double dy = (currentPosition.Y() * 1000) - (previousPosition.Y() * 1000);
    if (static_cast<int64_t>(dx) == 0 && static_cast<int64_t>(dy) == 0)
    {
        //the points are close enough not to matter
        //don't update lastAngle
        return delta;
    }

    double angle = GetAngleBetween(previousPosition, currentPosition);

    //special case when angle / lastAngle span 0 degrees
    if (lastAngle >= 270 && angle <= 90)
    {
        delta = lastAngle - (360 + angle);
    }
    else if (lastAngle <= 90 && angle >= 270)
    {
        delta = (360 + lastAngle) - angle;
    }
    else
    {
        delta = (lastAngle - angle);
    }
    lastAngle = angle;

    // Return
    return delta;
}

/// <summary>
/// calculates the angle between the previousPosition and the current one and then computes the delta between
/// the lastAngle.  lastAngle is also updated
/// </summary>
double StrokeRenderer::GetAngleBetween(Point const & previousPosition, Point const & currentPosition)
{
    double angle = 0.0;

    //input points typically come in very close to each other
    double dx = (currentPosition.X() * 1000) - (previousPosition.X() * 1000);
    double dy = (currentPosition.Y() * 1000) - (previousPosition.Y() * 1000);
    if (static_cast<int64_t>(dx) == 0 && static_cast<int64_t>(dy) == 0)
    {
        //the points are close enough not to matter
        return angle;
    }

    // Calculate angle
    if (dx == 0.0)
    {
        if (dy == 0.0)
        {
            angle = 0.0;
        }
        else if (dy > 0.0)
        {
            angle = Math::PI / 2.0;
        }
        else
        {
            angle = Math::PI * 3.0 / 2.0;
        }
    }
    else if (dy == 0.0)
    {
        if (dx > 0.0)
        {
            angle = 0.0;
        }
        else
        {
            angle = Math::PI;
        }
    }
    else
    {
        if (dx < 0.0)
        {
            angle = atan(dy / dx) + Math::PI;
        }
        else if (dy < 0.0)
        {
            angle = atan(dy / dx) + (2 * Math::PI);
        }
        else
        {
            angle = atan(dy / dx);
        }
    }

    // Convert to degrees
    angle = angle * 180 / Math::PI;

    // Return
    return angle;
}

#ifdef INKCANVAS_QT
/// <summary>
/// Get the DrawingAttributes to use for a highlighter stroke. The return value is a copy of
/// the DA passed in if color.A != 255 with color.A overriden to be 255. Otherwise it returns
/// the DA passed in.
/// </summary>
SharedPointer<DrawingAttributes> StrokeRenderer::GetHighlighterAttributes(Stroke& stroke, SharedPointer<DrawingAttributes> da)
{
    //System.Diagnostics.Debug::Assert(da.IsHighlighter = true);
    if (da->Color().alpha() != SolidStrokeAlpha)
    {
        SharedPointer<DrawingAttributes> copy(stroke.GetDrawingAttributes()->Clone());
        copy->SetColor(GetHighlighterColor(copy->Color()));
        return copy;
    }

    return da;
}
#endif

INKCANVAS_END_NAMESPACE
