#ifndef STYLUSPOINTCOLLECTION_H
#define STYLUSPOINTCOLLECTION_H

#include <QSharedPointer>
#include <QList>
#include <QMatrix>
#include <QObject>

#include <iterator>
#include <vector>

class StylusPoint;
class StylusPointDescription;
class CancelEventArgs;

class StylusPointCollection : public QObject, public QList<StylusPoint>
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
    StylusPointCollection(QSharedPointer<StylusPointDescription> stylusPointDescription);

    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="initialCapacity">initialCapacity
    StylusPointCollection(QSharedPointer<StylusPointDescription> stylusPointDescription, int initialCapacity);


    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="stylusPoints">stylusPoints
    StylusPointCollection(QVector<StylusPoint> const & stylusPoints);

    /// <summary>
    /// StylusPointCollection
    /// </summary>
    /// <param name="points">points
    StylusPointCollection(QVector<QPointF> const & points);

    /// <summary>
    /// ctor called by input with a raw int[]
    /// </summary>
    /// <param name="stylusPointDescription">stylusPointDescription
    /// <param name="rawPacketData">rawPacketData
    /// <param name="tabletToView">tabletToView
    /// <param name="tabletToViewMatrix">tabletToView
    StylusPointCollection(QSharedPointer<StylusPointDescription> stylusPointDescription, QVector<int> rawPacketData,
                          QMatrix & tabletToView, QMatrix & tabletToViewMatrix);

    /// <summary>
    /// Adds the StylusPoints in the StylusPointCollection to this StylusPointCollection
    /// </summary>
    /// <param name="stylusPoints">stylusPoints
    void Add(StylusPointCollection & stylusPoints);

    /// <summary>
    /// Read only access to the StylusPointDescription shared by the StylusPoints in this collection
    /// </summary>
    QSharedPointer<StylusPointDescription> Description();

    /// <summary>
    /// called by base class Collection<T> when the list is being cleared;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void ClearItems();

    /// <summary>
    /// called by base class Collection<T> when an item is removed from list;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void RemoveItem(int index);

    /// <summary>
    /// called by base class Collection<T> when an item is added to list;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void InsertItem(int index, StylusPoint & stylusPoint);

    /// <summary>
    /// called by base class Collection<T> when an item is set in list;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void SetItem(int index, StylusPoint & stylusPoint);

    /// <summary>
    /// Clone
    /// </summary>
    StylusPointCollection * Clone();

    /// <summary>
    /// Explicit cast converter between StylusPointCollection and Point[]
    /// </summary>
    /// <param name="stylusPoints">stylusPoints
    operator QVector<QPointF>();

    /// <summary>
    /// Clone and truncate
    /// </summary>
    /// <param name="count">The maximum count of points to clone (used by GestureRecognizer)
    /// <returns></returns>
    StylusPointCollection * Clone(int count);

    /// <summary>
    /// Clone with a transform, used by input
    /// </summary>
    StylusPointCollection * Clone(QMatrix const & transform, QSharedPointer<StylusPointDescription> descriptionToUse);


    /// <summary>
    /// clone implementation
    /// </summary>
    StylusPointCollection * Clone(QMatrix const & transform, QSharedPointer<StylusPointDescription> descriptionToUse, int count);

    /// <summary>
    /// virtual for raising changed notification
    /// </summary>
    /// <param name="e">
    virtual void OnChanged();

    /// <summary>
    /// Transform the StylusPoints in this collection by the specified transform
    /// </summary>
    /// <param name="transform">transform
    void Transform(QMatrix const & transform);

    /// <summary>
    /// Reformat
    /// </summary>
    /// <param name="subsetToReformatTo">subsetToReformatTo
    StylusPointCollection * Reformat(QSharedPointer<StylusPointDescription> subsetToReformatTo);

    /// <summary>
    /// Helper that transforms and scales in one go
    /// </summary>
    StylusPointCollection * Reformat(QSharedPointer<StylusPointDescription> subsetToReformatTo, QMatrix const & transform);

    /// <summary>
    /// Returns this StylusPointCollection as a flat integer array in the himetric coordiate space
    /// </summary>
    /// <returns></returns>
    QVector<int> ToHiMetricArray();

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
    void ToISFReadyArrays(QVector<QVector<int>> & output, bool & shouldPersistPressure);

    /// <summary>
    /// helper use to consult with any listening strokes if it is safe to go to zero count
    /// </summary>
    /// <returns></returns>
    bool CanGoToZero();

private:
    QSharedPointer<StylusPointDescription> _stylusPointDescription;

public:
    static constexpr double MaxXY = 0;
    static constexpr double MinXY = 0;
};

#endif // STYLUSPOINTCOLLECTION_H
