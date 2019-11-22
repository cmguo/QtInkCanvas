#include "styluspointcollection.h"
#include "styluspoint.h"
#include "styluspointdescription.h"
#include "styluspointproperties.h"
#include "strokecollectionserializer.h"
#include "styluspointpropertyinfodefaults.h"
#include "events.h"

StylusPointCollection::StylusPointCollection()
    : _stylusPointDescription(new StylusPointDescription())
{
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="initialCapacity">initialCapacity
StylusPointCollection::StylusPointCollection(int initialCapacity)
    : StylusPointCollection()
{
    if (initialCapacity < 0)
    {
        throw std::exception("initialCapacity");
    }
    QList<StylusPoint>::reserve(initialCapacity);
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
StylusPointCollection::StylusPointCollection(QSharedPointer<StylusPointDescription> stylusPointDescription)
{
    if (nullptr == stylusPointDescription)
    {
        throw std::exception();
    }
    _stylusPointDescription = stylusPointDescription;
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
/// <param name="initialCapacity">initialCapacity
StylusPointCollection::StylusPointCollection(QSharedPointer<StylusPointDescription> stylusPointDescription, int initialCapacity)
    : StylusPointCollection (stylusPointDescription)
{
    if (initialCapacity < 0)
    {
        throw std::exception("initialCapacity");
    }
    reserve(initialCapacity);
}


/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="stylusPoints">stylusPoints
StylusPointCollection::StylusPointCollection(QVector<StylusPoint> const & stylusPoints)
    //: this() //don't call the base ctor, we want to use the first sp
{
    QVector<StylusPoint> points(stylusPoints);
    if (points.size() == 0)
    {
        throw std::exception("stylusPoints");
    }

    //
    // set our packet description to the first in the array
    //
    _stylusPointDescription = points[0].Description();

    reserve(points.size());
    for (int x = 0; x < points.size(); x++)
    {
        push_back(points[x]);
    }
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="points">points
StylusPointCollection::StylusPointCollection(QVector<QPointF> const & points)
    : StylusPointCollection()
{
    QList<StylusPoint> stylusPoints;
    for (QPointF point : points)
    {
        //this can throw (since point.X or Y can be beyond our range)
        //don't add to our collection until after we instance
        //all of the styluspoints and we know the ranges are valid
        stylusPoints.push_back(StylusPoint(point));
    }

    if (stylusPoints.size() == 0)
    {
        throw std::exception("points");
    }

    reserve(stylusPoints.size());
    append(stylusPoints);
}

/// <summary>
/// ctor called by input with a raw int[]
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
/// <param name="rawPacketData">rawPacketData
/// <param name="tabletToView">tabletToView
/// <param name="tabletToViewMatrix">tabletToView
StylusPointCollection::StylusPointCollection(QSharedPointer<StylusPointDescription> stylusPointDescription, QVector<int> rawPacketData, QMatrix & tabletToView, QMatrix & tabletToViewMatrix)
{
    if (nullptr == stylusPointDescription)
    {
        throw std::exception();
    }
    _stylusPointDescription = stylusPointDescription;

    int lengthPerPoint = stylusPointDescription->GetInputArrayLengthPerPoint();
    int logicalPointCount = rawPacketData.size() / lengthPerPoint;
    //Debug.Assert(0 == rawPacketData.Length % lengthPerPoint, "Invalid assumption about packet length, there shouldn't be any remainder");

    //
    // set our capacity and validate
    //
    reserve(logicalPointCount);
    for (int count = 0, i = 0; count < logicalPointCount; count++, i += lengthPerPoint)
    {

        //first, determine the x, y values by xf-ing them
        QPointF p(rawPacketData[i], rawPacketData[i + 1]);
        //if (tabletToView != nullptr)
        //{
        //    tabletToView.TryTransform(p, p);
        //}
        //else
        {
            p = tabletToViewMatrix.map(p);
        }

        int startIndex = 2;
        bool containsTruePressure = stylusPointDescription->ContainsTruePressure();
        if (containsTruePressure)
        {
            //don't copy pressure in the int[] for extra data
            startIndex++;
        }

        QVector<int> data;
        int dataLength = lengthPerPoint - startIndex;
        if (dataLength > 0)
        {
            //copy the rest of the data
            data.resize(dataLength);
            for (int localIndex = 0, rawArrayIndex = i + startIndex; localIndex < data.size(); localIndex++, rawArrayIndex++)
            {
                data[localIndex] = rawPacketData[rawArrayIndex];
            }
        }

        StylusPoint newPoint(p.x(), p.y(), StylusPoint::DefaultPressure, _stylusPointDescription, data, false, false);
        if (containsTruePressure)
        {
            //use the algoritm to set pressure in StylusPoint
            int pressure = rawPacketData[i + 2];
            newPoint.SetPropertyValue(StylusPointProperties::NormalPressure, pressure);
        }

        //this does not go through our virtuals
        push_back(newPoint);
    }
}

/// <summary>
/// Adds the StylusPoints in the StylusPointCollection to this StylusPointCollection
/// </summary>
/// <param name="stylusPoints">stylusPoints
void StylusPointCollection::Add(StylusPointCollection & stylusPoints)
{
    //note that we don't raise an exception if stylusPoints.Count == 0
    if (!StylusPointDescription::AreCompatible(stylusPoints.Description(),
                                                _stylusPointDescription))
    {
        throw std::exception("stylusPoints");
    }

    // cache count outside of the loop, so if this SPC is ever passed
    // we don't loop forever
    int count = stylusPoints.size();
    for (int x = 0; x < count; x++)
    {
        StylusPoint stylusPoint = stylusPoints[x];
        stylusPoint.SetDescription(_stylusPointDescription);
        //this does not go through our virtuals
        push_back(stylusPoint);
    }

    if (stylusPoints.size() > 0)
    {
        OnChanged();
    }
}

/// <summary>
/// Read only access to the StylusPointDescription shared by the StylusPoints in this collection
/// </summary>
QSharedPointer<StylusPointDescription> StylusPointCollection::Description()
{
    if (nullptr == _stylusPointDescription)
    {
        _stylusPointDescription.reset(new StylusPointDescription());
    }
    return _stylusPointDescription;
}

/// <summary>
/// called by base class Collection<T> when the list is being cleared;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::ClearItems()
{
    if (CanGoToZero())
    {
        clear();
        OnChanged();
    }
    else
    {
        throw std::exception();
    }
}

/// <summary>
/// called by base class Collection<T> when an item is removed from list;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::RemoveItem(int index)
{
    if (this->size() > 1 || CanGoToZero())
    {
        removeAt(index);
        OnChanged();
    }
    else
    {
        throw std::exception();
    }

}

/// <summary>
/// called by base class Collection<T> when an item is added to list;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::InsertItem(int index, StylusPoint & stylusPoint)
{
    if (!StylusPointDescription::AreCompatible(stylusPoint.Description(),
                                            _stylusPointDescription))
    {
        throw std::exception("stylusPoint");
    }

    stylusPoint.SetDescription(_stylusPointDescription);
    insert(index, stylusPoint);

    OnChanged();
}

/// <summary>
/// called by base class Collection<T> when an item is set in list;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::SetItem(int index, StylusPoint & stylusPoint)
{
    if (!StylusPointDescription::AreCompatible(stylusPoint.Description(),
                                            _stylusPointDescription))
    {
        throw std::exception("stylusPoint");
    }

    stylusPoint.SetDescription(_stylusPointDescription);
    (*this)[index] = stylusPoint;

    OnChanged();
}

/// <summary>
/// Clone
/// </summary>
StylusPointCollection * StylusPointCollection::Clone()
{
    return Clone(QMatrix(1, 0, 0, 1, 0, 0), Description(), size());
}

/// <summary>
/// Explicit cast converter between StylusPointCollection and Point[]
/// </summary>
/// <param name="stylusPoints">stylusPoints
StylusPointCollection::operator QVector<QPointF>()
{
    QVector<QPointF> points(size());
    for (int i = 0; i < size(); i++)
    {
        points[i] = (*this)[i];
    }
    return points;
}

/// <summary>
/// Clone and truncate
/// </summary>
/// <param name="count">The maximum count of points to clone (used by GestureRecognizer)
/// <returns></returns>
StylusPointCollection * StylusPointCollection::Clone(int count)
{
    if (count > size() || count < 1)
    {
        throw std::exception("count");
    }

    return Clone(QMatrix(1, 0, 0, 1, 0, 0), Description(), count);

}

/// <summary>
/// Clone with a transform, used by input
/// </summary>
StylusPointCollection * StylusPointCollection::Clone(QMatrix const & transform, QSharedPointer<StylusPointDescription> descriptionToUse)
{
    return Clone(transform, descriptionToUse, size());
}


/// <summary>
/// clone implementation
/// </summary>
StylusPointCollection * StylusPointCollection::Clone(QMatrix const & transform, QSharedPointer<StylusPointDescription> descriptionToUse, int count)
{
    //Debug.Assert(count <= this.Count);
    //
    // We don't need to copy our _stylusPointDescription because it is immutable
    // and we don't need to copy our StylusPoints, because they are structs.
    //
    StylusPointCollection & newCollection(*new StylusPointCollection(descriptionToUse, count));

    bool isIdentity = transform.isIdentity();
    for (int x = 0; x < count; x++)
    {
        if (isIdentity)
        {
            newCollection.push_back((*this)[x]);
        }
        else
        {
            QPointF point;
            StylusPoint stylusPoint = (*this)[x];
            point = stylusPoint;
            point = transform.map(point);
            stylusPoint = point;
            newCollection.push_back(stylusPoint);
        }
    }
    return &newCollection;
}

/// <summary>
/// virtual for raising changed notification
/// </summary>
/// <param name="e">
void StylusPointCollection::OnChanged()
{
}

/// <summary>
/// Transform the StylusPoints in this collection by the specified transform
/// </summary>
/// <param name="transform">transform
void StylusPointCollection::Transform(QMatrix const & transform)
{
    QPointF point;
    for (int i = 0; i < size(); i++)
    {
        StylusPoint stylusPoint = (*this)[i];
        point = stylusPoint;
        point = transform.map(point);
        stylusPoint = point;

        //this does not go through our virtuals
        (*this)[i] = stylusPoint;
    }

    if (size() > 0)
    {
        OnChanged();
    }
}

/// <summary>
/// Reformat
/// </summary>
/// <param name="subsetToReformatTo">subsetToReformatTo
StylusPointCollection * StylusPointCollection::Reformat(QSharedPointer<StylusPointDescription> subsetToReformatTo)
{
    return Reformat(subsetToReformatTo, QMatrix(1, 0, 0, 1, 0, 0));
}

/// <summary>
/// Helper that transforms and scales in one go
/// </summary>
StylusPointCollection * StylusPointCollection::Reformat(QSharedPointer<StylusPointDescription> subsetToReformatTo, QMatrix const & transform)
{
    if (!subsetToReformatTo->IsSubsetOf(Description()))
    {
        throw std::exception("subsetToReformatTo");
    }

    QSharedPointer<StylusPointDescription> subsetToReformatToWithCurrentMetrics =
        StylusPointDescription::GetCommonDescription(subsetToReformatTo,
                                                    Description()); //preserve metrics from this spd

    if (StylusPointDescription::AreCompatible(Description(), subsetToReformatToWithCurrentMetrics) &&
        transform.isIdentity())
    {
        //subsetToReformatTo might have different x, y, p metrics
        return Clone(transform, subsetToReformatToWithCurrentMetrics);
    }

    //
    // we really need to reformat this...
    //
    StylusPointCollection * newCollection = new StylusPointCollection(subsetToReformatToWithCurrentMetrics, size());
    int additionalDataCount = subsetToReformatToWithCurrentMetrics->GetExpectedAdditionalDataCount();

    QVector<StylusPointPropertyInfo> properties
            = subsetToReformatToWithCurrentMetrics->GetStylusPointProperties();
    bool isIdentity = transform.isIdentity();

    for (int i = 0; i < size(); i++)
    {
        StylusPoint stylusPoint = (*this)[i];

        double xCoord = stylusPoint.X();
        double yCoord = stylusPoint.Y();
        float pressure = stylusPoint.GetUntruncatedPressureFactor();

        if (!isIdentity)
        {
            QPointF p(xCoord, yCoord);
            p = transform.map(p);
            xCoord = p.x();
            yCoord = p.y();
        }

        QVector<int> newData;
        if (additionalDataCount > 0)
        {
            //don't init, we'll do that below
            newData.resize(additionalDataCount);
        }

        StylusPoint newStylusPoint(xCoord, yCoord, pressure, subsetToReformatToWithCurrentMetrics, newData, false, false);

        //start at 3, skipping x, y, pressure
        for (int x = StylusPointDescription::RequiredCountOfProperties/*3*/; x < properties.size(); x++)
        {
            int value = stylusPoint.GetPropertyValue(properties[x]);
            newStylusPoint.SetPropertyValue(properties[x], value, false/*copy on write*/);
        }
        //bypass validation
        newCollection->push_back(newStylusPoint);
    }
    return newCollection;
}

/// <summary>
/// Returns this StylusPointCollection as a flat integer array in the himetric coordiate space
/// </summary>
/// <returns></returns>
QVector<int> StylusPointCollection::ToHiMetricArray()
{
    //
    // X and Y are in Avalon units, we need to convert to HIMETRIC
    //
    int lengthPerPoint = Description()->GetOutputArrayLengthPerPoint();
    QVector<int> output(lengthPerPoint * size());
    for (int i = 0, x = 0; i < size(); i++, x += lengthPerPoint)
    {
        StylusPoint stylusPoint = (*this)[i];
        output[x] = qRound(stylusPoint.X() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[x + 1] = qRound(stylusPoint.Y() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[x + 2] = stylusPoint.GetPropertyValue(StylusPointProperties::NormalPressure);

        if (lengthPerPoint > StylusPointDescription::RequiredCountOfProperties/*3*/)
        {
            QVector<int> additionalData = stylusPoint.GetAdditionalData();
            int countToCopy = lengthPerPoint - StylusPointDescription::RequiredCountOfProperties;/*3*/
            //Debug.Assert(additionalData.size() == countToCopy);

            for (int y = 0; y < countToCopy; y++)
            {
                output[x + y + 3] = additionalData[y];
            }
        }
    }
    return output;
}
/// <summary>
/// ToISFReadyArrays - Returns an array of arrays of packet values:
///
/// int[]
///     - int[x,x,x,x,x,x]
///     - int[y,y,y,y,y,y]
///     - int[p,p,p,p,p,p]
///
/// For ISF serialization
///
/// Also returns if any non-default pressures were found or the metric for
/// pressure was non-default
///
/// </summary>
void StylusPointCollection::ToISFReadyArrays(QVector<QVector<int>> & output, bool & shouldPersistPressure)
{
    //Debug.Assert(this.Count != 0, "Why are we serializing an empty StylusPointCollection???");
    //
    // X and Y are in Avalon units, we need to convert to HIMETRIC
    //

    //
    //


    int lengthPerPoint = Description()->GetOutputArrayLengthPerPoint();
    if (Description()->ButtonCount() > 0)
    {
        //don't serialize button data. See Windows OS Bugs 1413460 for details
        lengthPerPoint--;
    }

    output.resize(lengthPerPoint);
    for (int x = 0; x < lengthPerPoint; x++)
    {
        output[x] = QVector<int>(size());
    }

    //
    // we serialize pressure if
    // 1) The StylusPointPropertyInfo for pressure is not the default
    // 2) There is at least one non-default pressure value in this SPC
    //
    StylusPointPropertyInfo pressureInfo =
        Description()->GetPropertyInfo(StylusPointPropertyIds::NormalPressure);
    shouldPersistPressure =
        !StylusPointPropertyInfo::AreCompatible(pressureInfo, StylusPointPropertyInfoDefaults::NormalPressure);

    for (int b = 0; b < size(); b++)
    {
        StylusPoint stylusPoint = (*this)[b];
        output[0][b] = qRound(stylusPoint.X() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[1][b] = qRound(stylusPoint.Y() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[2][b] = stylusPoint.GetPropertyValue(StylusPointProperties::NormalPressure);
        //
        // it's not necessary to check HasDefaultPressure if
        // allDefaultPressures is already set
        //
        if (!shouldPersistPressure && !stylusPoint.HasDefaultPressure())
        {
            shouldPersistPressure = true;
        }
        if (lengthPerPoint > StylusPointDescription::RequiredCountOfProperties)
        {
            QVector<int> additionalData = stylusPoint.GetAdditionalData();
            int countToCopy = lengthPerPoint - StylusPointDescription::RequiredCountOfProperties;/*3*/
            //Debug.Assert(   Description()->ButtonCount() > 0 ?
            //                additionalData.size() -1 == countToCopy :
            //                additionalData.size() == countToCopy);

            for (int y = 0; y < countToCopy; y++)
            {
                output[y + 3][b] = additionalData[y];
            }
        }
    }
}

/// <summary>
/// helper use to consult with any listening strokes if it is safe to go to zero count
/// </summary>
/// <returns></returns>
bool StylusPointCollection::CanGoToZero()
{
    //if (null == this.CountGoingToZero)
    {
        //
        // no one is listening
        //
        //return true;
    }

    CancelEventArgs e;
    //e.Cancel = false;

    //
    // call the listeners
    //
    emit CountGoingToZero(e);
    //Debug.Assert(e.Cancel, "This event should always be cancelled");

    return !e.Cancel();

}
