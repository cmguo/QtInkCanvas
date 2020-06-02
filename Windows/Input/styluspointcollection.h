#ifndef STYLUSPOINTCOLLECTION_H
#define STYLUSPOINTCOLLECTION_H

#include "InkCanvas_global.h"
#include "Collections/ObjectModel/collection.h"
#include "Windows/Input/styluspoint.h"
#include "Collections/Generic/list.h"
#include "Windows/Media/matrix.h"
#include "sharedptr.h"

#ifndef INKCANVAS_CORE
#include <QObject>
#endif


INKCANVAS_BEGIN_NAMESPACE

class StylusPointDescription;
class CancelEventArgs;

// namespace System.Windows.Input

#ifndef INKCANVAS_CORE
class INKCANVAS_EXPORT StylusPointCollection : public QObject, public Collection<StylusPoint>
{
    Q_OBJECT
signals:
    /// <summary>
    /// Changed event, anytime the data in this collection changes, this event is raised
    /// </summary>
    void Changed();

    /// <summary>
    /// Internal only changed event used by Stroke to prevent zero count strokes
    /// </summary>
    void CountGoingToZero(CancelEventArgs & e);

#else
class INKCANVAS_EXPORT StylusPointCollection : public Collection<StylusPoint>
{
#endif
public:
    StylusPointCollection();

    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="initialCapacity">initialCapacity
    StylusPointCollection(int initialCapacity);

    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    StylusPointCollection(SharedPointer<StylusPointDescription> stylusPointDescription);

    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="initialCapacity">initialCapacity
    StylusPointCollection(SharedPointer<StylusPointDescription> stylusPointDescription, int initialCapacity);


    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="stylusPoints">stylusPoints
    StylusPointCollection(List<StylusPoint> const & stylusPoints);

    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="points">points
    StylusPointCollection(Array<Point> const & points);

    /// <summary>
    /// ctor called by input with a raw int[]
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="rawPacketData">rawPacketData
    /// <param name="tabletToView">tabletToView
    /// <param name="tabletToViewMatrix">tabletToView
    StylusPointCollection(SharedPointer<StylusPointDescription> stylusPointDescription, Array<int> rawPacketData,
                          Matrix const & tabletToView, Matrix const & tabletToViewMatrix);

    /// <summary>
    /// Adds the StylusPoints in the StylusPointCollection to this StylusPointCollection
    /// </summary>
    /// <param name="stylusPoints">stylusPoints
    void Add(StylusPointCollection & stylusPoints);

    using Collection::Add;

    /// <summary>
    /// Read only access to the StylusPointDescription shared by the StylusPoints in this collection
    /// </summary>
    SharedPointer<StylusPointDescription> Description();

    /// <summary>
    /// called by base class Collection<T> when the list is being cleared;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void ClearItems() override;

    /// <summary>
    /// called by base class Collection<T> when an item is removed from list;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void RemoveItem(int index) override;

    /// <summary>
    /// called by base class Collection<T> when an item is added to list;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void InsertItem(int index, StylusPoint const & stylusPoint) override;

    /// <summary>
    /// called by base class Collection<T> when an item is set in list;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void SetItem(int index, StylusPoint const & stylusPoint) override;

    /// <summary>
    /// Clone
    /// </summary>
    SharedPointer<StylusPointCollection> Clone();

    /// <summary>
    /// Explicit cast converter between StylusPointCollection and Point[]
    /// </summary>
    /// <param name="stylusPoints">stylusPoints
    operator Array<Point>();

    /// <summary>
    /// Clone and truncate
    /// </summary>
    /// <param name="count">The maximum count of points to clone (used by GestureRecognizer)
    /// <returns></returns>
    SharedPointer<StylusPointCollection> Clone(int Count);

    /// <summary>
    /// Clone with a transform, used by input
    /// </summary>
    SharedPointer<StylusPointCollection> Clone(Matrix const & transform, SharedPointer<StylusPointDescription> descriptionToUse);


    /// <summary>
    /// clone implementation
    /// </summary>
    SharedPointer<StylusPointCollection> Clone(Matrix const & transform, SharedPointer<StylusPointDescription> descriptionToUse, int Count);

    /// <summary>
    /// virtual for raising changed notification
    /// </summary>
    /// <param name="e">
    virtual void OnChanged();

    /// <summary>
    /// Transform the StylusPoints in this collection by the specified transform
    /// </summary>
    /// <param name="transform">transform
    void Transform(Matrix const & transform);

    /// <summary>
    /// Reformat
    /// </summary>
    /// <param name="subsetToReformatTo">subsetToReformatTo
    SharedPointer<StylusPointCollection> Reformat(SharedPointer<StylusPointDescription> subsetToReformatTo);

    /// <summary>
    /// Helper that transforms and scales in one go
    /// </summary>
    SharedPointer<StylusPointCollection> Reformat(SharedPointer<StylusPointDescription> subsetToReformatTo, Matrix const & transform);

    /// <summary>
    /// Returns this StylusPointCollection as a flat integer array in the himetric coordiate space
    /// </summary>
    /// <returns></returns>
    Array<int> ToHiMetricArray();

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
    void ToISFReadyArrays(Array<Array<int>> & output, bool & shouldPersistPressure);

    /// <summary>
    /// helper use to consult with any listening strokes if it is safe to go to zero count
    /// </summary>
    /// <returns></returns>
    bool CanGoToZero();

private:
    SharedPointer<StylusPointDescription> _stylusPointDescription;
};

INKCANVAS_END_NAMESPACE

#endif // STYLUSPOINTCOLLECTION_H
