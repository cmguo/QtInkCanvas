#include "strokenodeiterator.h"
#include "strokenode.h"
#include "styluspoint.h"

/// <summary>
/// Helper wrapper
/// </summary>
StrokeNodeIterator StrokeNodeIterator::GetIterator(Stroke & stroke, DrawingAttributes& drawingAttributes)
{
    //if (stroke == null)
    //{
    //    throw new System.ArgumentNullException("stroke");
    //}
    //if (drawingAttributes == null)
    //{
    //    throw new System.ArgumentNullException("drawingAttributes");
    //}

    QSharedPointer<StylusPointCollection> stylusPoints =
        drawingAttributes.FitToCurve() ? stroke.GetBezierStylusPoints() : stroke.StylusPoints();

    return GetIterator(stylusPoints, drawingAttributes);
}
/// <summary>
/// Creates a default enumerator for a given stroke
/// If using the strokes drawing attributes, pass stroke.DrawingAttributes for the second
/// argument.  If using an overridden DA, use that instance.
/// </summary>
StrokeNodeIterator StrokeNodeIterator::GetIterator(QSharedPointer<StylusPointCollection> stylusPoints, DrawingAttributes& drawingAttributes)
{
    //if (stylusPoints == null)
    //{
    //    throw new System.ArgumentNullException("stylusPoints");
    //}
    //if (drawingAttributes == null)
    //{
    //    throw new System.ArgumentNullException("drawingAttributes");
    //}

    StrokeNodeOperations * operations =
        StrokeNodeOperations::CreateInstance(*drawingAttributes.GetStylusShape());

    bool usePressure = !drawingAttributes.IgnorePressure();

    return StrokeNodeIterator(stylusPoints, operations, usePressure);
}



/// <summary>
/// Constructor for an incremental node enumerator that builds nodes
/// from array(s) of points and a given stylus shape.
/// </summary>
/// <param name="nodeShape">a shape that defines the stroke contour</param>
StrokeNodeIterator::StrokeNodeIterator(StylusShape & nodeShape)
    : StrokeNodeIterator( nullptr,   //stylusPoints
            StrokeNodeOperations::CreateInstance(nodeShape),
            false)  //usePressure)
{
}

/// <summary>
/// Constructor for an incremental node enumerator that builds nodes
/// from StylusPointCollections
/// called by the IncrementalRenderer
/// </summary>
/// <param name="drawingAttributes">drawing attributes</param>
StrokeNodeIterator::StrokeNodeIterator(DrawingAttributes& drawingAttributes)
    : StrokeNodeIterator( nullptr,   //stylusPoints
            StrokeNodeOperations::CreateInstance(*drawingAttributes.GetStylusShape()),
            !drawingAttributes.IgnorePressure())  //usePressure
{
}

/// <summary>
/// Private ctor
/// </summary>
/// <param name="stylusPoints"></param>
/// <param name="operations"></param>
/// <param name="usePressure"></param>
StrokeNodeIterator::StrokeNodeIterator(QSharedPointer<StylusPointCollection> stylusPoints,
                            StrokeNodeOperations* operations,
                            bool usePressure)
    : _stylusPoints(stylusPoints)
    , _operations(operations)
    , _usePressure(usePressure)
{
    //Note, StylusPointCollection can be null
    //_stylusPoints = stylusPoints;
    //if (operations == null)
    //{
    //    throw new ArgumentNullException("operations");
    //}
    //_operations = operations;
    //_usePressure = usePressure;
}

/// <summary>
/// Generates (enumerates) StrokeNode objects for a stroke increment
/// represented by an StylusPointCollection.  Called from IncrementalRenderer
/// </summary>
/// <param name="stylusPoints">StylusPointCollection</param>
/// <returns>yields StrokeNode objects one by one</returns>
StrokeNodeIterator StrokeNodeIterator::GetIteratorForNextSegment(QSharedPointer<StylusPointCollection> stylusPoints)
{
    //if (stylusPoints == null)
    //{
    //    throw new System.ArgumentNullException("stylusPoints");
    //}

    if (/*_stylusPoints != null && */_stylusPoints->size() > 0 && stylusPoints->size() > 0)
    {
        //insert the previous last point, but we need insert a compatible
        //previous point.  The easiest way to do this is to clone a point
        //(since StylusPoint is a struct, we get get one out to get a copy
        StylusPoint sp = (*stylusPoints)[0];
        StylusPoint lastStylusPoint = (*_stylusPoints)[_stylusPoints->size() - 1];
        sp.SetX(lastStylusPoint.X());
        sp.SetY(lastStylusPoint.Y());
        sp.SetPressureFactor(lastStylusPoint.PressureFactor());
        stylusPoints->insert(0, sp);
    }

    return StrokeNodeIterator(  stylusPoints,
                                    _operations,
                                    _usePressure);
}

/// <summary>
/// Generates (enumerates) StrokeNode objects for a stroke increment
/// represented by an array of points. This method is supposed to be used only
/// on objects created via the c-tor with a StylusShape parameter.
/// </summary>
/// <param name="points">an array of points representing a stroke increment</param>
/// <returns>yields StrokeNode objects one by one</returns>
StrokeNodeIterator StrokeNodeIterator::GetIteratorForNextSegment(QVector<QPointF> const & points)
{
    //if (points == null)
    //{
    //    throw new System.ArgumentNullException("points");
    //}
    QSharedPointer<StylusPointCollection> newStylusPoints(new StylusPointCollection(points));
    if (/*_stylusPoints != null && */_stylusPoints->size() > 0)
    {
        //insert the previous last point
        newStylusPoints->insert(0, (*_stylusPoints)[_stylusPoints->size() - 1]);
    }

    return StrokeNodeIterator(  newStylusPoints,
                                    _operations,
                                    _usePressure);
}

/// <summary>
/// Gets a StrokeNode at the specified index
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
StrokeNode StrokeNodeIterator::operator[](int index)
{
    return GetNode(index, (index == 0 ? -1 : index - 1));
}

/// <summary>
/// Gets a StrokeNode at the specified index that connects to a stroke at the previousIndex
/// previousIndex can be -1 to signify it should be empty (first strokeNode)
/// </summary>
/// <returns></returns>
StrokeNode StrokeNodeIterator::GetNode(int index, int previousIndex)
{
    if (/*_stylusPoints == null||  */index < 0 || index >= _stylusPoints->size() || previousIndex < -1 || previousIndex >= index)
    {
        throw new std::exception();
    }

    StylusPoint stylusPoint = (*_stylusPoints)[index];
    StylusPoint previousStylusPoint = (previousIndex == -1 ? StylusPoint() : (*_stylusPoints)[previousIndex]);
    float pressureFactor = 1.0f;
    float previousPressureFactor = 1.0f;
    if (_usePressure)
    {
        pressureFactor = StrokeNodeIterator::GetNormalizedPressureFactor(stylusPoint.PressureFactor());
        previousPressureFactor = StrokeNodeIterator::GetNormalizedPressureFactor(previousStylusPoint.PressureFactor());
    }

    StrokeNodeData nodeData(stylusPoint, pressureFactor);
    StrokeNodeData lastNodeData = StrokeNodeData::Empty();
    if (previousIndex != -1)
    {
        lastNodeData = StrokeNodeData(previousStylusPoint, previousPressureFactor);
    }

    //we use previousIndex+1 because index can skip ahead
    return StrokeNode(_operations, previousIndex + 1, nodeData, lastNodeData, index == _stylusPoints->size() - 1 /*Is this the last node?*/);
}
