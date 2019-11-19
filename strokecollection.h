#ifndef STROKECOLLECTION_H
#define STROKECOLLECTION_H

#include <QVector>
#include <QList>
#include <QMap>
#include <QObject>

class Stroke;

class StrokeCollection : public QObject, public QList<Stroke *>
{
    Q_OBJECT
public:
    /// <summary>
    /// The string used to designate the native persistence format
    ///      for ink data (e.g. used on the clipboard)
    /// </summary>
    static constexpr char const * InkSerializedFormat = "Ink Serialized Format";

    /// <summary>Creates an empty stroke collection</summary>
    StrokeCollection();

    /// <summary>Creates a StrokeCollection based on a collection of existing strokes</summary>
    StrokeCollection(QVector<Stroke*> const & strokes);

    /// <summary>Creates a collection from ISF data in the specified stream</summary>
    /// <param name="stream">Stream of ISF data</param>
    StrokeCollection(QIODevice * stream);


    StrokeCollection(StrokeCollection const & o);

    /// <summary>Save the collection of strokes, including any custom attributes to a stream</summary>
    /// <param name="stream">The stream to save Ink Serialized Format to</param>
    /// <param name="compress">Flag if set to true the data will be compressed, which can
    /// reduce the output buffer size in exchange for slower Save performance.</param>
    virtual void Save(QIODevice * stream, bool compress);

    /// <summary>Save the collection of strokes uncompressed, including any custom attributes to a stream</summary>
    /// <param name="stream">The stream to save Ink Serialized Format to</param>
    void Save(QIODevice * stream);

    /// <summary>
    /// Internal method for getting just the byte[] out
    /// </summary>
    void SaveIsf(QIODevice * stream, bool compress);

private:
    /// <summary>
    /// Private helper to read from a stream to the end and get a byte[]
    /// </summary>
    QIODevice* GetSeekableStream(QIODevice * stream);

    /// <summary>
    /// Allows addition of objects to the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    /// <param name="propertyData"></param>
    void AddPropertyData(QUuid const & propertyDataId, QVariant propertyData);

    /// <summary>
    /// Allows removal of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    void RemovePropertyData(QUuid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    QVariant GetPropertyData(QUuid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of a Array of QUuid const &s that are contained in the EPC
    /// </summary>
    QVector<QUuid> GetPropertyDataIds();

    /// <summary>
    /// Allows the checking of objects in the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    bool ContainsPropertyData(QUuid const & propertyDataId);

    /// <summary>
    /// Applies the specified transform matrix on every stroke in the collection.
    /// This method composes this transform with the existing
    /// transform on the stroke.</summary>
    /// <param name="transformMatrix">The transform to compose against each Stroke</param>
    /// <param name="applyToStylusTip">Boolean if true the transform matrix will be applied to StylusTip</param>
    /// <remarks>The StrokeCollection does not maintain a separate transform
    /// from each Stroke object. Calling Transform on the collection will
    /// cause each individual Stroke to be modified.
    /// If the StrokesChanged event fires, the changed parameter will be a pointer to 'this'
    /// collection, so any changes made to the changed event args will affect 'this' collection.</remarks>
    void Transform(QMatrix const & transformMatrix, bool applyToStylusTip);

    /// <summary>
    /// Performs a deep copy of the StrokeCollection.
    /// </summary>
    virtual StrokeCollection * Clone();

    /// <summary>
    /// called by base class Collection&lt;T&gt; when the list is being cleared;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void ClearItems();

    /// <summary>
    /// called by base class RemoveAt or Remove methods
    /// </summary>
    void RemoveItem(int index);

    /// <summary>
    /// called by base class Insert, Add methods
    /// </summary>
    void InsertItem(int index, Stroke * stroke);

    /// <summary>
    /// called by base class set_Item method
    /// </summary>
    void SetItem(int index, Stroke * stroke);

    /// <summary>
    /// Gets the index of the stroke, or -1 if it is not found
    /// </summary>
    /// <param name="stroke">stroke</param>
    /// <returns></returns>
    int IndexOf(Stroke * stroke);

    /// <summary>
    /// Remove a set of Stroke objects to the collection
    /// </summary>
    /// <param name="strokes">The strokes to remove from the collection</param>
    /// <remarks>Changes to the collection trigger a StrokesChanged event.</remarks>
    void Remove(StrokeCollection const & strokes);

    /// <summary>
    /// Add a set of Stroke objects to the collection
    /// </summary>
    /// <param name="strokes">The strokes to add to the collection</param>
    /// <remarks>The items are added to the collection at the end of the list.
    /// If the item already exists in the collection, then the item is not added again.</remarks>
    void Add(StrokeCollection const & strokes);

    /// <summary>
    /// Replace
    /// </summary>
    /// <param name="strokeToReplace"></param>
    /// <param name="strokesToReplaceWith"></param>
    void Replace(Stroke * strokeToReplace, StrokeCollection const & strokesToReplaceWith);

    /// <summary>
    /// Replace
    /// </summary>
    /// <param name="strokesToReplace"></param>
    /// <param name="strokesToReplaceWith"></param>
    void Replace(StrokeCollection const & strokesToReplace, StrokeCollection const & strokesToReplaceWith);

    /// <summary>
    /// called by StrokeCollectionSerializer during Load, bypasses Change notification
    /// </summary>
    void AddWithoutEvent(Stroke *stroke);


    /// <summary>Collection of extended properties on this StrokeCollection</summary>
    QMap<QUuid, QVariant> ExtendedProperties()
    {
        return _extendedProperties;
    }


signals:
    /// <summary>
    /// Event that notifies listeners whenever a change occurs in the set
    /// of stroke objects contained in the collection.
    /// </summary>
    /// <value>StrokeCollectionChangedEventHandler</value>
    void StrokesChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index);

    /// <summary>
    /// Event that notifies internal listeners whenever a change occurs in the set
    /// of stroke objects contained in the collection.
    /// </summary>
    /// <value>StrokeCollectionChangedEventHandler</value>
    void StrokesChangedInternal(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index);

    /// <summary>
    /// Event that notifies listeners whenever a change occurs in the propertyData
    /// </summary>
    /// <value>PropertyDataChangedEventHandler</value>
    void PropertyDataChanged(QUuid const & propName);

    /// <summary>
    /// INotifyPropertyChanged.PropertyChanged event, explicitly implemented
    /// </summary>
    void PropertyChanged(QByteArray const & propName);

    /// <summary>
    /// INotifyCollectionChanged.CollectionChanged event, explicitly implemented
    /// </summary>
    void CollectionChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index);

protected:
    /// <summary>Method called on derived classes whenever a drawing attributes
    /// change has occurred in the stroke references in the collection</summary>
    /// <param name="e">The change information for the stroke collection</param>
    /// <remarks>StrokesChanged will not be called when drawing attributes or
    /// custom attributes are changed. Changes that trigger StrokesChanged
    /// include packets or points changing, modified tranforms, and stroke objects
    /// being added or removed from the collection.
    /// To ensure that events fire for event listeners, derived classes
    /// should call this method.</remarks>
    virtual void OnStrokesChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index);

    /// <summary>
    /// Method called on derived classes whenever a change occurs in
    /// the PropertyData.
    /// </summary>
    /// <remarks>Derived classes should call this method (their base class)
    /// to ensure that event listeners are notified</remarks>
    virtual void OnPropertyDataChanged(QUuid const & propName);

    /// <summary>
    /// Method called when a property change occurs to the StrokeCollection
    /// </summary>
    /// <param name="e">The EventArgs specifying the name of the changed property.</param>
    /// <remarks>To follow the QUuid const &elines, this method should take a PropertyChangedEventArgs
    /// instance, but every other INotifyPropertyChanged implementation follows this pattern.</remarks>
    virtual void OnPropertyChanged(QByteArray const & propName);


    /// <summary>
    /// Private helper that starts searching for stroke at index,
    /// but will loop around before reporting -1.  This is used for
    /// Stroke.Remove(StrokeCollection).  For example, if we're removing
    /// strokes, chances are they are in contiguous order.  If so, calling
    /// IndexOf to validate each stroke is O(n2).  If the strokes are in order
    /// this produces closer to O(n), if they are not in order, it is no worse
    /// </summary>
private:
    int OptimisticIndexOf(int startingIndex, Stroke * stroke);

    /// <summary>
    /// Private helper that returns an array of indexes where the specified
    /// strokes exist in this stroke collection.  Returns null if at least one is not found.
    ///
    /// The indexes are sorted from smallest to largest
    /// </summary>
    /// <returns></returns>
    QVector<int> GetStrokeIndexes(StrokeCollection const & strokes);

    // This function will invoke OnStrokesChanged method.
    //      addedStrokes    -   the collection which contains the added strokes during the previous op.
    //      removedStrokes  -   the collection which contains the removed strokes during the previous op.
    void RaiseStrokesChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index);

signals:
    // The private PropertyChanged event
    void _propertyChanged(QByteArray const & propName);

    // private CollectionChanged event raiser
    void _collectionChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index);    // Custom 'user-defined' attributes assigned to this collection

private:
    //  In v1, these were called Ink.ExtendedProperties
    QMap<QUuid, QVariant> _extendedProperties;

    /// <summary>
    /// Constants for the PropertyChanged event
    /// </summary>
    static constexpr char const * IndexerName = "Item[]";
    static constexpr char const * CountName = "Count";

    //
    // Nested types...
    //


};

#endif // STROKECOLLECTION_H
