#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/styluspoint.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Input/styluspointproperties.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"
#include "Windows/Ink/events.h"
#include "Internal/debug.h"

#ifndef INKCANVAS_CORE
#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"
#else
class StrokeCollectionSerializer
{
public:
    //#region Constants (Static Fields)
    static constexpr double AvalonToHimetricMultiplier = 2540.0 / 96.0;
    static constexpr double HimetricToAvalonMultiplier = 96.0 / 2540.0;
};
#endif

INKCANVAS_BEGIN_NAMESPACE

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
        throw std::runtime_error("initialCapacity");
    }
    SetCapacity(initialCapacity);
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
StylusPointCollection::StylusPointCollection(SharedPointer<StylusPointDescription> stylusPointDescription)
{
    if (nullptr == stylusPointDescription)
    {
        throw std::runtime_error("");
    }
    _stylusPointDescription = stylusPointDescription;
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
/// <param name="initialCapacity">initialCapacity
StylusPointCollection::StylusPointCollection(SharedPointer<StylusPointDescription> stylusPointDescription, int initialCapacity)
    : StylusPointCollection (stylusPointDescription)
{
    if (initialCapacity < 0)
    {
        throw std::runtime_error("initialCapacity");
    }
    SetCapacity(initialCapacity);
}


/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="stylusPoints">stylusPoints
StylusPointCollection::StylusPointCollection(List<StylusPoint> const & stylusPoints)
    //: this() //don't call the base ctor, we want to use the first sp
{
    List<StylusPoint> points(stylusPoints);
    if (points.Count() == 0)
    {
        throw std::runtime_error("stylusPoints");
    }

    //
    // set our packet description to the first in the array
    //
    _stylusPointDescription = points[0].Description();

    SetCapacity(points.Count());
    for (int x = 0; x < points.Count(); x++)
    {
        Add(points[x]);
    }
}

/// <summary>
/// StylusPointCollection
/// </summary>
/// <param name="points">points
StylusPointCollection::StylusPointCollection(Array<Point> const & points)
    : StylusPointCollection()
{
    List<StylusPoint> stylusPoints;
    for (Point point : points)
    {
        //this can throw (since point.X or Y can be beyond our range)
        //don't add to our collection until after we instance
        //all of the styluspoints and we know the ranges are valid
        stylusPoints.Add(StylusPoint(point));
    }

    if (stylusPoints.Count() == 0)
    {
        throw std::runtime_error("points");
    }

    SetCapacity(stylusPoints.Count());
    Items().AddRange(stylusPoints);
}

/// <summary>
/// ctor called by input with a raw int[]
/// </summary>
/// <param name="stylusPointDescription">stylusPointDescription
/// <param name="rawPacketData">rawPacketData
/// <param name="tabletToView">tabletToView
/// <param name="tabletToViewMatrix">tabletToView
StylusPointCollection::StylusPointCollection(SharedPointer<StylusPointDescription> stylusPointDescription, Array<int> rawPacketData, Matrix const & tabletToView, Matrix const & tabletToViewMatrix)
{
    if (nullptr == stylusPointDescription)
    {
        throw std::runtime_error("");
    }
    _stylusPointDescription = stylusPointDescription;

    int lengthPerPoint = stylusPointDescription->GetInputArrayLengthPerPoint();
    int logicalPointCount = rawPacketData.Length() / lengthPerPoint;
    Debug::Assert(0 == rawPacketData.Length() % lengthPerPoint, "Invalid assumption about packet length, there shouldn't be any remainder");

    //
    // set our capacity and validate
    //
    SetCapacity(logicalPointCount);
    for (int count = 0, i = 0; count < logicalPointCount; count++, i += lengthPerPoint)
    {

        //first, determine the x, y values by xf-ing them
        Point p(rawPacketData[i], rawPacketData[i + 1]);
        (void) tabletToView;
        //if (tabletToView != nullptr)
        //{
        //    p = tabletToView.map(p);
        //}
        //else
        {
            p = tabletToViewMatrix.Transform(p);
        }

        int startIndex = 2;
        bool containsTruePressure = stylusPointDescription->ContainsTruePressure();
        if (containsTruePressure)
        {
            //don't copy pressure in the int[] for extra data
            startIndex++;
        }

        Array<int> data;
        int dataLength = lengthPerPoint - startIndex;
        if (dataLength > 0)
        {
            //copy the rest of the data
            data.resize(dataLength);
            for (int localIndex = 0, rawArrayIndex = i + startIndex; localIndex < data.Length(); localIndex++, rawArrayIndex++)
            {
                data[localIndex] = rawPacketData[rawArrayIndex];
            }
        }

        StylusPoint newPoint(p.X(), p.Y(), StylusPoint::DefaultPressure, _stylusPointDescription, data, false, false);
        if (containsTruePressure)
        {
            //use the algoritm to set pressure in StylusPoint
            int pressure = rawPacketData[i + 2];
            newPoint.SetPropertyValue(StylusPointProperties::NormalPressure, pressure);
        }

        //this does not go through our virtuals
        Items().Add(newPoint);
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
        throw std::runtime_error("stylusPoints");
    }

    // cache count outside of the loop, so if this SPC is ever passed
    // we don't loop forever
    int count = stylusPoints.Count();
    for (int x = 0; x < count; x++)
    {
        StylusPoint stylusPoint = stylusPoints[x];
        stylusPoint.SetDescription(_stylusPointDescription);
        //this does not go through our virtuals
        this->Items().Add(stylusPoint);
    }

    if (stylusPoints.Count() > 0)
    {
        OnChanged();
    }
}

/// <summary>
/// Read only access to the StylusPointDescription shared by the StylusPoints in this collection
/// </summary>
SharedPointer<StylusPointDescription> StylusPointCollection::Description()
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
        Collection::ClearItems();
        OnChanged();
    }
    else
    {
        throw std::runtime_error("");
    }
}

/// <summary>
/// called by base class Collection<T> when an item is removed from list;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::RemoveItem(int index)
{
    if (this->Count() > 1 || CanGoToZero())
    {
        this->Items().RemoveAt(index);
        OnChanged();
    }
    else
    {
        throw std::runtime_error("");
    }

}

/// <summary>
/// called by base class Collection<T> when an item is added to list;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::InsertItem(int index, StylusPoint const & stylusPoint)
{
    if (!StylusPointDescription::AreCompatible(stylusPoint.Description(),
                                            _stylusPointDescription))
    {
        throw std::runtime_error("stylusPoint");
    }

    //stylusPoint.SetDescription(_stylusPointDescription);
    Collection<StylusPoint>::InsertItem(index, stylusPoint);
    at(index).SetDescription(_stylusPointDescription);

    OnChanged();
}

/// <summary>
/// called by base class Collection<T> when an item is set in list;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StylusPointCollection::SetItem(int index, StylusPoint const & stylusPoint)
{
    if (!StylusPointDescription::AreCompatible(stylusPoint.Description(),
                                            _stylusPointDescription))
    {
        throw std::runtime_error("stylusPoint");
    }

    //stylusPoint.SetDescription(_stylusPointDescription);
    at(index) = stylusPoint;
    at(index).SetDescription(_stylusPointDescription);

    OnChanged();
}

/// <summary>
/// Clone
/// </summary>
SharedPointer<StylusPointCollection> StylusPointCollection::Clone()
{
    return Clone(Matrix(), Description(), Count());
}

/// <summary>
/// Explicit cast converter between StylusPointCollection and Point[]
/// </summary>
/// <param name="stylusPoints">stylusPoints
StylusPointCollection::operator Array<Point>()
{
    Array<Point> points(Count());
    for (int i = 0; i < Count(); i++)
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
SharedPointer<StylusPointCollection> StylusPointCollection::Clone(int count)
{
    if (count > Count() || count < 1)
    {
        throw std::runtime_error("count");
    }

    return Clone(Matrix(), Description(), count);

}

/// <summary>
/// Clone with a transform, used by input
/// </summary>
SharedPointer<StylusPointCollection> StylusPointCollection::Clone(Matrix const & transform, SharedPointer<StylusPointDescription> descriptionToUse)
{
    return Clone(transform, descriptionToUse, Count());
}


/// <summary>
/// clone implementation
/// </summary>
SharedPointer<StylusPointCollection> StylusPointCollection::Clone(Matrix const & transform, SharedPointer<StylusPointDescription> descriptionToUse, int count)
{
    Debug::Assert(count <= Count());
    //
    // We don't need to copy our _stylusPointDescription because it is immutable
    // and we don't need to copy our StylusPoints, because they are structs.
    //
    SharedPointer<StylusPointCollection> newCollection(new StylusPointCollection(descriptionToUse, count));

    bool isIdentity = transform.IsIdentity();
    for (int x = 0; x < count; x++)
    {
        if (isIdentity)
        {
            newCollection->Items().Add((*this)[x]);
        }
        else
        {
            Point point;
            StylusPoint stylusPoint = (*this)[x];
            point = stylusPoint;
            point = transform.Transform(point);
            stylusPoint = point;
            newCollection->Items().Add(stylusPoint);
        }
    }
    return newCollection;
}

/// <summary>
/// virtual for raising changed notification
/// </summary>
/// <param name="e">
void StylusPointCollection::OnChanged()
{
#ifdef INKCANVAS_QT_SIGNALS
    emit Changed();
#endif
}

/// <summary>
/// Transform the StylusPoints in this collection by the specified transform
/// </summary>
/// <param name="transform">transform
void StylusPointCollection::Transform(Matrix const & transform)
{
    Point point;
    for (int i = 0; i < Count(); i++)
    {
        StylusPoint & stylusPoint = Items()[i];
        point = stylusPoint;
        point = transform.Transform(point);
        stylusPoint.SetX(point.X());
        stylusPoint.SetY(point.Y());

        //this does not go through our virtuals
        //at(i) = stylusPoint;
    }

    if (Count() > 0)
    {
        OnChanged();
    }
}

/// <summary>
/// Reformat
/// </summary>
/// <param name="subsetToReformatTo">subsetToReformatTo
SharedPointer<StylusPointCollection> StylusPointCollection::Reformat(SharedPointer<StylusPointDescription> subsetToReformatTo)
{
    return Reformat(subsetToReformatTo, Matrix());
}

/// <summary>
/// Helper that transforms and scales in one go
/// </summary>
SharedPointer<StylusPointCollection> StylusPointCollection::Reformat(SharedPointer<StylusPointDescription> subsetToReformatTo, Matrix const & transform)
{
    if (!subsetToReformatTo->IsSubsetOf(Description()))
    {
        throw std::runtime_error("subsetToReformatTo");
    }

    SharedPointer<StylusPointDescription> subsetToReformatToWithCurrentMetrics =
        StylusPointDescription::GetCommonDescription(subsetToReformatTo,
                                                    Description()); //preserve metrics from this spd

    if (StylusPointDescription::AreCompatible(Description(), subsetToReformatToWithCurrentMetrics) &&
        transform.IsIdentity())
    {
        //subsetToReformatTo might have different x, y, p metrics
        return Clone(transform, subsetToReformatToWithCurrentMetrics);
    }

    //
    // we really need to reformat this...
    //
    SharedPointer<StylusPointCollection> newCollection(new StylusPointCollection(subsetToReformatToWithCurrentMetrics, Count()));
    int additionalDataCount = subsetToReformatToWithCurrentMetrics->GetExpectedAdditionalDataCount();

    List<StylusPointPropertyInfo> properties
            = subsetToReformatToWithCurrentMetrics->GetStylusPointProperties();
    bool isIdentity = transform.IsIdentity();

    for (int i = 0; i < Count(); i++)
    {
        StylusPoint stylusPoint = (*this)[i];

        double xCoord = stylusPoint.X();
        double yCoord = stylusPoint.Y();
        float pressure = stylusPoint.GetUntruncatedPressureFactor();

        if (!isIdentity)
        {
            Point p(xCoord, yCoord);
            p = transform.Transform(p);
            xCoord = p.X();
            yCoord = p.Y();
        }

        Array<int> newData;
        if (additionalDataCount > 0)
        {
            //don't init, we'll do that below
            newData.resize(additionalDataCount);
        }

        StylusPoint newStylusPoint(xCoord, yCoord, pressure, subsetToReformatToWithCurrentMetrics, newData, false, false);

        //start at 3, skipping x, y, pressure
        for (int x = StylusPointDescription::RequiredCountOfProperties/*3*/; x < properties.Count(); x++)
        {
            int value = stylusPoint.GetPropertyValue(properties[x]);
            newStylusPoint.SetPropertyValue(properties[x], value, false/*copy on write*/);
        }
        //bypass validation
        newCollection->Items().Add(newStylusPoint);
    }
    return newCollection;
}

/// <summary>
/// Returns this StylusPointCollection as a flat integer array in the himetric coordiate space
/// </summary>
/// <returns></returns>
Array<int> StylusPointCollection::ToHiMetricArray()
{
    //
    // X and Y are in Avalon units, we need to convert to HIMETRIC
    //
    int lengthPerPoint = Description()->GetOutputArrayLengthPerPoint();
    Array<int> output(lengthPerPoint * Count());
    for (int i = 0, x = 0; i < Count(); i++, x += lengthPerPoint)
    {
        StylusPoint stylusPoint = (*this)[i];
        output[x] = Math::Round(stylusPoint.X() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[x + 1] = Math::Round(stylusPoint.Y() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[x + 2] = stylusPoint.GetPropertyValue(StylusPointProperties::NormalPressure);

        if (lengthPerPoint > StylusPointDescription::RequiredCountOfProperties/*3*/)
        {
            List<int> additionalData = stylusPoint.GetAdditionalData();
            int countToCopy = lengthPerPoint - StylusPointDescription::RequiredCountOfProperties;/*3*/
            Debug::Assert(additionalData.Count() == countToCopy);

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
void StylusPointCollection::ToISFReadyArrays(Array<Array<int>> & output, bool & shouldPersistPressure)
{
    Debug::Assert(Count() != 0, "Why are we serializing an empty StylusPointCollection???");
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
        output[x] = Array<int>(Count());
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

    for (int b = 0; b < Count(); b++)
    {
        StylusPoint stylusPoint = (*this)[b];
        output[0][b] = Math::Round(stylusPoint.X() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
        output[1][b] = Math::Round(stylusPoint.Y() * StrokeCollectionSerializer::AvalonToHimetricMultiplier);
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
            List<int> additionalData = stylusPoint.GetAdditionalData();
            int countToCopy = lengthPerPoint - StylusPointDescription::RequiredCountOfProperties;/*3*/
            Debug::Assert(   Description()->ButtonCount() > 0 ?
                            additionalData.Count() -1 == countToCopy :
                            additionalData.Count() == countToCopy);

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

#ifdef INKCANVAS_QT_SIGNALS
    //
    // call the listeners
    //
    emit CountGoingToZero(e);
    Debug::Assert(e.Cancel(), "This event should always be cancelled");
#endif
    return !e.Cancel();

}

INKCANVAS_END_NAMESPACE
