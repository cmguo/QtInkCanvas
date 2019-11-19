#include "strokecollection.h"
#include "drawingattributes.h"
#include "stroke.h"

#include <QIODevice>
#include <QBuffer>
#include <QMatrix>

StrokeCollection::StrokeCollection()
{
}

/// <summary>Creates a StrokeCollection based on a collection of existing strokes</summary>
StrokeCollection::StrokeCollection(QVector<Stroke*> const & strokes)
{
    //unfortunately we have to check for dupes with this ctor
    for ( Stroke * stroke : strokes )
    {
        if ( QList<Stroke *>::contains(stroke) )
        {
            //clear and throw
            clear();
            throw std::exception("strokes");
        }
        push_back(stroke);
    }
}

/// <summary>Creates a collection from ISF data in the specified stream</summary>
/// <param name="stream">Stream of ISF data</param>
StrokeCollection::StrokeCollection(QIODevice * stream)
{
    if ( stream == nullptr )
    {
        throw std::exception("stream");
    }
    if ( !stream->isReadable() )
    {
        throw std::exception("stream");
    }

    QIODevice * seekableStream = GetSeekableStream(stream);
    if (seekableStream == nullptr)
    {
        throw std::exception("stream");
    }

    //this will init our stroke collection
    //StrokeCollectionSerializer serializer(this);
    //serializer.DecodeISF(stream);
}


/// <summary>Save the collection of strokes, including any custom attributes to a stream</summary>
/// <param name="stream">The stream to save Ink Serialized Format to</param>
/// <param name="compress">Flag if set to true the data will be compressed, which can
/// reduce the output buffer size in exchange for slower Save performance.</param>
void StrokeCollection::Save(QIODevice * stream, bool compress)
{
    if ( stream == nullptr )
    {
        throw std::exception("stream");
    }
    if ( !stream->isWritable() )
    {
        throw std::exception("stream");
    }
    SaveIsf(stream, compress);
}

/// <summary>Save the collection of strokes uncompressed, including any custom attributes to a stream</summary>
/// <param name="stream">The stream to save Ink Serialized Format to</param>
void StrokeCollection::Save(QIODevice * stream)
{
    Save(stream, true);
}

/// <summary>
/// Internal method for getting just the byte[] out
/// </summary>
void StrokeCollection::SaveIsf(QIODevice * stream, bool compress)
{
    //StrokeCollectionSerializer serializer(this);
    //serializer.CurrentCompressionMode = compress ? CompressionMode.Compressed : CompressionMode.NoCompression;
    //serializer.EncodeISF(stream);
}

/// <summary>
/// Private helper to read from a stream to the end and get a byte[]
/// </summary>
QIODevice* StrokeCollection::GetSeekableStream(QIODevice * stream)
{
    //Debug.Assert(stream != null);
    //Debug.Assert(stream.CanRead);
    if ( stream->seek(0) )
    {
        int bytesToRead = (int)( stream->size() - stream->pos() );
        if ( bytesToRead < 1 )
        {
            return nullptr; //nothing to read
        }
        //we can write to this
        return stream;
    }
    else
    {
        //Not all Stream implementations support Length.  Streams derived from
        //NetworkStream and CryptoStream are the primary examples, but there are others
        //theyll throw NotSupportedException
        QBuffer * ms = new QBuffer();

        int64_t bufferSize = 4096;
        char* buffer = new char[bufferSize];
        int64_t count = bufferSize;
        while ( count == bufferSize )
        {
            count = stream->read(buffer, bufferSize);
            ms->write(buffer, count);
        }
        ms->seek(0);
        return ms;
    }
}

/// <summary>
/// Allows addition of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
/// <param name="propertyData"></param>
void StrokeCollection::AddPropertyData(QUuid const & propertyDataId, QVariant propertyData)
{
    DrawingAttributes::ValidateStylusTipTransform(propertyDataId, propertyData);
    QVariant oldValue;
    if ( ContainsPropertyData(propertyDataId) )
    {
        oldValue = GetPropertyData(propertyDataId);
        _extendedProperties[propertyDataId] = propertyData;
    }
    else
    {
        _extendedProperties.insert(propertyDataId, propertyData);
    }
    // fire notification
    OnPropertyDataChanged(propertyDataId);
}

/// <summary>
/// Allows removal of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
void StrokeCollection::RemovePropertyData(QUuid const & propertyDataId)
{
    QVariant propertyData = GetPropertyData(propertyDataId);
    _extendedProperties.remove(propertyDataId);
    // fire notification
    OnPropertyDataChanged(propertyDataId);
}

/// <summary>
/// Allows retrieval of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
QVariant StrokeCollection::GetPropertyData(QUuid const & propertyDataId)
{
    if ( propertyDataId == QUuid() )
    {
        throw std::exception("propertyDataId");
    }

    return _extendedProperties[propertyDataId];
}

/// <summary>
/// Allows retrieval of a Array of QUuid const &s that are contained in the EPC
/// </summary>
QVector<QUuid> StrokeCollection::GetPropertyDataIds()
{
    return _extendedProperties.keys().toVector();
}

/// <summary>
/// Allows the checking of objects in the EPC
/// </summary>
/// <param name="propertyDataId"></param>
bool StrokeCollection::ContainsPropertyData(QUuid const & propertyDataId)
{
    return _extendedProperties.contains(propertyDataId);
}

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
void StrokeCollection::Transform(QMatrix const & transformMatrix, bool applyToStylusTip)
{
    // Ensure that the transformMatrix is invertible.
    if ( false == transformMatrix.isInvertible() )
        throw std::exception("transformMatrix");

    // if transformMatrix is identity or the StrokeCollection is empty
    //      then no change will occur anyway
    if ( transformMatrix.isIdentity() || size() == 0 )
    {
        return;
    }

    // Apply the transform to each strokes
    for ( Stroke * stroke : (*this) )
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
        stroke->Transform(transformMatrix, applyToStylusTip);
    }
}

/// <summary>
/// Performs a deep copy of the StrokeCollection.
/// </summary>
StrokeCollection * StrokeCollection::Clone()
{
    StrokeCollection * clone = new StrokeCollection();
    for ( Stroke * s : (*this) )
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that s could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
        clone->append(s->Clone());
    }

    //
    // clone epc if we have them
    //
    //if ( _extendedProperties )
    {
        clone->_extendedProperties = _extendedProperties;
    }
    return clone;
}

/// <summary>
/// called by base class Collection&lt;T&gt; when the list is being cleared;
/// raises a CollectionChanged event to any listeners
/// </summary>
void StrokeCollection::ClearItems()
{
    if ( size() > 0 )
    {
        StrokeCollection removed;
        for ( int x = 0; x < size(); x++ )
        {
            removed.append(this[x]);
        }

        clear();

        RaiseStrokesChanged(StrokeCollection() /*added*/, removed, -1);
    }
}

/// <summary>
/// called by base class RemoveAt or Remove methods
/// </summary>
void StrokeCollection::RemoveItem(int index)
{
    Stroke* removedStroke = (*this)[index];
    removeAt(index);

    StrokeCollection removed;
    removed.append(removedStroke);
    RaiseStrokesChanged(StrokeCollection() /*added*/, removed, index);
}

/// <summary>
/// called by base class Insert, Add methods
/// </summary>
void StrokeCollection::InsertItem(int index, Stroke * stroke)
{
    if ( stroke == nullptr )
    {
        throw std::exception("stroke");
    }
    if ( IndexOf(stroke) != -1 )
    {
        throw std::exception("stroke");
    }

    insert(index, stroke);

    StrokeCollection addedStrokes;
    addedStrokes.append(stroke);
    RaiseStrokesChanged(addedStrokes, StrokeCollection() /*removed*/, index);
}

/// <summary>
/// called by base class set_Item method
/// </summary>
void StrokeCollection::SetItem(int index, Stroke * stroke)
{
    if ( stroke == nullptr )
    {
        throw std::exception("stroke");
    }
    if ( IndexOf(stroke) != -1 )
    {
        throw std::exception("stroke");
    }

    Stroke* removedStroke = (*this)[index];
    (*this)[index] = stroke;

    StrokeCollection removed;
    removed.append(removedStroke);

    StrokeCollection added;
    added.append(stroke);
    RaiseStrokesChanged(added, removed, index);
}

/// <summary>
/// Gets the index of the stroke, or -1 if it is not found
/// </summary>
/// <param name="stroke">stroke</param>
/// <returns></returns>
int StrokeCollection::IndexOf(Stroke * stroke)
{
    if (stroke == nullptr)
    {
        //we never allow null strokes
        return -1;
    }
    for (int i = 0; i < size(); i++)
    {
        if (at(i) == stroke)
        {
            return i;
        }
    }
    return -1;
}

/// <summary>
/// Remove a set of Stroke objects to the collection
/// </summary>
/// <param name="strokes">The strokes to remove from the collection</param>
/// <remarks>Changes to the collection trigger a StrokesChanged event.</remarks>
void StrokeCollection::Remove(StrokeCollection const & strokes)
{
    if ( strokes.size() == 0 )
    {
        // NOTICE-2004/06/08-WAYNEZEN:
        // We don't throw if an empty collection is going to be removed. And there is no event either.
        // This rule is also applied to invoking Clear() with an empty StrokeCollection.
        return;
    }

    QVector<int> indexes = GetStrokeIndexes(strokes);


    for ( int x = indexes.size() - 1; x >= 0; x-- )
    {
        //bypass this.RemoveAt, which calls changed events
        //and call our protected List<Stroke> directly
        //remove from the back so the indexes are correct
        removeAt(indexes[x]);
    }

    RaiseStrokesChanged(StrokeCollection() /*added*/, strokes, indexes[0]);
}

/// <summary>
/// Add a set of Stroke objects to the collection
/// </summary>
/// <param name="strokes">The strokes to add to the collection</param>
/// <remarks>The items are added to the collection at the end of the list.
/// If the item already exists in the collection, then the item is not added again.</remarks>
void StrokeCollection::Add(StrokeCollection const & strokes)
{
    if ( strokes.size() == 0 )
    {
        // NOTICE-2004/06/08-WAYNEZEN:
        // We don't throw if an empty collection is going to be added. And there is no event either.
        return;
    }

    int index = size();

    //validate that none of the strokes exist in the collection
    for ( int x = 0; x < strokes.size(); x++ )
    {
        Stroke* stroke = strokes[x];
        if ( indexOf(stroke) != -1 )
        {
            throw std::exception("strokes");
        }
    }

    //add the strokes
    //bypass this.AddRange, which calls changed events
    //and call our protected List<Stroke> directly
    append(strokes);

    RaiseStrokesChanged(strokes, StrokeCollection() /*removed*/, index);
}

/// <summary>
/// Replace
/// </summary>
/// <param name="strokeToReplace"></param>
/// <param name="strokesToReplaceWith"></param>
void StrokeCollection::Replace(Stroke * strokeToReplace, StrokeCollection const & strokesToReplaceWith)
{
    if ( strokeToReplace == nullptr )
    {
        throw std::exception("SR.Get(SRID.EmptyScToReplace)");
    }

    StrokeCollection strokesToReplace;
    strokesToReplace.append(strokeToReplace);
    Replace(strokesToReplace, strokesToReplaceWith);
}

/// <summary>
/// Replace
/// </summary>
/// <param name="strokesToReplace"></param>
/// <param name="strokesToReplaceWith"></param>
void StrokeCollection::Replace(StrokeCollection const & strokesToReplace, StrokeCollection const & strokesToReplaceWith)
{
    int replaceCount = strokesToReplace.size();
    if ( replaceCount == 0 )
    {
        throw std::exception("strokesToReplace");
    }

    QVector<int> indexes = GetStrokeIndexes(strokesToReplace);

    //validate that none of the relplaceWith strokes exist in the collection
    for ( int x = 0; x < strokesToReplaceWith.size(); x++ )
    {
        Stroke* stroke = strokesToReplaceWith[x];
        if ( IndexOf(stroke) != -1 )
        {
            throw std::exception("strokesToReplaceWith");
        }
    }

    //bypass this.RemoveAt / InsertRange, which calls changed events
    //and call our protected List<Stroke> directly
    for ( int x = indexes.size() - 1; x >= 0; x-- )
    {
        //bypass this.RemoveAt, which calls changed events
        //and call our protected List<Stroke> directly
        //remove from the back so the indexes are correct
        removeAt(indexes[x]);
    }

    if ( strokesToReplaceWith.size() > 0 )
    {
        //insert at the
        for (Stroke * stroke : strokesToReplaceWith)
        insert(++indexes[0], stroke);
    }


    RaiseStrokesChanged(strokesToReplaceWith, strokesToReplace, indexes[0]);
}

/// <summary>
/// called by StrokeCollectionSerializer during Load, bypasses Change notification
/// </summary>
void StrokeCollection::AddWithoutEvent(Stroke *stroke)
{
    //Debug.Assert(stroke != null && IndexOf(stroke) == -1);
    push_back(stroke);
}


/// <summary>Method called on derived classes whenever a drawing attributes
/// change has occurred in the stroke references in the collection</summary>
/// <param name="e">The change information for the stroke collection</param>
/// <remarks>StrokesChanged will not be called when drawing attributes or
/// custom attributes are changed. Changes that trigger StrokesChanged
/// include packets or points changing, modified tranforms, and stroke objects
/// being added or removed from the collection.
/// To ensure that events fire for event listeners, derived classes
/// should call this method.</remarks>
void StrokeCollection::OnStrokesChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index)
{
    //raise our internal event first.  This is used by
    //our Renderer and IncrementalHitTester since if they can assume
    //they are the first in the delegate chain, they can be optimized
    //to not have to handle out of order events caused by 3rd party code
    //getting called first
    //if ( this.StrokesChangedInternal != null)
    {
        emit StrokesChangedInternal(addedStrokes, removedStrokes, index);
    }
    //if ( this.StrokesChanged != null )
    {
        emit StrokesChanged(addedStrokes, removedStrokes, index);
    }
    //if ( _collectionChanged != null )
    {
        //raise CollectionChanged.  We support the following
        //NotifyCollectionChangedActions
        //NotifyCollectionChangedEventArgs args = null;
        if ( size() == 0 )
        {
            //Reset
            //Debug.Assert(e.Removed.Count > 0);
            _collectionChanged(StrokeCollection(), removedStrokes, -1);
        }
        else if ( addedStrokes.size() == 0 )
        {
            //Remove
            _collectionChanged(StrokeCollection(), removedStrokes, index);
        }
        else if ( removedStrokes.size() == 0 )
        {
            //Add
            _collectionChanged(addedStrokes, StrokeCollection(), index);
        }
        else
        {
            //Replace
            _collectionChanged(addedStrokes, removedStrokes, index);
        }
    }
}


/// <summary>
/// Method called on derived classes whenever a change occurs in
/// the PropertyData.
/// </summary>
/// <remarks>Derived classes should call this method (their base class)
/// to ensure that event listeners are notified</remarks>
void StrokeCollection::OnPropertyDataChanged(QUuid const & propName)
{
    //if ( this.PropertyDataChanged != null )
    {
        emit PropertyDataChanged(propName);
    }
}

/// <summary>
/// Method called when a property change occurs to the StrokeCollection
/// </summary>
/// <param name="e">The EventArgs specifying the name of the changed property.</param>
/// <remarks>To follow the QUuid const &elines, this method should take a PropertyChangedEventArgs
/// instance, but every other INotifyPropertyChanged implementation follows this pattern.</remarks>
void StrokeCollection::OnPropertyChanged(QByteArray const & propName)
{
    //if ( _propertyChanged != null )
    {
        _propertyChanged(propName);
    }
}


/// <summary>
/// Private helper that starts searching for stroke at index,
/// but will loop around before reporting -1.  This is used for
/// Stroke.Remove(StrokeCollection).  For example, if we're removing
/// strokes, chances are they are in contiguous order.  If so, calling
/// IndexOf to validate each stroke is O(n2).  If the strokes are in order
/// this produces closer to O(n), if they are not in order, it is no worse
/// </summary>
int StrokeCollection::OptimisticIndexOf(int startingIndex, Stroke *stroke)
{
    //Debug.Assert(startingIndex >= 0);
    for ( int x = startingIndex; x < size(); x++ )
    {
        if ( at(x) == stroke )
        {
            return x;
        }
    }

    //we didn't find anything on the first pass, now search the beginning
    for ( int x = 0; x < startingIndex; x++ )
    {
        if ( at(x) == stroke )
        {
            return x;
        }
    }
    return -1;
}

/// <summary>
/// Private helper that returns an array of indexes where the specified
/// strokes exist in this stroke collection.  Returns null if at least one is not found.
///
/// The indexes are sorted from smallest to largest
/// </summary>
/// <returns></returns>
QVector<int> StrokeCollection::GetStrokeIndexes(StrokeCollection const & strokes)
{
    //to keep from walking the StrokeCollection twice for each stroke, we will maintain an index of
    //strokes to remove as we go
    QVector<int> indexes(strokes.size());
    for ( int x = 0; x < indexes.size(); x++ )
    {
        indexes[x] = INT_MAX;
    }

    int currentIndex = 0;
    int highestIndex = -1;
    int usedIndexCount = 0;
    for ( int x = 0; x < strokes.size(); x++ )
    {
        currentIndex = OptimisticIndexOf(currentIndex, strokes[x]);
        if ( currentIndex == -1 )
        {
            //stroke doe3sn't exist, bail out.
            return QVector<int>();
        }

        //
        // optimize for the most common case... replace is passes strokes
        // in contiguous order.  Only do the sort if we need to
        //
        if ( currentIndex > highestIndex )
        {
            //write current to the next available slot
            indexes[usedIndexCount++] = currentIndex;
            highestIndex = currentIndex;
            continue;
        }

        //keep in sorted order (smallest to largest) with a simple insertion sort
        for ( int y = 0; y < indexes.size(); y++ )
        {
            if ( currentIndex < indexes[y] )
            {
                if ( indexes[y] != INT_MAX )
                {
                    //shift from the end
                    for ( int i = indexes.size() - 1; i > y; i-- )
                    {
                        indexes[i] = indexes[i - 1];
                    }
                }
                indexes[y] = currentIndex;
                usedIndexCount++;

                if ( currentIndex > highestIndex )
                {
                    highestIndex = currentIndex;
                }
                break;
            }
        }
    }

    return indexes;
}

// This function will invoke OnStrokesChanged method.
//      addedStrokes    -   the collection which contains the added strokes during the previous op.
//      removedStrokes  -   the collection which contains the removed strokes during the previous op.
void StrokeCollection::RaiseStrokesChanged(StrokeCollection const & addedStrokes, StrokeCollection const & removedStrokes, int index)
{
    // Invoke OnPropertyChanged
    OnPropertyChanged(CountName);
    OnPropertyChanged(IndexerName);

    // Invoke OnStrokesChanged which will fire the StrokesChanged event AND the CollectionChanged event.
    OnStrokesChanged(addedStrokes, removedStrokes, index);
}
