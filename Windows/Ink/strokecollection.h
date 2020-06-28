#ifndef STROKECOLLECTION_H
#define STROKECOLLECTION_H

#include "InkCanvas_global.h"
#include "Collections/ObjectModel/collection.h"
#include "Collections/Generic/list.h"
#include "Collections/Generic/array.h"
#include "Windows/rect.h"
#include "sharedptr.h"
#include "guid.h"
#include "variant.h"

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

class StrokeCollectionChangedEventArgs;
class NotifyCollectionChangedEventArgs;
class IncrementalStrokeHitTester;
class IncrementalLassoHitTester;
class StylusShape;
class Stroke;
class DrawingContext;
class ExtendedPropertyCollection;
class ErasingStroke;

#ifdef INKCANVAS_QT
#define STROKE_COLLECTION_MULTIPLE_LAYER 1
#define STROKE_COLLECTION_EDIT_MASK 1
#else
#define STROKE_COLLECTION_MULTIPLE_LAYER 0
#define STROKE_COLLECTION_EDIT_MASK 0
#endif

#ifdef INKCANVAS_QT
class INKCANVAS_EXPORT StrokeCollection : public QObject, public Collection<SharedPointer<Stroke>>, public EnableSharedFromThis<StrokeCollection>
{
    Q_OBJECT
#else
class INKCANVAS_EXPORT StrokeCollection : public Collection<SharedPointer<Stroke>>, public EnableSharedFromThis<StrokeCollection>
{
#endif
public:
    /// <summary>
    /// The string used to designate the native persistence format
    ///      for ink data (e.g. used on the clipboard)
    /// </summary>
    static constexpr char const * InkSerializedFormat = "Ink Serialized Format";

    /// <summary>Creates an empty stroke collection</summary>
    StrokeCollection();

    /// <summary>Creates a StrokeCollection based on a collection of existing strokes</summary>
    StrokeCollection(Collection<SharedPointer<Stroke>> const & strokes);

#ifndef INKCANVAS_CORE
    /// <summary>Creates a collection from ISF data in the specified stream</summary>
    /// <param name="stream">Stream of ISF data</param>
    StrokeCollection(QIODevice * stream);
#endif

    StrokeCollection(StrokeCollection const & o);

    ~StrokeCollection();

#ifdef INKCANVAS_QT
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
#endif

public:
    /// <summary>
    /// Allows addition of objects to the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    /// <param name="propertyData"></param>
    void AddPropertyData(Guid const & propertyDataId, Variant const & propertyData);

    /// <summary>
    /// Allows removal of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    void RemovePropertyData(Guid const & propertyDataId);

    /// <summary>
    /// Allows retrieval of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    Variant GetPropertyData(Guid const & propertyDataId) const;

    /// <summary>
    /// Allows retrieval of a Array of Guid const &s that are contained in the EPC
    /// </summary>
    Array<Guid> GetPropertyDataIds() const;

    /// <summary>
    /// Allows the checking of objects in the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    bool ContainsPropertyData(Guid const & propertyDataId) const;

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
    void Transform(Matrix const & transformMatrix, bool applyToStylusTip);

    /// <summary>
    /// Performs a deep copy of the StrokeCollection.
    /// </summary>
    virtual SharedPointer<StrokeCollection> Clone();

    /// <summary>
    /// called by base class Collection&lt;T&gt; when the list is being cleared;
    /// raises a CollectionChanged event to any listeners
    /// </summary>
    void ClearItems() override;

    /// <summary>
    /// called by base class RemoveAt or Remove methods
    /// </summary>
    void RemoveItem(int index) override;

    /// <summary>
    /// called by base class Insert, Add methods
    /// </summary>
    void InsertItem(int index, SharedPointer<Stroke> const & stroke) override;

    /// <summary>
    /// called by base class set_Item method
    /// </summary>
    void SetItem(int index, SharedPointer<Stroke> const & stroke) override;

    /// <summary>
    /// Gets the index of the stroke, or -1 if it is not found
    /// </summary>
    /// <param name="stroke">stroke</param>
    /// <returns></returns>
    int IndexOf(SharedPointer<Stroke> const & stroke);

    /// <summary>
    /// Remove a set of Stroke objects to the collection
    /// </summary>
    /// <param name="strokes">The strokes to remove from the collection</param>
    /// <remarks>Changes to the collection trigger a StrokesChanged event.</remarks>
    void Remove(SharedPointer<StrokeCollection> strokes);

    /// <summary>
    /// Add a set of Stroke objects to the collection
    /// </summary>
    /// <param name="strokes">The strokes to add to the collection</param>
    /// <remarks>The items are added to the collection at the end of the list.
    /// If the item already exists in the collection, then the item is not added again.</remarks>
    void Add(SharedPointer<StrokeCollection> strokes);

    using Collection::Add;

    bool Remove(SharedPointer<Stroke> const & stroke);

    /// <summary>
    /// Replace
    /// </summary>
    /// <param name="strokeToReplace"></param>
    /// <param name="strokesToReplaceWith"></param>
    void Replace(SharedPointer<Stroke> strokeToReplace, SharedPointer<StrokeCollection> strokesToReplaceWith);

    /// <summary>
    /// Replace
    /// </summary>
    /// <param name="strokesToReplace"></param>
    /// <param name="strokesToReplaceWith"></param>
    void Replace(SharedPointer<StrokeCollection> strokesToReplace, SharedPointer<StrokeCollection> strokesToReplaceWith);

    /// <summary>
    /// called by StrokeCollectionSerializer during Load, bypasses Change notification
    /// </summary>
    void AddWithoutEvent(SharedPointer<Stroke>stroke);


    /// <summary>Collection of extended properties on this StrokeCollection</summary>
    ExtendedPropertyCollection& ExtendedProperties();

    ExtendedPropertyCollection const & ExtendedProperties() const;

#ifdef INKCANVAS_QT
    operator QVariantList();

signals:
    /// <summary>
    /// Event that notifies listeners whenever a change occurs in the set
    /// of stroke objects contained in the collection.
    /// </summary>
    /// <value>StrokeCollectionChangedEventHandler</value>
    void StrokesChanged(StrokeCollectionChangedEventArgs& e);

    /// <summary>
    /// Event that notifies internal listeners whenever a change occurs in the set
    /// of stroke objects contained in the collection.
    /// </summary>
    /// <value>StrokeCollectionChangedEventHandler</value>
    void StrokesChangedInternal(StrokeCollectionChangedEventArgs& e);

    /// <summary>
    /// Event that notifies listeners whenever a change occurs in the propertyData
    /// </summary>
    /// <value>PropertyDataChangedEventHandler</value>
    void PropertyDataChanged(Guid const & propName);

    /// <summary>
    /// INotifyPropertyChanged.PropertyChanged event, explicitly implemented
    /// </summary>
    void PropertyChanged(QByteArray const & propName);

    /// <summary>
    /// INotifyCollectionChanged.CollectionChanged event, explicitly implemented
    /// </summary>
    void CollectionChanged(StrokeCollectionChangedEventArgs& e);
#endif

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
    virtual void OnStrokesChanged(StrokeCollectionChangedEventArgs& e);

    /// <summary>
    /// Method called on derived classes whenever a change occurs in
    /// the PropertyData.
    /// </summary>
    /// <remarks>Derived classes should call this method (their base class)
    /// to ensure that event listeners are notified</remarks>
    virtual void OnPropertyDataChanged(Guid const & propName);

    /// <summary>
    /// Method called when a property change occurs to the StrokeCollection
    /// </summary>
    /// <param name="e">The EventArgs specifying the name of the changed property.</param>
    /// <remarks>To follow the Guid const &elines, this method should take a PropertyChangedEventArgs
    /// instance, but every other INotifyPropertyChanged implementation follows this pattern.</remarks>
    virtual void OnPropertyChanged(char const * propName);

    /// <summary>
    /// Private helper that starts searching for stroke at index,
    /// but will loop around before reporting -1.  This is used for
    /// Stroke.Remove(StrokeCollection).  For example, if we're removing
    /// strokes, chances are they are in contiguous order.  If so, calling
    /// IndexOf to validate each stroke is O(n2).  If the strokes are in order
    /// this produces closer to O(n), if they are not in order, it is no worse
    /// </summary>
private:
    int OptimisticIndexOf(int startingIndex, SharedPointer<Stroke> stroke);

    /// <summary>
    /// Private helper that returns an array of indexes where the specified
    /// strokes exist in this stroke collection.  Returns null if at least one is not found.
    ///
    /// The indexes are sorted from smallest to largest
    /// </summary>
    /// <returns></returns>
    Array<int> GetStrokeIndexes(SharedPointer<StrokeCollection> strokes);

    // This function will invoke OnStrokesChanged method.
    //      addedStrokes    -   the collection which contains the added strokes during the previous op.
    //      removedStrokes  -   the collection which contains the removed strokes during the previous op.
    void RaiseStrokesChanged(SharedPointer<StrokeCollection> addedStrokes, SharedPointer<StrokeCollection> removedStrokes, int index);

#ifdef INKCANVAS_QT
signals:
    // The private PropertyChanged event
    void _propertyChanged(QByteArray const & propName);

    // private CollectionChanged event raiser
    void _collectionChanged(NotifyCollectionChangedEventArgs& e);    // Custom 'user-defined' attributes assigned to this collection
#endif

public:
    /// <summary>
    /// Calculates the combined bounds of all strokes in the collection
    /// </summary>
    /// <returns></returns>
    Rect GetBounds();

    // ISSUE-2004/12/13-XIAOTU: In M8.2, the following two tap-hit APIs return the top-hit stroke,
    // giving preference to non-highlighter strokes. We have decided not to treat highlighter and
    // non-highlighter differently and only return the top-hit stroke. But there are two remaining
    // open-issues on this:
    //  1. Do we need to make these two APIs virtual, so user can treat highlighter differently if they
    //     want to?
    //  2. Since we are only returning the top-hit stroke, should we use Stroke as the return type?
    //

#if STROKE_COLLECTION_EDIT_MASK
    void SetEditMask(const QPolygonF &clipShape);

    ErasingStroke * GetEditMask();
#endif

    /// <summary>
    /// Tap-hit. Hit tests all strokes within a point, and returns a StrokeCollection for these strokes.Internally does Stroke.HitTest(Point, 1pxlRectShape).
    /// </summary>
    /// <returns>A StrokeCollection that either empty or contains the top hit stroke</returns>
    SharedPointer<StrokeCollection> HitTest(Point const & point);

    /// <summary>
    /// Tap-hit
    /// </summary>
    /// <param name="point">The central point</param>
    /// <param name="diameter">The diameter value of the circle</param>
    /// <returns>A StrokeCollection that either empty or contains the top hit stroke</returns>
    SharedPointer<StrokeCollection> HitTest(Point const & point, double diameter);

    /// <summary>
    /// Hit-testing with lasso
    /// </summary>
    /// <param name="lassoPoints">points making the lasso</param>
    /// <param name="percentageWithinLasso">the margin value to tell whether a stroke
    /// is in or outside of the rect</param>
    /// <returns>collection of strokes found inside the rectangle</returns>
    SharedPointer<StrokeCollection> HitTest(List<Point> const & lassoPoints, int percentageWithinLasso);


    /// <summary>
    /// Hit-testing with rectangle
    /// </summary>
    /// <param name="bounds">hitting rectangle</param>
    /// <param name="percentageWithinBounds">the percentage of the stroke that must be within
    /// the bounds to be considered hit</param>
    /// <returns>collection of strokes found inside the rectangle</returns>
    SharedPointer<StrokeCollection> HitTest(Rect const & bounds, int percentageWithinBounds);


    /// <summary>
    /// Issue: what's the return value
    /// </summary>
    /// <param name="path"></param>
    /// <param name="stylusShape"></param>
    /// <returns></returns>
    SharedPointer<StrokeCollection> HitTest(List<Point> const & path, StylusShape& stylusShape);

    /// <summary>
    /// Clips out all ink outside a given lasso
    /// </summary>
    /// <param name="lassoPoints">lasso</param>
    void Clip(List<Point> const & lassoPoints);

    /// <summary>
    /// Clips out all ink outside a given rectangle.
    /// </summary>
    /// <param name="bounds">rectangle to clip with</param>
    void Clip(Rect const & bounds);

    /// <summary>
    /// Erases all ink inside a lasso
    /// </summary>
    /// <param name="lassoPoints">lasso to erase within</param>
    void Erase(List<Point> const & lassoPoints);


    /// <summary>
    /// Erases all ink inside a given rectangle
    /// </summary>
    /// <param name="bounds">rectangle to erase within</param>
    void Erase(Rect const & bounds);


    /// <summary>
    /// Erases all ink hit by the contour of an erasing stroke
    /// </summary>
    /// <param name="eraserShape">Shape of the eraser</param>
    /// <param name="eraserPath">a path making the spine of the erasing stroke </param>
    void Erase(List<Point> const & eraserPath, StylusShape& eraserShape);

    /// <summary>
    /// Render the StrokeCollection under the specified DrawingContext.
    /// </summary>
    /// <param name="context"></param>
    void Draw(DrawingContext& context);

    //#endregion

    //#region Incremental hit-testing

    /// <summary>
    /// Creates an incremental hit-tester for hit-testing with a shape.
    /// Scenarios: stroke-erasing and point-erasing
    /// </summary>
    /// <param name="eraserShape">shape of the eraser</param>
    /// <returns>an instance of IncrementalStrokeHitTester</returns>
    IncrementalStrokeHitTester* GetIncrementalStrokeHitTester(StylusShape& eraserShape);


    /// <summary>
    /// Creates an incremental hit-tester for selecting with lasso.
    /// </summary>
    /// <param name="percentageWithinLasso">The percentage of the stroke that must be within the lasso to be considered hit</param>
    /// <returns>an instance of incremental hit-tester</returns>
    IncrementalLassoHitTester* GetIncrementalLassoHitTester(int percentageWithinLasso);
    //#endregion

private:
    /// <summary>
    /// Return all hit strokes that the StylusShape intersects and returns them in a StrokeCollection
    /// </summary>
    SharedPointer<StrokeCollection> PointHitTest(Point const & point, StylusShape& shape);

    void UpdateStrokeCollection(SharedPointer<Stroke> original, SharedPointer<StrokeCollection> toReplace, int& index);

private:
    //  In v1, these were called Ink.ExtendedProperties
    ExtendedPropertyCollection* _extendedProperties = nullptr;
#if STROKE_COLLECTION_EDIT_MASK
    QPolygonF maskShape_;
    ErasingStroke * mask_ = nullptr;
#endif

    /// <summary>
    /// Constants for the PropertyChanged event
    /// </summary>
    static constexpr char const * IndexerName = "Item[]";
    static constexpr char const * CountName = "Count";

    //
    // Nested types...
    //
};

INKCANVAS_END_NAMESPACE

#ifdef INKCANVAS_QT

#include "Windows/dependencyproperty.h"

INKCANVAS_BEGIN_NAMESPACE

class StrokeCollectionDefaultValueFactory : public DefaultValueFactory
{
private:
    virtual Variant DefaultValue() override
    {
        return Variant::fromValue(SharedPointer<StrokeCollection>(new StrokeCollection));
    }
};

INKCANVAS_END_NAMESPACE

#endif

#endif // STROKECOLLECTION_H
