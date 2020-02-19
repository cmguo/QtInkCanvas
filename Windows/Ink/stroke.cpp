#include "Windows/Ink/stroke.h"
#include "Internal/Ink/bezier.h"
#include "strokecollection.h"
#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"
#include "Windows/Ink/stylusshape.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Input/styluspoint.h"
#include "Internal/Ink/strokefindices.h"
#include "Internal/doubleutil.h"
#include "Internal/Ink/lasso.h"
#include "Internal/Ink/strokenodeiterator.h"
#include "incrementalhittester.h"
#include "Windows/Media/drawingcontext.h"
#include "Internal/Ink/strokerenderer.h"
#include "Windows/Ink/events.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QMatrix>
#include <QBrush>
#include <QDebug>
#include <QThread>

Stroke::Stroke()
{

}

Stroke::~Stroke()
{
    SetGeometry(nullptr);
}

/// <summary>Create a stroke from a StylusPointCollection</summary>
/// <remarks>
/// </remarks>
/// <param name="stylusPoints">StylusPointCollection that makes up the stroke</param>
Stroke::Stroke(QSharedPointer<StylusPointCollection> stylusPoints)
    : Stroke (stylusPoints, nullptr, nullptr)
{
}

/// <summary>Create a stroke from a StylusPointCollection</summary>
/// <remarks>
/// </remarks>
/// <param name="stylusPoints">StylusPointCollection that makes up the stroke</param>
/// <param name="drawingAttributes">drawingAttributes</param>
Stroke::Stroke(QSharedPointer<StylusPointCollection> stylusPoints, QSharedPointer<DrawingAttributes> drawingAttributes)
    : Stroke(stylusPoints, drawingAttributes, nullptr)
{
}

/// <summary>Create a stroke from a StylusPointCollection</summary>
/// <remarks>
/// </remarks>
/// <param name="stylusPoints">StylusPointCollection that makes up the stroke</param>
/// <param name="drawingAttributes">drawingAttributes</param>
/// <param name="extendedProperties">extendedProperties</param>
Stroke::Stroke(QSharedPointer<StylusPointCollection> stylusPoints, QSharedPointer<DrawingAttributes> drawingAttributes, ExtendedPropertyCollection* extendedProperties)
{
    if (stylusPoints->size() == 0)
    {
        throw std::runtime_error("stylusPoints");
    }

    _drawingAttributes = drawingAttributes;
    _stylusPoints = stylusPoints;
    _extendedProperties = extendedProperties;

    Initialize();
}

/// <summary>
/// helper to set up listeners, called by ctor and by Clone
/// </summary>
void Stroke::Initialize()
{
    //_drawingAttributes.AttributeChanged += new PropertyDataChangedEventHandler(DrawingAttributes_Changed);
    //_stylusPoints.Changed += new EventHandler(StylusPoints_Changed);
    //_stylusPoints.CountGoingToZero += new CancelEventHandler(StylusPoints_CountGoingToZero);
    QObject::connect(_drawingAttributes.get(), &DrawingAttributes::AttributeChanged,
                     this, &Stroke::DrawingAttributes_Changed);
    QObject::connect(_stylusPoints.get(), &StylusPointCollection::Changed,
                     this, &Stroke::StylusPoints_Changed);
    QObject::connect(_stylusPoints.get(), &StylusPointCollection::CountGoingToZero,
                     this, &Stroke::StylusPoints_CountGoingToZero);
}

/// <summary>Returns a new stroke that has a deep copy.</summary>
/// <remarks>Deep copied data includes points, point description, drawing attributes, and transform</remarks>
/// <returns>Deep copy of current stroke</returns>
Stroke::Stroke(Stroke const & o)
    : _drawingAttributes(o._drawingAttributes)
    , _stylusPoints(o._stylusPoints)
    , _cloneStylusPoints(o._cloneStylusPoints)
{


    //Clone is also called from Stroke.Copy internally for point
    //erase.  In that case, we don't want to clone the StylusPoints
    //because they will be replaced after we call
    if (o._cloneStylusPoints)
    {
        _stylusPoints = o._stylusPoints->Clone();
    }
    _drawingAttributes = o._drawingAttributes->Clone();
    if (o._extendedProperties != nullptr)
    {
        _extendedProperties = o._extendedProperties->Clone();
    }
    //set up listeners
    Initialize();

    //
    // copy state
    //
    //Debug::Assert(_cachedGeometry == nullptr/* || _cachedGeometry.IsFrozen*/);
    //we don't need to cache if this is frozen
    //if (null != _cachedGeometry)
    //{
    //    clone._cachedGeometry = _cachedGeometry.Clone();
    //}
    //don't need to clone these, they are value types
    //and are copied by MemberwiseClone
    //_isSelected
    //_drawAsHollow
    //_cachedBounds

    //this need to be reset
    _cloneStylusPoints = true;
}


/// <summary>Transforms the ink and also changes the StylusTip</summary>
/// <param name="transformMatrix">Matrix to transform the stroke by</param>
/// <param name="applyToStylusTip">Boolean if true the transform matrix will be applied to StylusTip</param>
void Stroke::Transform(QMatrix transformMatrix, bool applyToStylusTip)
{
    if (transformMatrix.isIdentity())
    {
        return;
    }

    if (!transformMatrix.isInvertible())
    {
        throw std::runtime_error("transformMatrix");
    }
    else if ( MatrixHelper::ContainsNaN(transformMatrix))
    {
        throw std::runtime_error("transformMatrix");
    }
    else if ( MatrixHelper::ContainsInfinity(transformMatrix))
    {
        throw std::runtime_error("transformMatrix");
    }
    else
    {
        // we need to force a recaculation of the cached path geometry right after the
        // DrawingAttributes changed, beforet the events are raised.
        std::unique_ptr<Geometry> geometry;
        SetGeometry(geometry);
        // Set the cached bounds to empty, which will force a re-calculation of the _cachedBounds upon next GetBounds call.
        _cachedBounds = QRectF();

        if (applyToStylusTip)
        {
            //we use this flag to prevent this method from causing two
            //invalidates, which causes a good deal of memory thrash when
            //the strokes are being rendered
            _delayRaiseInvalidated = true;
        }

        //try
        {
            FinallyHelper final([this]() {
                _delayRaiseInvalidated = false;
            });
            _stylusPoints->Transform(transformMatrix);

            if (applyToStylusTip)
            {
                QMatrix newMatrix = _drawingAttributes->StylusTipTransform();
                // Don't allow a Translation in the matrix
                //transformMatrix.OffsetX = 0;
                //transformMatrix.OffsetY = 0;
                transformMatrix.translate(transformMatrix.dx() / transformMatrix.m11(), transformMatrix.dx() / transformMatrix.m22());
                newMatrix *= transformMatrix;
                //only persist the StylusTipTransform if there is an inverse.
                //there are cases where two invertible xf's result in a non-invertible one
                //we decided not to throw here because it is so unobvious
                if (newMatrix.isInvertible())
                {
                    _drawingAttributes->SetStylusTipTransform(newMatrix);
                }
            }
            if (_delayRaiseInvalidated)
            {
                OnInvalidated(EventArgs::Empty);
            }
            //else OnInvalidated was already raised
        }
        //catch (...)
        //{
            //We do this in a finally block to reset
            //our state in the event that an exception is thrown.
        //    _delayRaiseInvalidated = false;
        //}
    }
}

/// <summary>
/// Returns a Bezier smoothed version of the StylusPoints
/// </summary>
/// <returns></returns>
QSharedPointer<StylusPointCollection> Stroke::GetBezierStylusPoints()
{
    // Since we can't compute Bezier for single point stroke, we should return.
    if (_stylusPoints->size() < 2)
    {
        return _stylusPoints;
    }

    // Construct the Bezier approximation
    Bezier bezier;
    if (!bezier.ConstructBezierState(   *_stylusPoints,
                                        _drawingAttributes->FittingError()))
    {
        //construction failed, return a clone of the original points
        return _stylusPoints;
    }

    double tolerance = 0.5;
    StylusShape * stylusShape = _drawingAttributes->GetStylusShape();
    if (nullptr != stylusShape)
    {
        QRectF shapeBoundingBox = stylusShape->BoundingBox();
        double min = qMin(shapeBoundingBox.width(), shapeBoundingBox.height());
        tolerance = log10(min + min);
        tolerance *= (StrokeCollectionSerializer::AvalonToHimetricMultiplier / 2);
        if (tolerance < 0.5)
        {
            //don't allow tolerance to drop below .5 or we
            //can wind up with an huge amount of bezier points
            tolerance = 0.5;
        }
    }

    QList<QPointF> bezierPoints = bezier.Flatten(tolerance);
    return GetInterpolatedStylusPoints(bezierPoints);
}

/// <summary>
/// Interpolate packet / pressure data from _stylusPoints
/// </summary>
QSharedPointer<StylusPointCollection> Stroke::GetInterpolatedStylusPoints(QList<QPointF> & bezierPoints)
{
    Debug::Assert(/*bezierPoints != null && */bezierPoints.size() > 0);

    //new points need the same description
    QSharedPointer<StylusPointCollection> bezierStylusPoints(new StylusPointCollection(_stylusPoints->Description(), bezierPoints.size()));

    //
    // add the first point
    //
    AddInterpolatedBezierPoint( *bezierStylusPoints,
                                bezierPoints[0],
                                (*_stylusPoints)[0].GetAdditionalData(),
                                (*_stylusPoints)[0].PressureFactor());

    if (bezierPoints.size() == 1)
    {
        return bezierStylusPoints;
    }

    //
    // this is a little tricky... Bezier points are not equidistant, so we have to
    // use the length between the points instead of the indexes to interpolate pressure
    //
    //  Bezier points:   P0 ------------------------------ P1 ---------- P2 --------- P3
    //  Stylus points:   P0 -------- P1 ------------ P2 ------------- P3 ---------- P4
    //
    //  Or in terms of lengths...
    //  Bezier lengths:  L1 ------------------------------
    //                   L2 ---------------------------------------------
    //                   L3 ---------------------------------------------------------
    //
    //  Stylus lengths   L1 --------
    //                   L2 ------------------------
    //                   L3 -----------------------------------------
    //                   L4 --------------------------------------------------------
    //
    //
    //
    double bezierLength = 0.0;
    double prevUnbezierLength = 0.0;
    double unbezierLength = GetDistanceBetweenPoints((*_stylusPoints)[0], (*_stylusPoints)[1]);

    int stylusPointsIndex = 1;
    int stylusPointsCount = _stylusPoints->size();
    //skip the first and last point
    for (int x = 1; x < bezierPoints.size() - 1; x++)
    {
        bezierLength += GetDistanceBetweenPoints(bezierPoints[x - 1], bezierPoints[x]);
        while (stylusPointsCount > stylusPointsIndex)
        {
            if (bezierLength >= prevUnbezierLength &&
                bezierLength < unbezierLength)
            {
                Debug::Assert(stylusPointsCount > stylusPointsIndex);

                StylusPoint prevStylusPoint = (*_stylusPoints)[stylusPointsIndex - 1];
                float percentFromPrev =
                    ((float)bezierLength - (float)prevUnbezierLength) /
                    ((float)unbezierLength - (float)prevUnbezierLength);
                float pressureAtPrev = prevStylusPoint.PressureFactor();
                float pressureDelta = (*_stylusPoints)[stylusPointsIndex].PressureFactor() - pressureAtPrev;
                float interopolatedPressure = (percentFromPrev * pressureDelta) + pressureAtPrev;

                AddInterpolatedBezierPoint(*bezierStylusPoints,
                                            bezierPoints[x],
                                            prevStylusPoint.GetAdditionalData(),
                                            interopolatedPressure);
                break;

            }
            else
            {
                Debug::Assert(bezierLength >= prevUnbezierLength);
                //
                // move our unbezier lengths forward...
                //
                stylusPointsIndex++;
                if (stylusPointsCount > stylusPointsIndex)
                {
                    prevUnbezierLength = unbezierLength;
                    unbezierLength +=
                        GetDistanceBetweenPoints((*_stylusPoints)[stylusPointsIndex - 1],
                                                 (*_stylusPoints)[stylusPointsIndex]);
                } //else we'll break
            }
        }
    }

    //
    // add the last point
    //
    AddInterpolatedBezierPoint( *bezierStylusPoints,
                                bezierPoints[bezierPoints.size() - 1],
                                (*_stylusPoints)[stylusPointsCount - 1].GetAdditionalData(),
                                (*_stylusPoints)[stylusPointsCount - 1].PressureFactor());

    return bezierStylusPoints;
}

/// <summary>
/// helper used to get the length between two points
/// </summary>
double Stroke::GetDistanceBetweenPoints(QPointF const & p1, QPointF const & p2)
{
    QPointF spine = p2 - p1;
    return sqrt(QPointF::dotProduct(spine, spine));
}

/// <summary>
/// helper for adding a StylusPoint to the BezierStylusPoints
/// </summary>
void Stroke::AddInterpolatedBezierPoint(StylusPointCollection & bezierStylusPoints,
                                        QPointF & bezierPoint,
                                        QVector<int> const & additionalData,
                                        float pressure)
{
    double xVal = bezierPoint.x() > StylusPoint::MaxXY ?
                StylusPoint::MaxXY :
                (bezierPoint.x() < StylusPoint::MinXY ? StylusPoint::MinXY : bezierPoint.x());

    double yVal = bezierPoint.y() > StylusPoint::MaxXY ?
                StylusPoint::MaxXY :
                (bezierPoint.y() < StylusPoint::MinXY ? StylusPoint::MinXY : bezierPoint.y());


    StylusPoint newBezierPoint(xVal, yVal, pressure, bezierStylusPoints.Description(), additionalData, false, false);


    bezierStylusPoints.AddItem(newBezierPoint);
}

/// <summary>
/// Allows addition of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
/// <param name="propertyData"></param>
void Stroke::AddPropertyData(QUuid const & propertyDataId, QVariant propertyData)
{
    DrawingAttributes::ValidateStylusTipTransform(propertyDataId, propertyData);

    QVariant oldValue;
    if (ContainsPropertyData(propertyDataId))
    {
        oldValue = GetPropertyData(propertyDataId);
        ExtendedProperties()[propertyDataId] = propertyData;
    }
    else
    {
        ExtendedProperties().Add(propertyDataId, propertyData);
    }

    // fire notification
    PropertyDataChangedEventArgs e(propertyDataId, propertyData, oldValue);
    OnPropertyDataChanged(e);
}


/// <summary>
/// Allows removal of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
void Stroke::RemovePropertyData(QUuid const & propertyDataId)
{
    QVariant propertyData = GetPropertyData(propertyDataId);
    ExtendedProperties().Remove(propertyDataId);
    // fire notification
    PropertyDataChangedEventArgs e(propertyDataId, QVariant(), propertyData);
    OnPropertyDataChanged(e);
}

/// <summary>
/// Allows retrieval of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
QVariant Stroke::GetPropertyData(QUuid const & propertyDataId)
{
    return ExtendedProperties()[propertyDataId];
}

/// <summary>
/// Allows retrieval of a Array of guids that are contained in the EPC
/// </summary>
QVector<QUuid> Stroke::GetPropertyDataIds()
{
    return ExtendedProperties().GetGuidArray();
}

/// <summary>
/// Allows the checking of objects in the EPC
/// </summary>
/// <param name="propertyDataId"></param>
bool Stroke::ContainsPropertyData(QUuid const & propertyDataId)
{
    return ExtendedProperties().Contains(propertyDataId);
}


/// <summary>
/// Allows an application to configure the rendering state
/// associated with this stroke (e.g. outline pen, brush, color,
/// stylus tip, etc.)
/// </summary>
/// <remarks>
/// If the stroke has been deleted, this will return null for 'get'.
/// If the stroke has been deleted, the 'set' will no-op.
/// </remarks>
/// <value>The drawing attributes associated with the current stroke.</value>
void Stroke::SetDrawingAttributes(QSharedPointer<DrawingAttributes> value)
{
    //_drawingAttributes.AttributeChanged -= new PropertyDataChangedEventHandler(DrawingAttributes_Changed);
    QObject::disconnect(_drawingAttributes.get(), &DrawingAttributes::AttributeChanged,
                     this, &Stroke::DrawingAttributes_Changed);

    DrawingAttributesReplacedEventArgs e(value, _drawingAttributes);

    QSharedPointer<DrawingAttributes> previousDa = _drawingAttributes;
    _drawingAttributes = value;


    // If the drawing attributes change involves Width, Height, StylusTipTransform, IgnorePressure, or FitToCurve,
    // we need to force a recaculation of the cached path geometry right after the
    // DrawingAttributes changed, beforet the events are raised.
    std::unique_ptr<Geometry> geometry;
    if (false == DrawingAttributes::GeometricallyEqual(*previousDa, *_drawingAttributes))
    {
        SetGeometry(geometry);
        // Set the cached bounds to empty, which will force a re-calculation of the _cachedBounds upon next GetBounds call.
        _cachedBounds = QRectF();
    }

    //_drawingAttributes.AttributeChanged += new PropertyDataChangedEventHandler(DrawingAttributes_Changed);
    QObject::connect(_drawingAttributes.get(), &DrawingAttributes::AttributeChanged,
                     this, &Stroke::DrawingAttributes_Changed);
    OnDrawingAttributesReplaced(e);
    OnInvalidated(EventArgs::Empty);
    OnPropertyChanged(DrawingAttributesName);
}


/// <summary>
/// StylusPoints
/// </summary>
void Stroke::SetStylusPoints(QSharedPointer<StylusPointCollection> value)
{
    if (value->size() == 0)
    {
        //we don't allow this
        throw new std::runtime_error("StylusPoints");
    }

    // Force a recaculation of the cached path geometry
    std::unique_ptr<Geometry> geometry;
    SetGeometry(geometry);

    // Set the cached bounds to empty, which will force a re-calculation of the _cachedBounds upon next GetBounds call.
    _cachedBounds = QRectF();

    StylusPointsReplacedEventArgs e(value, _stylusPoints);

    //_stylusPoints.Changed -= new EventHandler(StylusPoints_Changed);
    //_stylusPoints.CountGoingToZero -= new CancelEventHandler(StylusPoints_CountGoingToZero);
    QObject::disconnect(_stylusPoints.get(), &StylusPointCollection::Changed,
                     this, &Stroke::StylusPoints_Changed);
    QObject::disconnect(_stylusPoints.get(), &StylusPointCollection::CountGoingToZero,
                     this, &Stroke::StylusPoints_CountGoingToZero);

    _stylusPoints = value;

    //_stylusPoints.Changed += new EventHandler(StylusPoints_Changed);
    //_stylusPoints.CountGoingToZero += new CancelEventHandler(StylusPoints_CountGoingToZero);
    QObject::connect(_stylusPoints.get(), &StylusPointCollection::Changed,
                     this, &Stroke::StylusPoints_Changed);
    QObject::connect(_stylusPoints.get(), &StylusPointCollection::CountGoingToZero,
                     this, &Stroke::StylusPoints_CountGoingToZero);

    // fire notification
    OnStylusPointsReplaced(e);
    OnInvalidated(EventArgs::Empty);
    OnPropertyChanged("StylusPoints");
}


/// <summary>
/// ExtendedProperties
/// </summary>
ExtendedPropertyCollection& Stroke::ExtendedProperties()
{
    if (_extendedProperties == nullptr)
    {
        _extendedProperties = new ExtendedPropertyCollection;
    }

    return *_extendedProperties;
}

/// <summary>
/// Clip
/// </summary>
/// <param name="cutAt">Fragment markers for clipping</param>
QSharedPointer<StrokeCollection> Stroke::Clip(QVector<StrokeFIndices> cutAt)
{
#ifdef DEBUG
    //
    // Assert there are  no overlaps between multiple StrokeFIndices
    //
    AssertSortedNoOverlap(cutAt);
#endif

    QSharedPointer<StrokeCollection> leftovers(new StrokeCollection);
    if (cutAt.size() == 0)
    {
        return leftovers;
    }

    if ((cutAt.size() == 1) && cutAt[0].IsFull())
    {
        leftovers->AddItem(sharedFromThis()); //clip and erase always return clones
        return leftovers;
    }


    QSharedPointer<StylusPointCollection> sourceStylusPoints = StylusPoints();
    if (GetDrawingAttributes()->FitToCurve())
    {
        sourceStylusPoints = GetBezierStylusPoints();
    }

    //
    // Assert the findices are NOT out of range with the packets
    //
    //System.Diagnostics.Debug.Assert(false == ((!DoubleUtil::AreClose(cutAt[cutAt.Length - 1].EndFIndex, StrokeFIndices::AfterLast)) &&
    //                            Math.Ceiling(cutAt[cutAt.Length - 1].EndFIndex) > sourceStylusPoints.Count - 1));

    for (int i = 0; i < cutAt.size(); i++)
    {
        StrokeFIndices fragment = cutAt[i];
        if(DoubleUtil::GreaterThanOrClose(fragment.BeginFIndex(), fragment.EndFIndex()))
        {
            // ISSUE-2004/06/26-vsmirnov - temporary workaround for bugs
            // in point erasing: drop invalid fragments
            //System.Diagnostics.Debug.Assert(DoubleUtil::LessThan(fragment.BeginFIndex, fragment.EndFIndex));
            continue;
        }

        QSharedPointer<Stroke> stroke = Copy(sourceStylusPoints, fragment.BeginFIndex(), fragment.EndFIndex());

        // Add the stroke to the output collection
        leftovers->AddItem(stroke);
    }

    return leftovers;
}

/// <summary>
///
/// </summary>
/// <param name="cutAt">Fragment markers for clipping</param>
/// <returns>Survived fragments of current Stroke as a StrokeCollection</returns>
QSharedPointer<StrokeCollection> Stroke::Erase(QVector<StrokeFIndices> cutAt)
{
#ifdef DEBUG
    //
    // Assert there are  no overlaps between multiple StrokeFIndices
    //
    AssertSortedNoOverlap(cutAt);
#endif

    QSharedPointer<StrokeCollection> leftovers(new StrokeCollection);
    // Return an empty collection if the entire stroke it to erase
    if ((cutAt.size() == 0) || ((cutAt.size() == 1) && cutAt[0].IsFull()))
    {
        return leftovers;
    }

    QSharedPointer<StylusPointCollection> sourceStylusPoints = StylusPoints();
    if (GetDrawingAttributes()->FitToCurve())
    {
        sourceStylusPoints = GetBezierStylusPoints();
    }

    //
    // Assert the findices are NOT out of range with the packets
    //
    //System.Diagnostics.Debug.Assert(false == ((!DoubleUtil::AreClose(cutAt[cutAt.Length - 1].EndFIndex, StrokeFIndices::AfterLast)) &&
    //                            Math.Ceiling(cutAt[cutAt.Length - 1].EndFIndex) > sourceStylusPoints.Count - 1));


    int i = 0;
    double beginFIndex = StrokeFIndices::BeforeFirst;
    if (cutAt[0].BeginFIndex() == StrokeFIndices::BeforeFirst)
    {
        beginFIndex = cutAt[0].EndFIndex();
        i++;
    }
    for (; i < cutAt.size(); i++)
    {
        StrokeFIndices fragment = cutAt[i];
        if(DoubleUtil::GreaterThanOrClose(beginFIndex, fragment.BeginFIndex()))
        {
            // ISSUE-2004/06/26-vsmirnov - temporary workaround for bugs
            // in point erasing: drop invalid fragments
            Debug::Assert(DoubleUtil::LessThan(beginFIndex, fragment.BeginFIndex()));
            continue;
        }


        QSharedPointer<Stroke> stroke = Copy(sourceStylusPoints, beginFIndex, fragment.BeginFIndex());
        // Add the stroke to the output collection
        leftovers->AddItem(stroke);

        beginFIndex = fragment.EndFIndex();
    }

    if (beginFIndex != StrokeFIndices::AfterLast)
    {
        QSharedPointer<Stroke> stroke = Copy(sourceStylusPoints, beginFIndex, StrokeFIndices::AfterLast);

        // Add the stroke to the output collection
        leftovers->AddItem(stroke);
    }

    return leftovers;
}


/// <summary>
/// Creates a new stroke from a subset of the points
/// </summary>
QSharedPointer<Stroke> Stroke::Copy(QSharedPointer<StylusPointCollection> sourceStylusPoints, double beginFIndex, double endFIndex)
{
    Debug::Assert(sourceStylusPoints != nullptr);
    //
    // get the floor and ceiling to copy from, we'll adjust the ends below
    //
    int beginIndex =
        (DoubleUtil::AreClose(StrokeFIndices::BeforeFirst, beginFIndex))
            ? 0 : static_cast<int>(floor(beginFIndex));

    int endIndex =
        (DoubleUtil::AreClose(StrokeFIndices::AfterLast, endFIndex))
            ? (sourceStylusPoints->size() - 1) : static_cast<int>(ceil(endFIndex));

    int pointCount = endIndex - beginIndex + 1;
    //System.Diagnostics.Debug.Assert(pointCount >= 1);

    QSharedPointer<StylusPointCollection> stylusPoints(new StylusPointCollection(_stylusPoints->Description(), pointCount));

    //
    // copy the data from the floor of beginIndex to the ceiling
    //
    for (int i = 0; i < pointCount; i++)
    {
        //System.Diagnostics.Debug.Assert(sourceStylusPoints.Count > i + beginIndex);
        StylusPoint stylusPoint = (*sourceStylusPoints)[i + beginIndex];
        stylusPoints->AddItem(stylusPoint);
    }
    //System.Diagnostics.Debug.Assert(stylusPoints.Count == pointCount);

    //
    // at this point, the stroke has been reduced to one with n number of points
    // so we need to adjust the fIndices based on the new point data
    //
    // for example, in a stroke with 4 points:
    // 0, 1, 2, 3
    //
    // if the fIndexes passed 1.1 and 2.7
    // at this point beginIndex is 1 and endIndex is 3
    //
    // now that we've copied the stroke points 1, 2 and 3, we need to
    // adjust beginFIndex to .1 and endFIndex to 1.7
    //
    if (!DoubleUtil::AreClose(beginFIndex, StrokeFIndices::BeforeFirst))
    {
        beginFIndex = beginFIndex - beginIndex;
    }
    if (!DoubleUtil::AreClose(endFIndex, StrokeFIndices::AfterLast))
    {
        endFIndex = endFIndex - beginIndex;
    }

    if (stylusPoints->size() > 1)
    {

        QPointF begPoint = (*stylusPoints)[0];
        QPointF endPoint = (*stylusPoints)[stylusPoints->size() - 1];

        // Adjust the last point to fragment.EndFIndex.
        if ((!DoubleUtil::AreClose(endFIndex, StrokeFIndices::AfterLast)) && !DoubleUtil::AreClose(endIndex, endFIndex))
        {
            //
            // for 1.7, we need to get .3, because that is the distance
            // we need to back up between the third point and the second
            //
            // so this would be .3 = 2 - 1.7
            double ceiling = ceil(endFIndex);
            double fraction = ceiling - endFIndex;

            endPoint = GetIntermediatePoint((*stylusPoints)[stylusPoints->size() - 1],
                                            (*stylusPoints)[stylusPoints->size() - 2],
                                            fraction);
            //qDebug() << "Copy endPoint" << endPoint;

        }

        // Adjust the first point to fragment.BeginFIndex.
        if ((!DoubleUtil::AreClose(beginFIndex, StrokeFIndices::BeforeFirst))
                && !DoubleUtil::AreClose(beginIndex, beginFIndex))
        {
            begPoint = GetIntermediatePoint((*stylusPoints)[0],
                                            (*stylusPoints)[1],
                                            beginFIndex);

            //qDebug() << "Copy begPoint" << begPoint;
        }

        //
        // now set the end points
        //
        StylusPoint tempEnd = (*stylusPoints)[stylusPoints->size() - 1];
        tempEnd.SetX(endPoint.x());
        tempEnd.SetY(endPoint.y());
        stylusPoints->SetItem(stylusPoints->size() - 1, tempEnd);

        StylusPoint tempBegin = (*stylusPoints)[0];
        tempBegin.SetX(begPoint.x());
        tempBegin.SetY(begPoint.y());
        stylusPoints->SetItem(0, tempBegin);
    }

    QSharedPointer<Stroke> stroke = nullptr;
    try
    {
        //
        // set a flag that tells clone not to clone the StylusPoints
        // we do this in a try finally so we alway reset our state
        // even if Clone (which is virtual) throws
        //
        _cloneStylusPoints = false;
        stroke = Clone();
        if (stroke->GetDrawingAttributes()->FitToCurve())
        {
            //
            // we're using the beziered points for the new data,
            // FitToCurve needs to be false to prevent re-bezier.
            //
            stroke->GetDrawingAttributes()->SetFitToCurve(false);
        }

        //this will reset the cachedGeometry and cachedBounds
        stroke->SetStylusPoints(stylusPoints);
        _cloneStylusPoints = true;
    }
    catch (...)
    {
        _cloneStylusPoints = true;
    }

    return stroke;
}

/// <summary>
/// helper that will generate a new point between two points at an findex
/// </summary>
QPointF Stroke::GetIntermediatePoint(StylusPoint const & p1, StylusPoint const & p2, double findex)
{
    double xDistance = p2.X() - p1.X();
    double yDistance = p2.Y() - p1.Y();

    double xFDistance = xDistance * findex;
    double yFDistance = yDistance * findex;

    return QPointF(p1.X() + xFDistance, p1.Y() + yFDistance);
}


#ifdef DEBUG
/// <summary>
/// Helper method used to validate that the strokefindices in the array
/// are sorted and there are no overlaps
/// </summary>
/// <param name="fragments">fragments</param>
void AssertSortedNoOverlap(StrokeFIndices[] fragments)
{
    if (fragments.Length == 0)
    {
        return;
    }
    if (fragments.Length == 1)
    {
        System.Diagnostics.Debug.Assert(IsValidStrokeFIndices(fragments[0]));
        return;
    }
    double current = StrokeFIndices::BeforeFirst;
    for (int x = 0; x < fragments.Length; x++)
    {
        if (fragments[x].BeginFIndex <= current)
        {
            //
            // when x == 0, we're just starting, any value is valid
            //
            System.Diagnostics.Debug.Assert(x == 0);
        }
        current = fragments[x].BeginFIndex;
        System.Diagnostics.Debug.Assert(IsValidStrokeFIndices(fragments[x]) && fragments[x].EndFIndex > current);
        current = fragments[x].EndFIndex;
    }
}

bool IsValidStrokeFIndices(StrokeFIndices findex)
{
    return (!double.IsNaN(findex.BeginFIndex) && !double.IsNaN(findex.EndFIndex) && findex.BeginFIndex < findex.EndFIndex);
}

#endif


/// <summary>
/// Method called whenever the Stroke's drawing attributes are changed.
/// This method will trigger an event for any listeners interested in
/// drawing attributes.
/// </summary>
/// <param name="sender">The Drawing Attributes object that was changed</param>
/// <param name="e">More data about the change that occurred</param>
void Stroke::DrawingAttributes_Changed(PropertyDataChangedEventArgs& e)
{
    // set Geometry flag to be dirty if the DA change will cause change in geometry
    std::unique_ptr<Geometry> geometry;
    if (DrawingAttributes::IsGeometricalDaGuid(e.PropertyGuid()) == true)
    {
        SetGeometry(geometry);
        // Set the cached bounds to empty, which will force a re-calculation of the _cachedBounds upon next GetBounds call.
        _cachedBounds = QRectF();
    }

    OnDrawingAttributesChanged(e);
    if (!_delayRaiseInvalidated)
    {
        //when Stroke.Transform(Matrix, bool) is called, we don't raise invalidated from
        //here, but rather from the Stroke.Transform method.
        OnInvalidated(EventArgs::Empty);
    }
}

/// <summary>
/// Method called whenever the Stroke's StylusPoints are changed.
/// This method will trigger an event for any listeners interested in
/// Invalidate
/// </summary>
/// <param name="sender">The StylusPoints object that was changed</param>
/// <param name="e">event args</param>
void Stroke::StylusPoints_Changed()
{
    std::unique_ptr<Geometry> geometry;
    SetGeometry(geometry);
    _cachedBounds = QRectF();

    OnStylusPointsChanged();
    if (!_delayRaiseInvalidated)
    {
        //when Stroke.Transform(Matrix, bool) is called, we don't raise invalidated from
        //here, but rather from the Stroke.Transform method.
        OnInvalidated(EventArgs::Empty);
    }
}

void Stroke::StylusPoints_CountGoingToZero(CancelEventArgs& e)
{
    e.SetCancel(true);
    //StylusPoints will raise the exception
}

/// <summary>
/// Computes the bounds of the stroke in the default rendering context
/// </summary>
/// <returns></returns>
QRectF Stroke::GetBounds()
{
    if (_cachedBounds.isEmpty())
    {
        StrokeNodeIterator iterator = StrokeNodeIterator::GetIterator(*this, *GetDrawingAttributes());
        for (int i = 0; i < iterator.Count(); i++)
        {
            StrokeNode strokeNode = iterator[i];
            _cachedBounds |= strokeNode.GetBounds();
        }
    }

    return _cachedBounds;
}

/// <summary>
/// Render the Stroke under the specified DrawingContext. The draw method is a
/// batch operationg that uses the rendering methods exposed off of DrawingContext
/// </summary>
/// <param name="context"></param>
void Stroke::Draw(DrawingContext &context)
{
    //if (nullptr == context)
    //{
    //    throw std::runtime_error("context");
    //}

    //our code never calls this public API so we can assume that opacity
    //has not been set up

    //call our public Draw method with the strokes.DA
    Draw(context, GetDrawingAttributes());
}


/// <summary>
/// Render the StrokeCollection under the specified DrawingContext. This draw method uses the
/// passing in drawing attribute to override that on the stroke.
/// </summary>
/// <param name="drawingContext"></param>
/// <param name="drawingAttributes"></param>
void Stroke::Draw(DrawingContext & drawingContext, QSharedPointer<DrawingAttributes>  drawingAttributes)
{
    //if (nullptr == drawingContext)
    //{
    //    throw std::runtime_error("context");
    //}


    //             context.VerifyAccess();

    //our code never calls this public API so we can assume that opacity
    //has not been set up

    if (drawingAttributes->IsHighlighter())
    {
        drawingContext.PushOpacity(StrokeRenderer::HighlighterOpacity);
        try
        {
            QSharedPointer<DrawingAttributes> da(StrokeRenderer::GetHighlighterAttributes(*this, GetDrawingAttributes()));
            DrawInternal(drawingContext, da, false);
            drawingContext.Pop();
        }
        catch(...)
        {
            drawingContext.Pop();
            throw;
        }
    }
    else
    {
        DrawInternal(drawingContext, drawingAttributes, false);
    }
}


/// <summary>
/// Clip with rect. Calculate the after-clipping Strokes. Only the "in-segments" are left after this operation.
/// </summary>
/// <param name="bounds">A Rect to clip with</param>
/// <returns>The after-clipping strokes.</returns>
QSharedPointer<StrokeCollection> Stroke::GetClipResult(QRectF const & bounds)
{
    return GetClipResult({ bounds.topLeft(), bounds.topRight(), bounds.bottomRight(), bounds.bottomLeft() });
}


/// <summary>
/// Clip with lasso. Calculate the after-clipping Strokes. Only the "in-segments" are left after this operation.
/// </summary>
/// <param name="lassoPoints">The lasso points to clip with</param>
/// <returns>The after-clipping strokes</returns>
QSharedPointer<StrokeCollection> Stroke::GetClipResult(QVector<QPointF> const & lassoPoints)
{
    if (lassoPoints.count() == 0)
    {
        throw std::runtime_error("SR.Get(SRID.EmptyArray)");
    }

    SingleLoopLasso lasso;
    lasso.AddPoints(lassoPoints);
    return Clip(HitTest(lasso));
}


/// <summary>
/// Erase with a rect. Calculate the after-erasing Strokes. Only the "out-segments" are left after this operation.
/// </summary>
/// <param name="bounds">A Rect to clip with</param>
/// <returns>The after-erasing strokes</returns>
QSharedPointer<StrokeCollection> Stroke::GetEraseResult(QRectF const & bounds)
{
    return GetEraseResult({ bounds.topLeft(), bounds.topRight(), bounds.bottomRight(), bounds.bottomLeft() });
}

/// <summary>
/// Erase with lasso points.
/// </summary>
/// <param name="lassoPoints">Lasso points to erase with</param>
/// <returns>The after-erasing strokes</returns>
QSharedPointer<StrokeCollection> Stroke::GetEraseResult(QVector<QPointF> const & lassoPoints)
{
    if (lassoPoints.size() == 0)
    {
        throw std::runtime_error("SR.Get(SRID.EmptyArray)");
    }

    SingleLoopLasso lasso;
    lasso.AddPoints(lassoPoints);
    return Erase(HitTest(lasso));
}

/// <summary>
/// Erase with an eraser with passed in shape
/// </summary>
/// <param name="eraserPath">The path to erase</param>
/// <param name="eraserShape">Shape of the eraser</param>
/// <returns></returns>
QSharedPointer<StrokeCollection> Stroke::GetEraseResult(QVector<QPointF> const & eraserPath, StylusShape & eraserShape)
{
    // Check the input parameters
    //if (eraserShape == nullptr)
    //{
    //    throw std::runtime_error("eraserShape");
    //}

    return Erase(EraseTest(eraserPath, eraserShape));
}


/// <summary>
/// Tap-hit. Hit tests with a point. Internally does Stroke.HitTest(Point, 1pxlRectShape).
/// </summary>
/// <param name="point">The location to do the hitest</param>
/// <returns>True is this stroke is hit, false otherwise</returns>
bool Stroke::HitTest(QPointF const & point)
{
    EllipseStylusShape stylusShape(TapHitPointSize, TapHitPointSize, TapHitRotation);
    return HitTest({point}, stylusShape);
}

/// <summary>
/// Tap-hit. Hit tests with a point.
/// </summary>
/// <param name="point">The location to do the hittest</param>
/// <param name="diameter">diameter of the tip</param>
/// <returns>true if hit, false otherwise</returns>
bool Stroke::HitTest(QPointF const & point, double diameter)
{
    if (qIsNaN(diameter) || diameter < DrawingAttributes::MinWidth || diameter > DrawingAttributes::MaxWidth)
    {
        throw std::runtime_error("diameter");
    }
    EllipseStylusShape stylusShape(diameter, diameter, TapHitRotation);
    return HitTest({point}, stylusShape);
}

/// <summary>
/// Check whether a certain percentage of the stroke is within the Rect passed in.
/// </summary>
/// <param name="bounds"></param>
/// <param name="percentageWithinBounds"></param>
/// <returns></returns>
bool Stroke::HitTest(QRectF const & bounds, int percentageWithinBounds)
{
    if ((percentageWithinBounds < 0) || (percentageWithinBounds > 100))
    {
        throw std::runtime_error("percentageWithinBounds");
    }

    if (percentageWithinBounds == 0)
    {
        return true;
    }

    StrokeInfo strokeInfo(sharedFromThis());
    //try
    //{
        //strokeInfo = new StrokeInfo(this);

        QSharedPointer<StylusPointCollection> stylusPoints = strokeInfo.StylusPoints();
        double target = strokeInfo.TotalWeight() * percentageWithinBounds / 100.0f - PercentageTolerance;

        for (int i = 0; i < stylusPoints->size(); i++)
        {
            if (true == bounds.contains((*stylusPoints)[i]))
            {
                target -= strokeInfo.GetPointWeight(i);
                if (DoubleUtil::LessThanOrClose(target, 0))
                {
                    return true;
                }
            }
        }

        return false;
    //}
    //finally
    //{
    //    if (strokeInfo != null)
    //    {
    //        //detach from event handlers, or else we leak.
    //        strokeInfo.Detach();
    //    }
    //}
}

/// <summary>
/// Check whether a certain percentage of the stroke is within the lasso
/// </summary>
/// <param name="lassoPoints"></param>
/// <param name="percentageWithinLasso"></param>
/// <returns></returns>
bool Stroke::HitTest(QVector<QPointF> const & lassoPoints, int percentageWithinLasso)
{
    if ((percentageWithinLasso < 0) || (percentageWithinLasso > 100))
    {
        throw std::runtime_error("percentageWithinLasso");
    }

    if (percentageWithinLasso == 0)
    {
        return true;
    }


    StrokeInfo strokeInfo(sharedFromThis());
    //try
    //{
        //strokeInfo = new StrokeInfo(this);

        QSharedPointer<StylusPointCollection> stylusPoints = strokeInfo.StylusPoints();
        double target = strokeInfo.TotalWeight() * percentageWithinLasso / 100.0f - PercentageTolerance;

        SingleLoopLasso lasso;
        lasso.AddPoints(lassoPoints);

        for (int i = 0; i < stylusPoints->size(); i++)
        {
            if (true == lasso.Contains((*stylusPoints)[i]))
            {
                target -= strokeInfo.GetPointWeight(i);
                if (DoubleUtil::LessThan(target, 0))
                {
                    return true;
                }
            }
        }

        return false;
    //}
    //finally
    //{
    //    if (strokeInfo != null)
    //    {
    //        //detach from event handlers, or else we leak.
    //        strokeInfo.Detach();
    //    }
    //}

}

/// <summary>
///
/// </summary>
/// <param name="path"></param>
/// <param name="stylusShape"></param>
/// <returns></returns>
bool Stroke::HitTest(QVector<QPointF> const & path, StylusShape &stylusShape)
{
    //if (stylusShape == nullptr)
    //{
    //    throw std::runtime_error("stylusShape");
    //}

    if (path.size() == 0)
    {
        return false;
    }

    ErasingStroke erasingStroke(stylusShape);
    erasingStroke.MoveTo(path);

    QRectF erasingBounds = erasingStroke.Bounds();

    if (erasingBounds.isEmpty())
    {
        return false;
    }

    if (erasingBounds.intersects(GetBounds()))
    {
        return erasingStroke.HitTest(StrokeNodeIterator::GetIterator(*this, *GetDrawingAttributes()));
    }

    return false;
}


/// <summary>
/// The core functionality to draw a stroke. The function can be called from the following code paths.
///     i) From StrokeVisual.OnRender
///         a. Highlighter strokes have been grouped and the correct opacity has been set on the container visual.
///         b. For a highlighter stroke with color.A != 255, the DA passed in is a copy with color.A set to 255.
///         c. _drawAsHollow can be true, i.e., Selected stroke is drawn as hollow
///     ii) From StrokeCollection.Draw.
///         a. Highlighter strokes have been grouped and the correct opacity has been pushed.
///         b. For a highlighter stroke with color.A != 255, the DA passed in is a copy with color.A set to 255.
///         c. _drawAsHollow is always false, i.e., Selected stroke is not drawn as hollow
///     iii) From Stroke.Draw
///         a. The correct opacity has been pushed for a highlighter stroke
///         b. For a highlighter stroke with color.A != 255, the DA passed in is a copy with color.A set to 255.
///         c. _drawAsHollow is always false, i.e., Selected stroke is not drawn as hollow
/// We need to document the following:
/// 1) our default implementation so developers can see what we've done here -
///    including how we handle IsHollow
/// 2) the fact that opacity has already been set up correctly for the call.
/// 3) that developers should not call base.DrawCore if they override this
/// </summary>
/// <param name="drawingContext">DrawingContext to draw on</param>
/// <param name="drawingAttributes">DrawingAttributes to draw with</param>
void Stroke::DrawCore(DrawingContext & drawingContext, QSharedPointer<DrawingAttributes>  drawingAttributes)
{
    //if (nullptr == drawingContext)
    //{
    //    throw std::runtime_error("drawingContext");
    //}

    if (_drawAsHollow == true)
    {

        // Draw as hollow. Our profiler result shows that the two-pass-rendering approach is about 5 times
        // faster that using GetOutlinePathGeometry.
        // also, the minimum display size for selected ink is our default width / height

        QMatrix innerTransform, outerTransform;
        QSharedPointer<DrawingAttributes>  selectedDA = drawingAttributes->Clone();
        selectedDA->SetHeight(qMax(selectedDA->Height(), DrawingAttributes::DefaultHeight));
        selectedDA->SetWidth(qMax(selectedDA->Width(), DrawingAttributes::DefaultWidth));
        CalcHollowTransforms(selectedDA, innerTransform, outerTransform);

        // First pass drawing. Use drawingAttributes->Color to create a solid color brush. The stroke will be drawn as
        // 1 avalon-unit higher and wider (HollowLineSize = 1.0f)
        selectedDA->SetStylusTipTransform(outerTransform);
        QBrush brush(drawingAttributes->Color());
        //brush.Freeze();
        drawingContext.DrawGeometry(brush, Qt::NoPen, GetGeometry(selectedDA));

        //Second pass drawing with a white color brush. The stroke will be drawn as
        // 1 avalon-unit shorter and narrower (HollowLineSize = 1.0f) if the actual-width/height (considering StylusTipTransform)
        // is larger than HollowLineSize. Otherwise the same size stroke is drawn.
        selectedDA->SetStylusTipTransform(innerTransform);
        drawingContext.DrawGeometry(QBrush(Qt::white), Qt::NoPen, GetGeometry(selectedDA));
    }
    else
    {
#if DEBUG_RENDERING_FEEDBACK
        //render debug feedback?
        QUuid guid("52053C24-CBDD-4547-AAA1-DEFEBF7FD1E1");
        if (ContainsPropertyData(guid))
        {
            double thickness = GetPropertyData(guid).toDouble();

            //first, draw the outline of the stroke
            drawingContext.DrawGeometry(QBrush(),
                                        QPen(Qt::black, thickness),
                                        GetGeometry());

            Geometry* g2 = nullptr;
            QRectF b2;
            //next, overlay the connecting quad points
            StrokeNodeIterator iterator = StrokeNodeIterator::GetIterator(*this, *drawingAttributes);
            StrokeRenderer::CalcGeometryAndBounds(iterator,
                                                 *drawingAttributes,
                                                 drawingContext, thickness, true,
                                                 true, //calc bounds
                                                 g2,
                                                 b2);

        }
        else
        {
#endif
            QBrush brush(drawingAttributes->Color());
            //brush.Freeze();
            drawingContext.DrawGeometry(brush, Qt::NoPen, GetGeometry(drawingAttributes));
#if DEBUG_RENDERING_FEEDBACK
        }
#endif
    }
}

/// <summary>
/// Returns the Geometry of this stroke.
/// </summary>
/// <returns></returns>
Geometry* Stroke::GetGeometry()
{
    return GetGeometry(GetDrawingAttributes());
}

/// <summary>
/// Get the Geometry of the Stroke
/// </summary>
/// <param name="drawingAttributes"></param>
/// <returns></returns>
Geometry * Stroke::GetGeometry(QSharedPointer<DrawingAttributes> drawingAttributes)
{
    bool geometricallyEqual = DrawingAttributes::GeometricallyEqual(*drawingAttributes, *GetDrawingAttributes());

    // need to recalculate the PathGemetry if the DA passed in is "geometrically" different from
    // this DA, or if the cached PathGeometry is dirty.
    if (false == geometricallyEqual || (true == geometricallyEqual && nullptr == _cachedGeometry))
    {
        //Recalculate _pathGeometry;
        StrokeNodeIterator iterator = StrokeNodeIterator::GetIterator(*this, *drawingAttributes);
        Geometry * geometry = nullptr;
        QRectF bounds;
        std::unique_ptr<DrawingContext> debugDC;
        StrokeRenderer::CalcGeometryAndBounds(iterator,
                                             *drawingAttributes,
#if DEBUG_RENDERING_FEEDBACK
                                             *debugDC, 0, false,
#endif
                                             true, //calc bounds
                                             geometry,
                                             bounds);

        // return the calculated value directly. We cannot cache the result since the DA passed in
        // is "geometrically" different from this.DrawingAttributes.
        if (false == geometricallyEqual)
        {
            return geometry;
        }

        // Cache the value and set _isPathGeometryDirty to false;
        SetGeometry(geometry);
        SetBounds(bounds);

        return geometry;
    }

    // return a ref to our _cachedGeometry
    //System.Diagnostics.Debug.Assert(_cachedGeometry != null && _cachedGeometry.IsFrozen);
    return _cachedGeometry;
}


/// <summary>
/// our code - StrokeVisual.OnRender and StrokeCollection.Draw - always calls this
/// so we can assume the correct opacity has already been pushed on dc. The flag drawAsHollow is set
/// to true when this function is called from Renderer and this.IsSelected == true.
/// </summary>
void Stroke::DrawInternal(DrawingContext& dc, QSharedPointer<DrawingAttributes> DrawingAttributes, bool drawAsHollow)
{
    if (drawAsHollow == true)
    {
        // The Stroke.DrawCore may be overriden in the 3rd party code.
        // The out-side code could throw exception. We use try/finally block to protect our status.
        //try
        {
            FinallyHelper final([this](){
                _drawAsHollow = false;  // reset _drawAsHollow
            });
            _drawAsHollow = true;  // temporarily set the flag to be true
            DrawCore(dc, DrawingAttributes);
        }
        //catch(...)
        //{
        //    _drawAsHollow = false;  // reset _drawAsHollow
        //}
    }
    else
    {
        // IsSelected can be true or false, but _drawAsHollow must be false
        //System.Diagnostics.Debug.Assert(false == _drawAsHollow);
        DrawCore(dc, DrawingAttributes);
    }
}


void Stroke::SetIsSelected(bool value)
{
    if (value != _isSelected)
    {
        _isSelected = value;

        // Raise Invalidated event. This will cause Renderer to repaint and call back DrawCore
        OnInvalidated(EventArgs::Empty);
    }
}

/// <summary>
/// Set the path geometry
/// </summary>
void Stroke::SetGeometry(Geometry* geometry)
{
    //System.Diagnostics.Debug.Assert(geometry != null);
    if (_cachedGeometry) {
        delete _cachedGeometry;
    }
    _cachedGeometry = geometry;
    if (_cachedGeometry)
        _cachedGeometry->tryTakeOwn(this);
}

void Stroke::SetGeometry(std::unique_ptr<Geometry>& geometry)
{
    std::unique_ptr<Geometry> cachedGeometry(_cachedGeometry);
    cachedGeometry.swap(geometry);
    if (cachedGeometry)
        cachedGeometry->tryTakeOwn(this);
    _cachedGeometry = cachedGeometry.release();
}

/// <summary>Hit tests all segments within a contour generated with shape and path</summary>
/// <param name="shape"></param>
/// <param name="path"></param>
/// <returns>StrokeIntersection array for these segments</returns>
QVector<StrokeIntersection> Stroke::EraseTest(QVector<QPointF> const& path, StylusShape & shape)
{
    //System.Diagnostics.Debug.Assert(shape != null);
    //System.Diagnostics.Debug.Assert(path != null);
    if (path.size() == 0)
    {
        return QVector<StrokeIntersection>();
    }

    ErasingStroke erasingStroke(shape, path);
    QList<StrokeIntersection> intersections;
    erasingStroke.EraseTest(StrokeNodeIterator::GetIterator(*this, *GetDrawingAttributes()), intersections);
    return intersections.toVector();
}

/// <summary>
/// Hit tests all segments within the lasso loops
/// </summary>
/// <returns> a StrokeIntersection array for these segments</returns>
QVector<StrokeIntersection> Stroke::HitTest(Lasso& lasso)
{
    // Check the input parameters
    //System.Diagnostics.Debug.Assert(lasso != null);
    if (lasso.IsEmpty())
    {
        return QVector<StrokeIntersection>();
    }

    // The following will check whether all the points are within the lasso.
    // If yes, return the whole stroke as being hit.
    if (!lasso.Bounds().intersects(GetBounds()))
    {
        return QVector<StrokeIntersection>();
    }
    StrokeNodeIterator sni = StrokeNodeIterator::GetIterator(*this, *GetDrawingAttributes());
    return lasso.HitTest(sni);
}

//QDebug & operator<<(QDebug & d, StrokeIntersection const & v)
//{
//    d << v.ToString();
//    return d;
//}

//QDebug & operator<<(QDebug & d, StrokeFIndices const & v)
//{
//    d << v.ToString();
//    return d;
//}

/// <summary>
/// Calculate the after-erasing Strokes. Only the "out-segments" are left after this operation.
/// </summary>
/// <param name="cutAt">Array of intersections indicating the erasing locations</param>
/// <returns></returns>
QSharedPointer<StrokeCollection> Stroke::Erase(QVector<StrokeIntersection> cutAt)
{
    // Nothing needs to be erased
    if(cutAt.size() == 0)
    {
        QSharedPointer<StrokeCollection> strokes(new StrokeCollection);
        strokes->AddItem(sharedFromThis()); //clip and erase always return clones for this condition
        return strokes;
    }

    // Two assertions are deferred to the private erase function to avoid duplicate code.
    // 1. AssertSortedNoOverlap
    // 2. Check whether the insegments are out of range with the packets
    QVector<StrokeFIndices> hitSegments = StrokeIntersection::GetHitSegments(cutAt);
    //qDebug() << "Stroke::Erase cutAt" << cutAt;
    //qDebug() << "Stroke::Erase hitSegments" << hitSegments;
    return Erase(hitSegments);
}

/// <summary>
/// Calculate the after-clipping Strokes. Only the "in-segments" are left after this operation.
/// </summary>
/// <param name="cutAt">Array of intersections indicating the clipping locations</param>
/// <returns>The resulting StrokeCollection</returns>
QSharedPointer<StrokeCollection> Stroke::Clip(QVector<StrokeIntersection> cutAt)
{
    // Nothing is inside
    if (cutAt.size() == 0)
    {
        return QSharedPointer<StrokeCollection>(new StrokeCollection);
    }


    // Get the "in-segments"
    QVector<StrokeFIndices> inSegments = StrokeIntersection::GetInSegments(cutAt);

    // For special case like cutAt is {BF, AL, BF, 0.67}, the inSegments are empty
    if (inSegments.size() == 0)
    {
        return QSharedPointer<StrokeCollection>(new StrokeCollection);
    }

    // Two other validations are deferred to the private clip function to avoid duplicate code.
    // 1. ValidateSortedNoOverlap
    // 2. Check whether the insegments are out of range with the packets
    return Clip(inSegments);
}

/// <summary>
/// Calculate the two transforms for two-pass rendering used to draw as hollow. The resulting outerTransform will make the
/// first-pass-rendering 1 avalon-unit wider/heigher. The resulting innerTransform will make the second-pass-rendering 1 avalon-unit
/// narrower/shorter.
/// </summary>
void Stroke::CalcHollowTransforms(QSharedPointer<DrawingAttributes>  originalDa, QMatrix & innerTransform, QMatrix & outerTransform)
{

    //System.Diagnostics.Debug.Assert(DoubleUtil::IsZero(originalDa->StylusTipTransform.OffsetX) && DoubleUtil::IsZero(originalDa->StylusTipTransform.OffsetY));

    innerTransform = outerTransform = QMatrix();
    QPointF w = originalDa->StylusTipTransform().map(QPointF(originalDa->Width(), 0));
    QPointF h = originalDa->StylusTipTransform().map(QPoint(0, originalDa->Height()));

    // the newWidth and newHeight are the actual width/height of the stylus shape considering StylusTipTransform.
    // The assumption is TylusTipTransform has no translation component.
    double newWidth = qSqrt(w.x() * w.x() + w.y() * w.y());
    double newHeight = qSqrt(h.x() * h.x() + h.y() * h.y());

    double xTransform = DoubleUtil::GreaterThan(newWidth, HollowLineSize) ?
                        (newWidth - HollowLineSize) / newWidth : 1.0f;
    double yTransform = DoubleUtil::GreaterThan(newHeight, HollowLineSize) ?
                        (newHeight - HollowLineSize) / newHeight : 1.0f;

    innerTransform.scale(xTransform, yTransform);
    innerTransform *= originalDa->StylusTipTransform();

    outerTransform.scale((newWidth + HollowLineSize) / newWidth,
                         (newHeight + HollowLineSize) / newHeight);
    outerTransform *= originalDa->StylusTipTransform();
}


