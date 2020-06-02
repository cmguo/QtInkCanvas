#include "Windows/Ink/strokecollection.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/events.h"
#include "Windows/Ink/incrementalhittester.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Ink/stylusshape.h"
#include "Internal/Ink/lasso.h"
#include "Internal/Ink/erasingstroke.h"
#include "Windows/Input/styluspoint.h"
#include "Internal/Ink/strokerenderer.h"
#include "Internal/finallyhelper.h"

#ifndef INKCANVAS_CORE
#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"
#include "Windows/Media/drawingcontext.h"
#include "notifycollectionchangedeventargs.h"
#endif

#ifdef INKCANVAS_QT
#include <QIODevice>
#include <QBuffer>
#include <Matrix>
#endif

INKCANVAS_BEGIN_NAMESPACE

StrokeCollection::StrokeCollection()
{
}

StrokeCollection::StrokeCollection(StrokeCollection const & o)
    : Collection<SharedPointer<Stroke>>(o)
    , _extendedProperties(o._extendedProperties)
{
    if (_extendedProperties)
        _extendedProperties = _extendedProperties->Clone();
}

StrokeCollection::~StrokeCollection()
{
    delete _extendedProperties;
#if STROKE_COLLECTION_MULTIPLE_LAYER
    for (QObject * c : children())
        c->setParent(nullptr);
#endif
}

/// <summary>Creates a StrokeCollection based on a collection of existing strokes</summary>
StrokeCollection::StrokeCollection(Collection<SharedPointer<Stroke>> const & strokes)
{
    List<SharedPointer<Stroke>> items = Items();
    //unfortunately we have to check for dupes with this ctor
    for ( SharedPointer<Stroke> stroke : strokes )
    {
        if ( items.Contains(stroke) )
        {
            //clear and throw
            Clear();
            throw std::runtime_error("strokes");
        }
        items.Add(stroke);
    }
}

#ifdef INKCANVAS_QT

/// <summary>Creates a collection from ISF data in the specified stream</summary>
/// <param name="stream">Stream of ISF data</param>
StrokeCollection::StrokeCollection(QIODevice * stream)
{
    if ( stream == nullptr )
    {
        throw std::runtime_error("stream");
    }
    if ( !stream->isReadable() )
    {
        throw std::runtime_error("stream");
    }

    QIODevice * seekableStream = GetSeekableStream(stream);
    if (seekableStream == nullptr)
    {
        throw std::runtime_error("stream");
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
        throw std::runtime_error("stream");
    }
    if ( !stream->isWritable() )
    {
        throw std::runtime_error("stream");
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

#endif

/// <summary>
/// Allows addition of objects to the EPC
/// </summary>
/// <param name="propertyDataId"></param>
/// <param name="propertyData"></param>
void StrokeCollection::AddPropertyData(Guid const & propertyDataId, Variant const & propertyData)
{
    DrawingAttributes::ValidateStylusTipTransform(propertyDataId, propertyData);
    Variant oldValue;
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
void StrokeCollection::RemovePropertyData(Guid const & propertyDataId)
{
    Variant propertyData = GetPropertyData(propertyDataId);
    _extendedProperties->Remove(propertyDataId);
    // fire notification
    OnPropertyDataChanged(propertyDataId);
}

/// <summary>
/// Allows retrieval of objects from the EPC
/// </summary>
/// <param name="propertyDataId"></param>
Variant StrokeCollection::GetPropertyData(Guid const & propertyDataId) const
{
    if ( propertyDataId == Guid() )
    {
        throw std::runtime_error("propertyDataId");
    }

    return (*_extendedProperties)[propertyDataId];
}

/// <summary>
/// Allows retrieval of a Array of Guid const &s that are contained in the EPC
/// </summary>
Array<Guid> StrokeCollection::GetPropertyDataIds() const
{
    return _extendedProperties->GetGuidArray();
}

/// <summary>
/// Allows the checking of objects in the EPC
/// </summary>
/// <param name="propertyDataId"></param>
bool StrokeCollection::ContainsPropertyData(Guid const & propertyDataId) const
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
void StrokeCollection::Transform(Matrix & transformMatrix, bool applyToStylusTip)
{
    // Ensure that the transformMatrix is invertible.
    if ( false == transformMatrix.HasInverse() )
        throw std::runtime_error("transformMatrix");

    // if transformMatrix is identity or the StrokeCollection is empty
    //      then no change will occur anyway
    if ( transformMatrix.IsIdentity() || Count() == 0 )
    {
        return;
    }

    // Apply the transform to each strokes
    for ( SharedPointer<Stroke> stroke : (*this) )
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
SharedPointer<StrokeCollection> StrokeCollection::Clone()
{
   SharedPointer<StrokeCollection> clone(new StrokeCollection());
    for ( SharedPointer<Stroke> s : (*this) )
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that s could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
        clone->Add(s->Clone());
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
    if ( Count() > 0 )
    {
        SharedPointer<StrokeCollection> removed(new StrokeCollection);
        for ( int x = 0; x < Count(); x++ )
        {
            removed->Add((*this)[x]);
        }

        Collection::ClearItems();

        RaiseStrokesChanged(nullptr /*added*/, removed, -1);
    }
}


/// <summary>
/// called by base class RemoveAt or Remove methods
/// </summary>
void StrokeCollection::RemoveItem(int index)
{
    SharedPointer<Stroke> removedStroke = (*this)[index];
    Collection::RemoveItem(index);

    SharedPointer<StrokeCollection> removed(new StrokeCollection);
    removed->Items().Add(removedStroke);
    RaiseStrokesChanged(nullptr /*added*/, removed, index);
}

bool StrokeCollection::RemoveItem(SharedPointer<Stroke> stroke)
{
    int index = IndexOf(stroke);
    if (index < 0) {
#if STROKE_COLLECTION_MULTIPLE_LAYER
        if (!children().isEmpty()) {
            // try in other collections
            SharedPointer<StrokeCollection> strokes(new StrokeCollection);
            strokes->Items().AddItem(stroke);
            Remove(strokes);
        }
#endif
        return false;
    }
    RemoveItem(index);
    return true;
}

/// <summary>
/// called by base class Insert, Add methods
/// </summary>
void StrokeCollection::InsertItem(int index, SharedPointer<Stroke> stroke)
{
    if ( stroke == nullptr )
    {
        throw std::runtime_error("stroke");
    }
    if ( IndexOf(stroke) != -1 )
    {
        throw std::runtime_error("stroke");
    }

    Collection::InsertItem(index, stroke);

    SharedPointer<StrokeCollection> addedStrokes(new StrokeCollection);
    addedStrokes->Items().Add(stroke);
    RaiseStrokesChanged(addedStrokes, nullptr /*removed*/, index);
}

/// <summary>
/// called by base class set_Item method
/// </summary>
void StrokeCollection::SetItem(int index, SharedPointer<Stroke> stroke)
{
    if ( stroke == nullptr )
    {
        throw std::runtime_error("stroke");
    }
    if ( IndexOf(stroke) != -1 )
    {
        throw std::runtime_error("stroke");
    }

    SharedPointer<Stroke> removedStroke = (*this)[index];
    at(index) = stroke;

    SharedPointer<StrokeCollection> removed(new StrokeCollection);
    removed->Items().Add(removedStroke);

    SharedPointer<StrokeCollection> added(new StrokeCollection);
    added->Items().Add(stroke);
    RaiseStrokesChanged(added, removed, index);
}

/// <summary>
/// Gets the index of the stroke, or -1 if it is not found
/// </summary>
/// <param name="stroke">stroke</param>
/// <returns></returns>
int StrokeCollection::IndexOf(SharedPointer<Stroke> stroke)
{
    if (stroke == nullptr)
    {
        //we never allow null strokes
        return -1;
    }
    for (int i = 0; i < Count(); i++)
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
void StrokeCollection::Remove(SharedPointer<StrokeCollection> strokes)
{
    if ( strokes->Count() == 0 )
    {
        // NOTICE-2004/06/08-WAYNEZEN:
        // We don't throw if an empty collection is going to be removed. And there is no event either.
        // This rule is also applied to invoking Clear() with an empty StrokeCollection.
        return;
    }

    List<int> indexes = GetStrokeIndexes(strokes);

#if STROKE_COLLECTION_MULTIPLE_LAYER
    if (indexes.size() == 0 && !children().isEmpty()) {
        for (QObject * c : children()) {
            StrokeCollection * cc = qobject_cast<StrokeCollection*>(c);
            SharedPointer<StrokeCollection> ss(new StrokeCollection);
            for (SharedPointer<Stroke> & s : static_cast<QVector<SharedPointer<Stroke>>&>(*strokes)) {
                if (cc->contains(s)) {
                    ss->append(s);
                    s = nullptr;
                }
            }
            if (ss->size() > 0) {
                cc->Remove(ss);
                strokes->removeAll(nullptr);
            }
        }
        if ( strokes->size() == 0 )
            return;
        indexes = GetStrokeIndexes(strokes);
    }
#endif

    for ( int x = indexes.Count() - 1; x >= 0; x-- )
    {
        //bypass this.RemoveAt, which calls changed events
        //and call our protected List<Stroke> directly
        //remove from the back so the indexes are correct
        this->Items().RemoveAt(indexes[x]);
    }

    RaiseStrokesChanged(nullptr /*added*/, strokes, indexes[0]);
}

/// <summary>
/// Add a set of Stroke objects to the collection
/// </summary>
/// <param name="strokes">The strokes to add to the collection</param>
/// <remarks>The items are added to the collection at the end of the list.
/// If the item already exists in the collection, then the item is not added again.</remarks>
void StrokeCollection::Add(SharedPointer<StrokeCollection> strokes)
{
    if ( strokes->Count() == 0 )
    {
        // NOTICE-2004/06/08-WAYNEZEN:
        // We don't throw if an empty collection is going to be added. And there is no event either.
        return;
    }

    int index = Count();

    //validate that none of the strokes exist in the collection
    for ( int x = 0; x < strokes->Count(); x++ )
    {
        SharedPointer<Stroke> stroke = (*strokes)[x];
        if ( IndexOf(stroke) != -1 )
        {
            throw std::runtime_error("strokes");
        }
    }

    //add the strokes
    //bypass this.AddRange, which calls changed events
    //and call our protected List<Stroke> directly
    this->Items().AddRange(strokes->Items());

    RaiseStrokesChanged(strokes, nullptr /*removed*/, index);
}

/// <summary>
/// Replace
/// </summary>
/// <param name="strokeToReplace"></param>
/// <param name="strokesToReplaceWith"></param>
void StrokeCollection::Replace(SharedPointer<Stroke> strokeToReplace, SharedPointer<StrokeCollection> strokesToReplaceWith)
{
    if ( strokeToReplace == nullptr )
    {
        throw std::runtime_error("SR.Get(SRID.EmptyScToReplace)");
    }

    SharedPointer<StrokeCollection> strokesToReplace(new StrokeCollection);
    strokesToReplace->Add(strokeToReplace);
    Replace(strokesToReplace, strokesToReplaceWith);
}

/// <summary>
/// Replace
/// </summary>
/// <param name="strokesToReplace"></param>
/// <param name="strokesToReplaceWith"></param>
void StrokeCollection::Replace(SharedPointer<StrokeCollection> strokesToReplace, SharedPointer<StrokeCollection> strokesToReplaceWith)
{
    int replaceCount = strokesToReplace->Count();
    if ( replaceCount == 0 )
    {
        throw std::runtime_error("strokesToReplace");
    }

    List<int> indexes = GetStrokeIndexes(strokesToReplace);
    if ( indexes.Count() == 0 )
    {
#if STROKE_COLLECTION_MULTIPLE_LAYER
        if (!children().isEmpty()) {
            for (QObject * c : children()) {
                try {
                    qobject_cast<StrokeCollection*>(c)
                            ->Replace(strokesToReplace, strokesToReplaceWith);
                    return;
                } catch (std::exception &) {
                }
            }
        }
#endif
        throw std::runtime_error("strokesToReplace");
    }

    //validate that none of the relplaceWith strokes exist in the collection
    for ( int x = 0; x < strokesToReplaceWith->Count(); x++ )
    {
        SharedPointer<Stroke> stroke = (*strokesToReplaceWith)[x];
        if ( IndexOf(stroke) != -1 )
        {
            throw std::runtime_error("strokesToReplaceWith");
        }
    }

    //bypass this.RemoveAt / InsertRange, which calls changed events
    //and call our protected List<Stroke> directly
    for ( int x = indexes.Count() - 1; x >= 0; x-- )
    {
        //bypass this.RemoveAt, which calls changed events
        //and call our protected List<Stroke> directly
        //remove from the back so the indexes are correct
        this->Items().RemoveAt(indexes[x]);
    }

    if ( strokesToReplaceWith->Count() > 0 )
    {
        //insert at the
        this->Items().InsertRange(indexes[0], strokesToReplaceWith->Items());
    }


    RaiseStrokesChanged(strokesToReplaceWith, strokesToReplace, indexes[0]);
}

/// <summary>
/// called by StrokeCollectionSerializer during Load, bypasses Change notification
/// </summary>
void StrokeCollection::AddWithoutEvent(SharedPointer<Stroke>stroke)
{
    //Debug.Assert(stroke != null && IndexOf(stroke) == -1);
    this->Items().Add(stroke);
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

const ExtendedPropertyCollection &StrokeCollection::ExtendedProperties() const
{
    if ( _extendedProperties == nullptr )
    {
        static ExtendedPropertyCollection emptyProperties;
        return emptyProperties;
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
#ifdef INKCANVAS_QT
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
        if ( Count() == 0 )
        {
            //Reset
            //Debug.Assert(e.Removed.Count > 0);
            args = new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction::Reset);
        }
        else if ( e.Added()->Count() == 0 )
        {
            //Remove
            args = new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction::Remove, *e.Removed(), e.Index());
        }
        else if ( e.Removed()->Count() == 0 )
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
#endif
}


/// <summary>
/// Method called on derived classes whenever a change occurs in
/// the PropertyData.
/// </summary>
/// <remarks>Derived classes should call this method (their base class)
/// to ensure that event listeners are notified</remarks>
void StrokeCollection::OnPropertyDataChanged(Guid const & propName)
{
#ifdef INKCANVAS_QT
    //if ( this.PropertyDataChanged != null )
    {
        emit PropertyDataChanged(propName);
    }
#endif
}

/// <summary>
/// Method called when a property change occurs to the StrokeCollection
/// </summary>
/// <param name="e">The EventArgs specifying the name of the changed property.</param>
/// <remarks>To follow the Guid const &elines, this method should take a PropertyChangedEventArgs
/// instance, but every other INotifyPropertyChanged implementation follows this pattern.</remarks>
void StrokeCollection::OnPropertyChanged(char const * propName)
{
#ifdef INKCANVAS_QT
    //if ( _propertyChanged != null )
    {
        _propertyChanged(propName);
    }
#endif
}

#ifdef INKCANVAS_QT
StrokeCollection::operator QVariantList()
{
    QVariantList list;
    for (auto s : *this)
        list.append(QVariant::fromValue(s));
    return list;
}
#endif

/// <summary>
/// helper that starts searching for stroke at index,
/// but will loop around before reporting -1.  This is used for
/// Stroke.Remove(StrokeCollection).  For example, if we're removing
/// strokes, chances are they are in contiguous order.  If so, calling
/// IndexOf to validate each stroke is O(n2).  If the strokes are in order
/// this produces closer to O(n), if they are not in order, it is no worse
/// </summary>
int StrokeCollection::OptimisticIndexOf(int startingIndex, SharedPointer<Stroke>stroke)
{
    //Debug.Assert(startingIndex >= 0);
    for ( int x = startingIndex; x < Count(); x++ )
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
Array<int> StrokeCollection::GetStrokeIndexes(SharedPointer<StrokeCollection> strokes)
{
    //to keep from walking the StrokeCollection twice for each stroke, we will maintain an index of
    //strokes to remove as we go
    Array<int> indexes(strokes->Count());
    for ( int x = 0; x < indexes.Length(); x++ )
    {
        indexes[x] = INT_MAX;
    }

    int currentIndex = 0;
    int highestIndex = -1;
    int usedIndexCount = 0;
    for ( int x = 0; x < strokes->Count(); x++ )
    {
        currentIndex = OptimisticIndexOf(currentIndex, (*strokes)[x]);
        if ( currentIndex == -1 )
        {
            //stroke doe3sn't exist, bail out.
            return Array<int>();
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
        for ( int y = 0; y < indexes.Length(); y++ )
        {
            if ( currentIndex < indexes[y] )
            {
                if ( indexes[y] != INT_MAX )
                {
                    //shift from the end
                    for ( int i = indexes.Length() - 1; i > y; i-- )
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
void StrokeCollection::RaiseStrokesChanged(SharedPointer<StrokeCollection> addedStrokes, SharedPointer<StrokeCollection> removedStrokes, int index)
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
Rect StrokeCollection::GetBounds()
{
    Rect bounds;
    for (SharedPointer<Stroke> stroke : *this)
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
//#pragma warning disable 1634, 1691
//#pragma warning suppress 6506
        bounds.Union(stroke->GetBounds());
//#pragma warning restore 1634, 1691
    }
    return bounds;
}

#if STROKE_COLLECTION_EDIT_MASK

void StrokeCollection::SetEditMask(const QPolygonF &clipShape)
{
    maskShape_ = clipShape;
    if (mask_) {
        delete mask_;
        mask_ = nullptr;
    }
}

ErasingStroke *StrokeCollection::GetEditMask()
{
    if (mask_)
        return mask_;
    if (!maskShape_.empty()) {
        Point c = maskShape_.boundingRect().center();
        StylusShape ss(maskShape_.translated(-c));
        mask_ = new ErasingStroke(ss);
        mask_->MoveTo({c});
    }
    return mask_;
}

#endif

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
SharedPointer<StrokeCollection> StrokeCollection::HitTest(Point const & point)
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
SharedPointer<StrokeCollection> StrokeCollection::HitTest(Point const & point, double diameter)
{
    if (Double::IsNaN(diameter) || diameter < DrawingAttributes::MinWidth || diameter > DrawingAttributes::MaxWidth)
    {
        throw std::runtime_error("diameter");
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
SharedPointer<StrokeCollection> StrokeCollection::HitTest(List<Point> const & lassoPoints, int percentageWithinLasso)
{
    // Check the input parameters
    //if (lassoPoints == nullptr)
    //{
    //    throw std::runtime_error("lassoPoints");
    //}
    if ((percentageWithinLasso < 0) || (percentageWithinLasso > 100))
    {
        throw std::runtime_error("percentageWithinLasso");
    }

    if (lassoPoints.Count() < 3)
    {
        return SharedPointer<StrokeCollection>();
    }

    Lasso* lasso = new SingleLoopLasso();
    lasso->AddPoints(lassoPoints);

    // Enumerate through the strokes and collect those captured by the lasso.
    SharedPointer<StrokeCollection> lassoedStrokes(new StrokeCollection());
    for (SharedPointer<Stroke> stroke : *this)
    {

        if (percentageWithinLasso == 0)
        {
            lassoedStrokes->Add(stroke);
        }
        else
        {


            //try
            {
                StrokeInfo strokeInfo(stroke);

                SharedPointer<StylusPointCollection> stylusPoints = strokeInfo.StylusPoints();
                double target = strokeInfo.TotalWeight() * percentageWithinLasso / 100.0 - Stroke::PercentageTolerance;

                for (int i = 0; i < stylusPoints->Count(); i++)
                {
                    if (true == lasso->Contains((*stylusPoints)[i]))
                    {
                        target -= strokeInfo.GetPointWeight(i);
                        if (DoubleUtil::LessThanOrClose(target, 0))
                        {
                            lassoedStrokes->Add(stroke);
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
SharedPointer<StrokeCollection> StrokeCollection::HitTest(Rect const & bounds, int percentageWithinBounds)
{
    // Check the input parameters
    if ((percentageWithinBounds < 0) || (percentageWithinBounds > 100))
    {
        throw std::runtime_error("percentageWithinBounds");
    }
    if (bounds.IsEmpty())
    {
        return SharedPointer<StrokeCollection>();
    }

    // Enumerate thru the strokes collect those found within the rectangle.
    SharedPointer<StrokeCollection> hits(new StrokeCollection());
    for (SharedPointer<Stroke> stroke : *this)
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
//#pragma warning disable 1634, 1691
//#pragma warning suppress 6506
        if (true == stroke->HitTest(bounds, percentageWithinBounds))
        {
            hits->Add(stroke);
        }
//#pragma warning restore 1634, 1691
    }
    return hits;
}


/// <summary>
/// Issue: what's the return value
/// </summary>
/// <param name="path"></param>
/// <param name="stylusShape"></param>
/// <returns></returns>
SharedPointer<StrokeCollection> StrokeCollection::HitTest(List<Point> const & path, StylusShape& stylusShape)
{
    // Check the input parameters
    //if (stylusShape == nullptr)
    //{
    //    throw std::runtime_error("stylusShape");
    //}
    //if (path == nullptr)
    //{
    //    throw std::runtime_error("path");
    //}
    if (path.Count() == 0)
    {
        return SharedPointer<StrokeCollection>();
    }

    // validate input
    ErasingStroke erasingStroke(stylusShape, path);
    Rect erasingBounds = erasingStroke.Bounds();
    if (erasingBounds.IsEmpty())
    {
        return SharedPointer<StrokeCollection>();
    }
    SharedPointer<StrokeCollection> hits(new StrokeCollection());
    for (SharedPointer<Stroke> stroke : *this)
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when get methods might deref a null.  It's complaining
        // here that 'stroke'' could be null, but StrokeCollection never allows nulls to be added
        // so this is not possible
//#pragma warning disable 1634, 1691
//#pragma warning suppress 6506
        if (erasingBounds.IntersectsWith(stroke->GetBounds()) &&
            erasingStroke.HitTest(StrokeNodeIterator::GetIterator(*stroke, *stroke->GetDrawingAttributes())))
        {
            hits->Add(stroke);
        }
//#pragma warning restore 1634, 1691
    }

    return hits;
}

/// <summary>
/// Clips out all ink outside a given lasso
/// </summary>
/// <param name="lassoPoints">lasso</param>
void StrokeCollection::Clip(List<Point> const & lassoPoints)
{
    // Check the input parameters
    //if (lassoPoints == nullptr)
    //{
    //    throw std::runtime_error("lassoPoints");
    //}

    int length = lassoPoints.Count();
    if (length == 0)
    {
        throw std::runtime_error("SR.Get(SRID.EmptyArray)");
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
        Clear(); //raises the appropriate events
        return;
    }

    SingleLoopLasso lasso;
    lasso.AddPoints(lassoPoints);

    for (int i = 0; i < Count(); i++)
    {
        SharedPointer<Stroke> stroke = (*this)[i];
        SharedPointer<StrokeCollection> clipResult = stroke->Clip(stroke->HitTest(lasso));
        UpdateStrokeCollection(stroke, clipResult, i);
    }
}

/// <summary>
/// Clips out all ink outside a given rectangle.
/// </summary>
/// <param name="bounds">rectangle to clip with</param>
void StrokeCollection::Clip(Rect const & bounds)
{
    if (bounds.IsEmpty() == false)
    {
        Clip({ bounds.TopLeft(), bounds.TopRight(), bounds.BottomRight(), bounds.BottomLeft() });
    }
}

/// <summary>
/// Erases all ink inside a lasso
/// </summary>
/// <param name="lassoPoints">lasso to erase within</param>
void StrokeCollection::Erase(List<Point> const & lassoPoints)
{
    // Check the input parameters
    //if (lassoPoints == nullptr)
    //{
    //    throw std::runtime_error("lassoPoints");
    //}
    int length = lassoPoints.Count();
    if (length == 0)
    {
        throw std::runtime_error("SR.Get(SRID.EmptyArray)");
    }

    if (length < 3)
    {
        return;
    }

    SingleLoopLasso lasso;
    lasso.AddPoints(lassoPoints);
    for (int i = 0; i < Count(); i++)
    {
        SharedPointer<Stroke> stroke = (*this)[i];

        SharedPointer<StrokeCollection> eraseResult = stroke->Erase(stroke->HitTest(lasso));
        UpdateStrokeCollection(stroke, eraseResult, i);
    }
}


/// <summary>
/// Erases all ink inside a given rectangle
/// </summary>
/// <param name="bounds">rectangle to erase within</param>
void StrokeCollection::Erase(Rect const & bounds)
{
    if (bounds.IsEmpty() == false)
    {
        Erase({ bounds.TopLeft(), bounds.TopRight(), bounds.BottomRight(), bounds.BottomLeft() });
    }
}


/// <summary>
/// Erases all ink hit by the contour of an erasing stroke
/// </summary>
/// <param name="eraserShape">Shape of the eraser</param>
/// <param name="eraserPath">a path making the spine of the erasing stroke </param>
void StrokeCollection::Erase(List<Point> const & eraserPath, StylusShape& eraserShape)
{
    // Check the input parameters
    //if (eraserShape == nullptr)
    //{
    //    throw std::runtime_error("SR.Get(SRID.SCEraseShape)");
    //}
    //if (eraserPath == nullptr)
    //{
    //    throw std::runtime_error(SR.Get(SRID.SCErasePath));
    //}
    if (eraserPath.Count() == 0)
    {
        return;
    }

    ErasingStroke erasingStroke(eraserShape, eraserPath);
    for (int i = 0; i < Count(); i++)
    {
        SharedPointer<Stroke> stroke = (*this)[i];

        List<StrokeIntersection> intersections;
        erasingStroke.EraseTest(StrokeNodeIterator::GetIterator(*stroke, *stroke->GetDrawingAttributes()), intersections);
#if STROKE_COLLECTION_EDIT_MASK
        if (GetEditMask()) {
            QVector<StrokeIntersection> mask;
            mask_->EraseTest(StrokeNodeIterator::GetIterator(*stroke, *stroke->GetDrawingAttributes()), mask);
            if (!mask.isEmpty())
                intersections = StrokeIntersection::GetMaskedHitSegments(intersections, mask);
        }
        if (intersections.isEmpty())
            continue;
#endif
        SharedPointer<StrokeCollection> eraseResult = stroke->Erase(intersections.ToArray());

        UpdateStrokeCollection(stroke, eraseResult, i);
    }

#if STROKE_COLLECTION_MULTIPLE_LAYER
    for (QObject * c : children()) {
        qobject_cast<StrokeCollection*>(c)->Erase(eraserPath, eraserShape);
    }
#endif
}

INKCANVAS_END_NAMESPACE

#ifdef INKCANVAS_QT
static bool operator<(QColor l, QColor r)
{
    return l.rgba() < r.rgba();
}
#endif

INKCANVAS_BEGIN_NAMESPACE

#ifndef INKCANVAS_CORE

/// <summary>
/// Render the StrokeCollection under the specified DrawingContext.
/// </summary>
/// <param name="context"></param>
void StrokeCollection::Draw(DrawingContext& context)
{
    //if (nullptr == context)
    //{
    //    throw std::runtime_error("context");
    //}

    //The verification of UI context affinity is done in Stroke.Draw()

    QList<SharedPointer<Stroke>> solidStrokes;
    QMap<QColor, QList<SharedPointer<Stroke>>> highLighters;

    for (int i = 0; i < size(); i++)
    {
        SharedPointer<Stroke> stroke = (*this)[i];
        QList<SharedPointer<Stroke>> strokes;
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

    for (QList<SharedPointer<Stroke>> strokes : highLighters.values())
    {
        context.PushOpacity(StrokeRenderer::HighlighterOpacity);
        FinallyHelper final([&context](){
            context.Pop();
        });

        //try
        {
            for (SharedPointer<Stroke> stroke : strokes)
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

    for (SharedPointer<Stroke> stroke : solidStrokes)
    {
        stroke->DrawInternal(context, stroke->GetDrawingAttributes(), false/*Don't draw selected stroke as hollow*/);
    }
}

#endif

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
    //    throw std::runtime_error("eraserShape");
    //}
    return new IncrementalStrokeHitTester(shared_from_this(), eraserShape);
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
        throw std::runtime_error("percentageWithinLasso");
    }
    return new IncrementalLassoHitTester(shared_from_this(), percentageWithinLasso);
}

/// <summary>
/// Return all hit strokes that the StylusShape intersects and returns them in a StrokeCollection
/// </summary>
SharedPointer<StrokeCollection> StrokeCollection::PointHitTest(Point const & point, StylusShape& shape)
{
    // Create the collection to return
    SharedPointer<StrokeCollection> hits(new StrokeCollection());
    for (int i = 0; i < Count(); i++)
    {
        SharedPointer<Stroke> stroke = (*this)[i];
        if (stroke->HitTest(List<Point>({ point }), shape))
        {
            hits->Add(stroke);
        }
    }
#if STROKE_COLLECTION_MULTIPLE_LAYER
    for (QObject * c : children()) {
         hits->append(*qobject_cast<StrokeCollection*>(c)->PointHitTest(point, shape));
    }
#endif

    return hits;
}

void StrokeCollection::UpdateStrokeCollection(SharedPointer<Stroke> original, SharedPointer<StrokeCollection> toReplace, int& index)
{
    //System.Diagnostics.Debug.Assert(original != null && toReplace != nullptr);
    //System.Diagnostics.Debug.Assert(index >= 0 && index < this.Count);
    if (toReplace->Count() == 0)
    {
        RemoveItem(original);
        index--;
    }
    else if (!(toReplace->Count() == 1 && (*toReplace)[0] == original))
    {
        Replace(original, toReplace);

        // Update the current index
        index += toReplace->Count() - 1;
    }
}

INKCANVAS_END_NAMESPACE
