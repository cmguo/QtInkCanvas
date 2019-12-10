#include "Windows/Ink/strokecollection.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/events.h"
#include "Windows/Ink/incrementalhittester.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Ink/stylusshape.h"
#include "Internal/Ink/lasso.h"
#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"
#include "Windows/Input/styluspoint.h"
#include "Internal/Ink/strokerenderer.h"
#include "Windows/Media/drawingcontext.h"
#include "notifycollectionchangedeventargs.h"
#include "Internal/finallyhelper.h"

#include <QIODevice>
#include <QBuffer>
#include <QMatrix>

StrokeCollection::StrokeCollection()
{
}

StrokeCollection::StrokeCollection(StrokeCollection const & o)
    : Collection<QSharedPointer<Stroke>>(o)
    , _extendedProperties(o._extendedProperties)
{

}

/// <summary>Creates a StrokeCollection based on a collection of existing strokes</summary>
StrokeCollection::StrokeCollection(QVector<QSharedPointer<Stroke>> const & strokes)
{
    //unfortunately we have to check for dupes with this ctor
    for ( QSharedPointer<Stroke> stroke : strokes )
    {
        if ( contains(stroke) )
        {
            //clear and throw
            clear();
            throw std::exception("strokes");
        }
        append(stroke);
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
    StrokeCollectionSerializer serializer(*this);
    serializer.DecodeISF(*stream);
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
    StrokeCollectionSerializer serializer(*this);
    serializer.CurrentCompressionMode = compress ? CompressionMode::Compressed : CompressionMode::NoCompression;
    serializer.EncodeISF(*stream);
}

/// <summary>
/// helper to read from a stream to the end and get a byte[]
/// </summary>
QIODevice* StrokeCollection::GetSeekableStream(QIODevice * stream)
{
    //Debug.Assert(stream != nullptr);
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
        _extendedProperties->Set(propertyDataId, propertyData);
    }
    else
    {
        _extendedProperties->Add(propertyDataId, propertyData);
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
    _extendedProperties->Remove(propertyDataId);
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

    return (*_extendedProperties)[propertyDataId];
}

/// <summary>
/// Allows retrieval of a Array of QUuid const &s that are contained in the EPC
/// </summary>
QVector<QUuid> StrokeCollection::GetPropertyDataIds()
{
    return _extendedProperties->GetGuidArray();
}

/// <summary>
/// Allows the checking of objects in the EPC
/// </summary>
/// <param name="propertyDataId"></param>
bool StrokeCollection::ContainsPropertyData(QUuid const & propertyDataId)
{
    return _extendedProperties->Contains(propertyDataId);
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
    if ( transformMatrix.isIdentity() || count() == 0 )
    {
        return;
    }

    // Apply the transform to each strokes
    for ( QSharedPointer<Stroke> stroke : (*this) )
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
QSharedPointer<StrokeCollection> StrokeCollection::Clone()
{
   QSharedPointer<StrokeCollection> clone(new StrokeCollection());
    for ( QSharedPointer<Stroke> s : (*this) )
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
    if ( count() > 0 )
    {
        QSharedPointer<StrokeCollection> removed(new StrokeCollection);
        for ( int x = 0; x < count(); x++ )
        {
            removed->append((*this)[x]);
        }

        clear();

        RaiseStrokesChanged(nullptr /*added*/, removed, -1);
    }
}

void StrokeCollection::AddItem(QSharedPointer<Stroke> stroke)
{
    InsertItem(size(), stroke);
}

/// <summary>
/// called by base class RemoveAt or Remove methods
/// </summary>
void StrokeCollection::RemoveItem(int index)
{
    QSharedPointer<Stroke> removedStroke = (*this)[index];
    removeAt(index);

    QSharedPointer<StrokeCollection> removed(new StrokeCollection);
    removed->append(removedStroke);
    RaiseStrokesChanged(nullptr /*added*/, removed, index);
}

bool StrokeCollection::RemoveItem(QSharedPointer<Stroke> stroke)
{
    int index = IndexOf(stroke);
    if (index < 0) return false;
    RemoveItem(index);
    return true;
}

/// <summary>
/// called by base class Insert, Add methods
/// </summary>
void StrokeCollection::InsertItem(int index, QSharedPointer<Stroke> stroke)
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

    QSharedPointer<StrokeCollection> addedStrokes(new StrokeCollection);
    addedStrokes->append(stroke);
    RaiseStrokesChanged(addedStrokes, nullptr /*removed*/, index);
}

/// <summary>
/// called by base class set_Item method
/// </summary>
void StrokeCollection::SetItem(int index, QSharedPointer<Stroke> stroke)
{
    if ( stroke == nullptr )
    {
        throw std::exception("stroke");
    }
    if ( IndexOf(stroke) != -1 )
    {
        throw std::exception("stroke");
    }

    QSharedPointer<Stroke> removedStroke = (*this)[index];
    at(index) = stroke;

    QSharedPointer<StrokeCollection> removed(new StrokeCollection);
    removed->append(removedStroke);

    QSharedPointer<StrokeCollection> added(new StrokeCollection);
    added->append(stroke);
    RaiseStrokesChanged(added, removed, index);
}

/// <summary>
/// Gets the index of the stroke, or -1 if it is not found
/// </summary>
/// <param name="stroke">stroke</param>
/// <returns></returns>
int StrokeCollection::IndexOf(QSharedPointer<Stroke> stroke)
{
    if (stroke == nullptr)
    {
        //we never allow null strokes
        return -1;
    }
    for (int i = 0; i < count(); i++)
    {
        if (at(i) == stroke)
        {
            return i;
        }
    }
    return -1;
}

int StrokeCollection::Count()
{
    return size();
}

/// <summary>
/// Remove a set of Stroke objects to the collection
/// </summary>
/// <param name="strokes">The strokes to remove from the collection</param>
/// <remarks>Changes to the collection trigger a StrokesChanged event.</remarks>
void StrokeCollection::Remove(QSharedPointer<StrokeCollection> strokes)
{
    if ( strokes->size() == 0 )
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

    RaiseStrokesChanged(nullptr /*added*/, strokes, indexes[0]);
}

/// <summary>
/// Add a set of Stroke objects to the collection
/// </summary>
/// <param name="strokes">The strokes to add to the collection</param>
/// <remarks>The items are added to the collection at the end of the list.
/// If the item already exists in the collection, then the item is not added again.</remarks>
void StrokeCollection::Add(QSharedPointer<StrokeCollection> strokes)
{
    if ( strokes->size() == 0 )
    {
        // NOTICE-2004/06/08-WAYNEZEN:
        // We don't throw if an empty collection is going to be added. And there is no event either.
        return;
    }

    int index = size();

    //validate that none of the strokes exist in the collection
    for ( int x = 0; x < strokes->size(); x++ )
    {
        QSharedPointer<Stroke> stroke = (*strokes)[x];
        if ( indexOf(stroke) != -1 )
        {
            throw std::exception("strokes");
        }
    }

    //add the strokes
    //bypass this.AddRange, which calls changed events
    //and call our protected List<Stroke> directly
    append(*strokes);

    RaiseStrokesChanged(strokes, nullptr /*removed*/, index);
}

/// <summary>
/// Replace
/// </summary>
/// <param name="strokeToReplace"></param>
/// <param name="strokesToReplaceWith"></param>
void StrokeCollection::Replace(QSharedPointer<Stroke> strokeToReplace, QSharedPointer<StrokeCollection> strokesToReplaceWith)
{
    if ( strokeToReplace == nullptr )
    {
        throw std::exception("SR.Get(SRID.EmptyScToReplace)");
    }

    QSharedPointer<StrokeCollection> strokesToReplace(new StrokeCollection);
    strokesToReplace->append(strokeToReplace);
    Replace(strokesToReplace, strokesToReplaceWith);
}

/// <summary>
/// Replace
/// </summary>
/// <param name="strokesToReplace"></param>
/// <param name="strokesToReplaceWith"></param>
void StrokeCollection::Replace(QSharedPointer<StrokeCollection> strokesToReplace, QSharedPointer<StrokeCollection> strokesToReplaceWith)
{
    int replaceCount = strokesToReplace->size();
    if ( replaceCount == 0 )
    {
        throw std::exception("strokesToReplace");
    }

    QVector<int> indexes = GetStrokeIndexes(strokesToReplace);
    if ( indexes.size() == 0 )
    {
        throw std::exception("strokesToReplace");
    }

    //validate that none of the relplaceWith strokes exist in the collection
    for ( int x = 0; x < strokesToReplaceWith->size(); x++ )
    {
        QSharedPointer<Stroke> stroke = (*strokesToReplaceWith)[x];
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

    if ( strokesToReplaceWith->size() > 0 )
    {
        //insert at the
        for (QSharedPointer<Stroke> stroke : *strokesToReplaceWith)
            insert(indexes[0]++, stroke);
    }


    RaiseStrokesChanged(strokesToReplaceWith, strokesToReplace, indexes[0]);
}

/// <summary>
/// called by StrokeCollectionSerializer during Load, bypasses Change notification
/// </summary>
void StrokeCollection::AddWithoutEvent(QSharedPointer<Stroke>stroke)
{
    //Debug.Assert(stroke != null && IndexOf(stroke) == -1);
    append(stroke);
}

/// <summary>Collection of extended properties on this StrokeCollection</summary>
ExtendedPropertyCollection& StrokeCollection::ExtendedProperties()
{
    if ( _extendedProperties == nullptr )
    {
        _extendedProperties = new ExtendedPropertyCollection();
    }
    return *_extendedProperties;
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
void StrokeCollection::OnStrokesChanged(StrokeCollectionChangedEventArgs& e)
{
    //raise our internal event first.  This is used by
    //our Renderer and IncrementalHitTester since if they can assume
    //they are the first in the delegate chain, they can be optimized
    //to not have to handle out of order events caused by 3rd party code
    //getting called first
    //if ( this.StrokesChangedInternal != nullptr)
    {
        emit StrokesChangedInternal(e);
    }
    //if ( this.StrokesChanged != null )
    {
        emit StrokesChanged(e);
    }
    //if ( _collectionChanged != null )
    {
        //raise CollectionChanged.  We support the following
        //NotifyCollectionChangedActions
        NotifyCollectionChangedEventArgs* args = nullptr;
        if ( size() == 0 )
        {
            //Reset
            //Debug.Assert(e.Removed.Count > 0);
            args = new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction::Reset);
        }
        else if ( e.Added()->size() == 0 )
        {
            //Remove
            args = new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction::Remove, *e.Removed(), e.Index());
        }
        else if ( e.Removed()->size() == 0 )
        {
            //Add
            args = new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction::Add, *e.Added(), e.Index());
        }
        else
        {
            //Replace
            args = new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction::Replace, *e.Added(), *e.Removed(), e.Index());
        }
        _collectionChanged(*args);
        delete  args;
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

StrokeCollection::operator QVariantList()
{
    QVariantList list;
    for (auto s : *this)
        list.append(QVariant::fromValue(s));
    return list;
}

/// <summary>
/// helper that starts searching for stroke at index,
/// but will loop around before reporting -1.  This is used for
/// Stroke.Remove(StrokeCollection).  For example, if we're removing
/// strokes, chances are they are in contiguous order.  If so, calling
/// IndexOf to validate each stroke is O(n2).  If the strokes are in order
/// this produces closer to O(n), if they are not in order, it is no worse
/// </summary>
int StrokeCollection::OptimisticIndexOf(int startingIndex, QSharedPointer<Stroke>stroke)
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
/// helper that returns an array of indexes where the specified
/// strokes exist in this stroke collection.  Returns null if at least one is not found.
///
/// The indexes are sorted from smallest to largest
/// </summary>
/// <returns></returns>
QVector<int> StrokeCollection::GetStrokeIndexes(QSharedPointer<StrokeCollection> strokes)
{
    //to keep from walking the StrokeCollection twice for each stroke, we will maintain an index of
    //strokes to remove as we go
    QVector<int> indexes(strokes->size());
    for ( int x = 0; x < indexes.size(); x++ )
    {
        indexes[x] = INT_MAX;
    }

    int currentIndex = 0;
    int highestIndex = -1;
    int usedIndexCount = 0;
    for ( int x = 0; x < strokes->size(); x++ )
    {
        currentIndex = OptimisticIndexOf(currentIndex, (*strokes)[x]);
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
void StrokeCollection::RaiseStrokesChanged(QSharedPointer<StrokeCollection> addedStrokes, QSharedPointer<StrokeCollection> removedStrokes, int index)
{
    StrokeCollectionChangedEventArgs eventArgs(addedStrokes, removedStrokes, index);

     // Invoke OnPropertyChanged
    OnPropertyChanged(CountName);
    OnPropertyChanged(IndexerName);

    // Invoke OnStrokesChanged which will fire the StrokesChanged event AND the CollectionChanged event.
    OnStrokesChanged(eventArgs);
}



/// <summary>
/// Calculates the combined bounds of all strokes in the collection
/// </summary>
/// <returns></returns>
QRectF StrokeCollection::GetBounds()
{
    QRectF bounds;
    for (QSharedPointer<Stroke> stroke : *this)
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
#pragma warning disable 1634, 1691
#pragma warning suppress 6506
        bounds |= stroke->GetBounds();
#pragma warning restore 1634, 1691
    }
    return bounds;
}

// ISSUE-2004/12/13-XIAOTU: In M8.2, the following two tap-hit APIs return the top-hit stroke,
// giving preference to non-highlighter strokes. We have decided not to treat highlighter and
// non-highlighter differently and only return the top-hit stroke. But there are two remaining
// open-issues on this:
//  1. Do we need to make these two APIs virtual, so user can treat highlighter differently if they
//     want to?
//  2. Since we are only returning the top-hit stroke, should we use Stroke as the return type?
//

/// <summary>
/// Tap-hit. Hit tests all strokes within a point, and returns a StrokeCollection for these strokes.Internally does Stroke.HitTest(Point, 1pxlRectShape).
/// </summary>
/// <returns>A StrokeCollection that either empty or contains the top hit stroke</returns>
QSharedPointer<StrokeCollection> StrokeCollection::HitTest(QPointF const & point)
{
    RectangleStylusShape stylusShape(1, 1);
    return PointHitTest(point, stylusShape);
}

/// <summary>
/// Tap-hit
/// </summary>
/// <param name="point">The central point</param>
/// <param name="diameter">The diameter value of the circle</param>
/// <returns>A StrokeCollection that either empty or contains the top hit stroke</returns>
QSharedPointer<StrokeCollection> StrokeCollection::HitTest(QPointF const & point, double diameter)
{
    if (qIsNaN(diameter) || diameter < DrawingAttributes::MinWidth || diameter > DrawingAttributes::MaxWidth)
    {
        throw std::exception("diameter");
    }
    EllipseStylusShape stylusShape(diameter, diameter);
    return PointHitTest(point, stylusShape);
}

/// <summary>
/// Hit-testing with lasso
/// </summary>
/// <param name="lassoPoints">points making the lasso</param>
/// <param name="percentageWithinLasso">the margin value to tell whether a stroke
/// is in or outside of the rect</param>
/// <returns>collection of strokes found inside the rectangle</returns>
QSharedPointer<StrokeCollection> StrokeCollection::HitTest(QVector<QPointF> const & lassoPoints, int percentageWithinLasso)
{
    // Check the input parameters
    //if (lassoPoints == nullptr)
    //{
    //    throw std::exception("lassoPoints");
    //}
    if ((percentageWithinLasso < 0) || (percentageWithinLasso > 100))
    {
        throw std::exception("percentageWithinLasso");
    }

    if (lassoPoints.size() < 3)
    {
        return QSharedPointer<StrokeCollection>();
    }

    Lasso* lasso = new SingleLoopLasso();
    lasso->AddPoints(lassoPoints);

    // Enumerate through the strokes and collect those captured by the lasso.
    QSharedPointer<StrokeCollection> lassoedStrokes(new StrokeCollection());
    for (QSharedPointer<Stroke> stroke : *this)
    {

        if (percentageWithinLasso == 0)
        {
            lassoedStrokes->append(stroke);
        }
        else
        {


            //try
            {
                StrokeInfo strokeInfo(stroke);

                QSharedPointer<StylusPointCollection> stylusPoints = strokeInfo.StylusPoints();
                double target = strokeInfo.TotalWeight() * percentageWithinLasso / 100.0 - Stroke::PercentageTolerance;

                for (int i = 0; i < stylusPoints->size(); i++)
                {
                    if (true == lasso->Contains((*stylusPoints)[i]))
                    {
                        target -= strokeInfo.GetPointWeight(i);
                        if (DoubleUtil::LessThanOrClose(target, 0))
                        {
                            lassoedStrokes->append(stroke);
                            break;
                        }
                    }
                }
            }
            //finally
            //{
            //    if (strokeInfo != nullptr)
            //    {
                    //detach from event handlers, or else we leak.
            //        strokeInfo->Detach();
            //    }
            //}
        }
    }

    // Return the resulting collection
    return lassoedStrokes;
}


/// <summary>
/// Hit-testing with rectangle
/// </summary>
/// <param name="bounds">hitting rectangle</param>
/// <param name="percentageWithinBounds">the percentage of the stroke that must be within
/// the bounds to be considered hit</param>
/// <returns>collection of strokes found inside the rectangle</returns>
QSharedPointer<StrokeCollection> StrokeCollection::HitTest(QRectF const & bounds, int percentageWithinBounds)
{
    // Check the input parameters
    if ((percentageWithinBounds < 0) || (percentageWithinBounds > 100))
    {
        throw std::exception("percentageWithinBounds");
    }
    if (bounds.isEmpty())
    {
        return QSharedPointer<StrokeCollection>();
    }

    // Enumerate thru the strokes collect those found within the rectangle.
    QSharedPointer<StrokeCollection> hits(new StrokeCollection());
    for (QSharedPointer<Stroke> stroke : *this)
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
#pragma warning disable 1634, 1691
#pragma warning suppress 6506
        if (true == stroke->HitTest(bounds, percentageWithinBounds))
        {
            hits->append(stroke);
        }
#pragma warning restore 1634, 1691
    }
    return hits;
}


/// <summary>
/// Issue: what's the return value
/// </summary>
/// <param name="path"></param>
/// <param name="stylusShape"></param>
/// <returns></returns>
QSharedPointer<StrokeCollection> StrokeCollection::HitTest(QVector<QPointF> const & path, StylusShape& stylusShape)
{
    // Check the input parameters
    //if (stylusShape == nullptr)
    //{
    //    throw std::exception("stylusShape");
    //}
    //if (path == nullptr)
    //{
    //    throw std::exception("path");
    //}
    if (path.size() == 0)
    {
        return QSharedPointer<StrokeCollection>();
    }

    // validate input
    ErasingStroke* erasingStroke = new ErasingStroke(stylusShape, path);
    QRectF erasingBounds = erasingStroke->Bounds();
    if (erasingBounds.isEmpty())
    {
        return QSharedPointer<StrokeCollection>();
    }
    QSharedPointer<StrokeCollection> hits(new StrokeCollection());
    for (QSharedPointer<Stroke> stroke : *this)
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
#pragma warning disable 1634, 1691
#pragma warning suppress 6506
        if (erasingBounds.intersects(stroke->GetBounds()) &&
            erasingStroke->HitTest(StrokeNodeIterator::GetIterator(*stroke, *stroke->GetDrawingAttributes())))
        {
            hits->append(stroke);
        }
#pragma warning restore 1634, 1691
    }

    return hits;
}

/// <summary>
/// Clips out all ink outside a given lasso
/// </summary>
/// <param name="lassoPoints">lasso</param>
void StrokeCollection::Clip(QVector<QPointF> const & lassoPoints)
{
    // Check the input parameters
    //if (lassoPoints == nullptr)
    //{
    //    throw std::exception("lassoPoints");
    //}

    int length = lassoPoints.size();
    if (length == 0)
    {
        throw std::exception("SR.Get(SRID.EmptyArray)");
    }

    if (length < 3)
    {
        //
        // if you're clipping with a point or a line with
        // two points, it doesn't matter where the line is or if it
        // intersects any of the strokes, the point or line has no region
        // so technically everything in the strokecollection
        // should be removed
        //
        clear(); //raises the appropriate events
        return;
    }

    SingleLoopLasso lasso;
    lasso.AddPoints(lassoPoints);

    for (int i = 0; i < size(); i++)
    {
        QSharedPointer<Stroke> stroke = (*this)[i];
        QSharedPointer<StrokeCollection> clipResult = stroke->Clip(stroke->HitTest(lasso));
        UpdateStrokeCollection(stroke, clipResult, i);
    }
}

/// <summary>
/// Clips out all ink outside a given rectangle.
/// </summary>
/// <param name="bounds">rectangle to clip with</param>
void StrokeCollection::Clip(QRectF const & bounds)
{
    if (bounds.isEmpty() == false)
    {
        Clip({ bounds.topLeft(), bounds.topRight(), bounds.bottomRight(), bounds.bottomLeft() });
    }
}

/// <summary>
/// Erases all ink inside a lasso
/// </summary>
/// <param name="lassoPoints">lasso to erase within</param>
void StrokeCollection::Erase(QVector<QPointF> const & lassoPoints)
{
    // Check the input parameters
    //if (lassoPoints == nullptr)
    //{
    //    throw std::exception("lassoPoints");
    //}
    int length = lassoPoints.size();
    if (length == 0)
    {
        throw std::exception("SR.Get(SRID.EmptyArray)");
    }

    if (length < 3)
    {
        return;
    }

    SingleLoopLasso lasso;
    lasso.AddPoints(lassoPoints);
    for (int i = 0; i < size(); i++)
    {
        QSharedPointer<Stroke> stroke = (*this)[i];

        QSharedPointer<StrokeCollection> eraseResult = stroke->Erase(stroke->HitTest(lasso));
        UpdateStrokeCollection(stroke, eraseResult, i);
    }
}


/// <summary>
/// Erases all ink inside a given rectangle
/// </summary>
/// <param name="bounds">rectangle to erase within</param>
void StrokeCollection::Erase(QRectF const & bounds)
{
    if (bounds.isEmpty() == false)
    {
        Erase({ bounds.topLeft(), bounds.topRight(), bounds.bottomRight(), bounds.bottomLeft() });
    }
}


/// <summary>
/// Erases all ink hit by the contour of an erasing stroke
/// </summary>
/// <param name="eraserShape">Shape of the eraser</param>
/// <param name="eraserPath">a path making the spine of the erasing stroke </param>
void StrokeCollection::Erase(QVector<QPointF> const & eraserPath, StylusShape& eraserShape)
{
    // Check the input parameters
    //if (eraserShape == nullptr)
    //{
    //    throw std::exception("SR.Get(SRID.SCEraseShape)");
    //}
    //if (eraserPath == nullptr)
    //{
    //    throw std::exception(SR.Get(SRID.SCErasePath));
    //}
    if (eraserPath.size() == 0)
    {
        return;
    }

    ErasingStroke* erasingStroke = new ErasingStroke(eraserShape, eraserPath);
    for (int i = 0; i < size(); i++)
    {
        QSharedPointer<Stroke> stroke = (*this)[i];

        QList<StrokeIntersection> intersections;
        erasingStroke->EraseTest(StrokeNodeIterator::GetIterator(*stroke, *stroke->GetDrawingAttributes()), intersections);
        QSharedPointer<StrokeCollection> eraseResult = stroke->Erase(intersections.toVector());

        UpdateStrokeCollection(stroke, eraseResult, i);
    }
}

static bool operator<(QColor l, QColor r)
{
    return l.rgba() < r.rgba();
}

/// <summary>
/// Render the StrokeCollection under the specified DrawingContext.
/// </summary>
/// <param name="context"></param>
void StrokeCollection::Draw(DrawingContext& context)
{
    //if (nullptr == context)
    //{
    //    throw std::exception("context");
    //}

    //The verification of UI context affinity is done in Stroke.Draw()

    QList<QSharedPointer<Stroke>> solidStrokes;
    QMap<QColor, QList<QSharedPointer<Stroke>>> highLighters;

    for (int i = 0; i < size(); i++)
    {
        QSharedPointer<Stroke> stroke = (*this)[i];
        QList<QSharedPointer<Stroke>> strokes;
        if (stroke->GetDrawingAttributes()->IsHighlighter())
        {
            // It's very important to override the Alpha value so that Colors of the same RGB vale
            // but different Alpha would be in the same list.
            QColor color = StrokeRenderer::GetHighlighterColor(stroke->GetDrawingAttributes()->Color());
            //if (highLighters.TryGetValue(color, out strokes) == false)
            //{
            //    strokes = new List<Stroke>();
            //    highLighters.Add(color, strokes);
            //}
            highLighters[color].append(stroke);
        }
        else
        {
            solidStrokes.append(stroke);
        }
    }

    for (QList<QSharedPointer<Stroke>> strokes : highLighters.values())
    {
        context.PushOpacity(StrokeRenderer::HighlighterOpacity);
        FinallyHelper final([&context](){
            context.Pop();
        });

        //try
        {
            for (QSharedPointer<Stroke> stroke : strokes)
            {
                stroke->DrawInternal(context, StrokeRenderer::GetHighlighterAttributes(*stroke, stroke->GetDrawingAttributes()),
                                    false /*Don't draw selected stroke as hollow*/);
            }
        }
        //finally
        //{
        //    context.Pop();
        //}
    }

    for (QSharedPointer<Stroke> stroke : solidStrokes)
    {
        stroke->DrawInternal(context, stroke->GetDrawingAttributes(), false/*Don't draw selected stroke as hollow*/);
    }
}


/// <summary>
/// Creates an incremental hit-tester for hit-testing with a shape.
/// Scenarios: stroke-erasing and point-erasing
/// </summary>
/// <param name="eraserShape">shape of the eraser</param>
/// <returns>an instance of IncrementalStrokeHitTester</returns>
IncrementalStrokeHitTester* StrokeCollection::GetIncrementalStrokeHitTester(StylusShape& eraserShape)
{
    //if (eraserShape == nullptr)
    //{
    //    throw std::exception("eraserShape");
    //}
    return new IncrementalStrokeHitTester(sharedFromThis(), eraserShape);
}


/// <summary>
/// Creates an incremental hit-tester for selecting with lasso.
/// </summary>
/// <param name="percentageWithinLasso">The percentage of the stroke that must be within the lasso to be considered hit</param>
/// <returns>an instance of incremental hit-tester</returns>
IncrementalLassoHitTester* StrokeCollection::GetIncrementalLassoHitTester(int percentageWithinLasso)
{
    if ((percentageWithinLasso < 0) || (percentageWithinLasso > 100))
    {
        throw std::exception("percentageWithinLasso");
    }
    return new IncrementalLassoHitTester(sharedFromThis(), percentageWithinLasso);
}

/// <summary>
/// Return all hit strokes that the StylusShape intersects and returns them in a StrokeCollection
/// </summary>
QSharedPointer<StrokeCollection> StrokeCollection::PointHitTest(QPointF const & point, StylusShape& shape)
{
    // Create the collection to return
    QSharedPointer<StrokeCollection> hits(new StrokeCollection());
    for (int i = 0; i < size(); i++)
    {
        QSharedPointer<Stroke> stroke = (*this)[i];
        if (stroke->HitTest(QVector<QPointF>({ point }), shape))
        {
            hits->append(stroke);
        }
    }

    return hits;
}

void StrokeCollection::UpdateStrokeCollection(QSharedPointer<Stroke> original, QSharedPointer<StrokeCollection> toReplace, int& index)
{
    //System.Diagnostics.Debug.Assert(original != null && toReplace != nullptr);
    //System.Diagnostics.Debug.Assert(index >= 0 && index < this.Count);
    if (toReplace->size() == 0)
    {
        RemoveItem(original);
        index--;
    }
    else if (!(toReplace->size() == 1 && (*toReplace)[0] == original))
    {
        Replace(original, toReplace);

        // Update the current index
        index += toReplace->size() - 1;
    }
}
