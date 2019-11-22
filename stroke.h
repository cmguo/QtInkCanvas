#ifndef STROKE_H
#define STROKE_H

#include "drawingattributes.h"
#include "styluspointcollection.h"

#include <QList>
#include <QMap>
#include <QObject>

class Geometry;
class StrokeFIndices;
class DrawingContext;
class StrokeCollection;
class StrokeIntersection;
class Lasso;
class StylusPointsReplacedEventArgs;
class DrawingAttributesReplacedEventArgs;

class Stroke : public QObject, public QEnableSharedFromThis<Stroke>
{
    Q_OBJECT
public:
    Stroke();

    /// <summary>Create a stroke from a StylusPointCollection</summary>
    /// <remarks>
    /// </remarks>
    /// <param name="stylusPoints">StylusPointCollection that makes up the stroke</param>
    Stroke(QSharedPointer<StylusPointCollection> stylusPoints);

    /// <summary>Create a stroke from a StylusPointCollection</summary>
    /// <remarks>
    /// </remarks>
    /// <param name="stylusPoints">StylusPointCollection that makes up the stroke</param>
    /// <param name="drawingAttributes">drawingAttributes</param>
    Stroke(QSharedPointer<StylusPointCollection> stylusPoints, QSharedPointer<DrawingAttributes> drawingAttributes);

    /// <summary>Create a stroke from a StylusPointCollection</summary>
    /// <remarks>
    /// </remarks>
    /// <param name="stylusPoints">StylusPointCollection that makes up the stroke</param>
    /// <param name="drawingAttributes">drawingAttributes</param>
    /// <param name="extendedProperties">extendedProperties</param>
    Stroke(QSharedPointer<StylusPointCollection> stylusPoints, QSharedPointer<DrawingAttributes> drawingAttributes, QVariantMap * extendedProperties);

    Stroke(Stroke const & o);

    virtual ~Stroke();

public:
    /// <summary>
    /// helper to set up listeners, called by ctor and by Clone
    /// </summary>
    void Initialize();

    /// <summary>Returns a new stroke that has a deep copy.</summary>
    /// <remarks>Deep copied data includes points, point description, drawing attributes, and transform</remarks>
    /// <returns>Deep copy of current stroke</returns>
    virtual QSharedPointer<Stroke> Clone() { return QSharedPointer<Stroke>(new Stroke(*this)); }

    /// <summary>Transforms the ink and also changes the StylusTip</summary>
    /// <param name="transformMatrix">Matrix to transform the stroke by</param>
    /// <param name="applyToStylusTip">Boolean if true the transform matrix will be applied to StylusTip</param>
    virtual void Transform(QMatrix transformMatrix, bool applyToStylusTip);

    /// <summary>
    /// Returns a Bezier smoothed version of the StylusPoints
    /// </summary>
    /// <returns></returns>
    QSharedPointer<StylusPointCollection> GetBezierStylusPoints();

    /// <summary>
    /// Interpolate packet / pressure data from _stylusPoints
    /// </summary>
    QSharedPointer<StylusPointCollection> GetInterpolatedStylusPoints(QList<QPointF> & bezierPoints);

    /// <summary>
    /// helper used to get the length between two points
    /// </summary>
    double GetDistanceBetweenPoints(QPointF const & p1, QPointF const & p2);

    /// <summary>
    /// helper for adding a StylusPoint to the BezierStylusPoints
    /// </summary>
    void AddInterpolatedBezierPoint(StylusPointCollection & bezierStylusPoints,
                                            QPointF & bezierPoint,
                                            QVector<int> const & additionalData,
                                            float pressure);

    /// <summary>
    /// Allows addition of objects to the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    /// <param name="propertyData"></param>
    void AddPropertyData(QString propertyDataId, QVariant propertyData);


    /// <summary>
    /// Allows removal of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    void RemovePropertyData(QString propertyDataId);

    /// <summary>
    /// Allows retrieval of objects from the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    QVariant GetPropertyData(QString propertyDataId);

    /// <summary>
    /// Allows retrieval of a Array of guids that are contained in the EPC
    /// </summary>
    QVector<QString> GetPropertyDataIds();

    /// <summary>
    /// Allows the checking of objects in the EPC
    /// </summary>
    /// <param name="propertyDataId"></param>
    bool ContainsPropertyData(QString propertyDataId);

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
    QSharedPointer<DrawingAttributes> GetDrawingAttributes()
    {
        return _drawingAttributes;
    }

    void SetDrawingAttributes(QSharedPointer<DrawingAttributes> value);

    /// <summary>
    /// StylusPoints
    /// </summary>
    QSharedPointer<StylusPointCollection> StylusPoints()
    {
        return _stylusPoints;
    }

    void SetStylusPoints(QSharedPointer<StylusPointCollection> value);

signals:
    /// <summary>Event that is fired when a drawing attribute is changed.</summary>
    /// <value>The event listener to add or remove in the listener chain</value>
    void DrawingAttributesChanged(PropertyDataChangedEventArgs& e);

    /// <summary>
    /// Event that is fired when the DrawingAttributes have been replaced
    /// </summary>
    void DrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& e);

    /// <summary>
    /// Notifies listeners whenever the StylusPoints have been replaced
    /// </summary>
    void StylusPointsReplaced(StylusPointsReplacedEventArgs& e);

    /// <summary>
    /// Notifies listeners whenever the StylusPoints have been changed
    /// </summary>
    void StylusPointsChanged();

    /// <summary>
    /// Notifies listeners whenever a change occurs in the propertyData
    /// </summary>
    /// <value>PropertyDataChangedEventHandler</value>
    void PropertyDataChanged(PropertyDataChangedEventArgs& e);


    /// <summary>
    /// Stroke would raise this event for PacketsChanged, DrawingAttributeChanged, or DrawingAttributeReplaced.
    /// Renderer would simply listen to this. Stroke developer can raise this event by calling OnInvalidated when
    /// he wants the renderer to repaint.
    /// </summary>
    void Invalidated();

    /// <summary>
    /// INotifyPropertyChanged.PropertyChanged event, explicitly implemented
    /// </summary>
    void PropertyChanged(QByteArray const & propName);

protected:
    virtual void OnDrawingAttributesChanged(PropertyDataChangedEventArgs& e)
    {
        emit DrawingAttributesChanged(e);
    }

    /// <summary>
    /// virtual version for developers deriving from InkCanvas.
    /// This method is what actually throws the event.
    /// </summary>
    /// <param name="e">DrawingAttributesReplacedEventArgs to raise the event with</param>
    virtual void OnDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& e)
    {
        emit DrawingAttributesReplaced(e);
    }

    /// <summary>
    /// Method called on derived classes whenever the StylusPoints are replaced
    /// </summary>
    /// <param name="e">EventArgs</param>
    virtual void OnStylusPointsReplaced(StylusPointsReplacedEventArgs& e)
    {
        emit StylusPointsReplaced(e);
    }

    /// <summary>
    /// Method called on derived classes whenever the StylusPoints are changed
    /// </summary>
    /// <param name="e">EventArgs</param>
    virtual void OnStylusPointsChanged()
    {
    }

    /// <summary>
    /// Method called on derived classes whenever a change occurs in
    /// the PropertyData.
    /// </summary>
    /// <remarks>Derived classes should call this method (their base class)
    /// to ensure that event listeners are notified</remarks>
    virtual void OnPropertyDataChanged(PropertyDataChangedEventArgs& e)
    {
        emit PropertyDataChanged(e);
    }


    /// <summary>
    /// Method called on derived classes whenever a stroke needs repaint. Developers who
    /// subclass Stroke and need a repaint could raise Invalidated through this virtual
    /// </summary>
    virtual void OnInvalidated()
    {
        emit Invalidated();
    }

    /// <summary>
    /// Method called when a property change occurs to the Stroke
    /// </summary>
    /// <param name="e">The EventArgs specifying the name of the changed property.</param>
    /// <remarks>To follow the guidelines, this method should take a PropertyChangedEventArgs
    /// instance, but every other INotifyPropertyChanged implementation follows this pattern.</remarks>
    virtual void OnPropertyChanged(QByteArray const & propName)
    {
        emit PropertyChanged(propName);
    }


    /// <summary>
    /// ExtendedProperties
    /// </summary>
    QVariantMap * ExtendedProperties()
    {
        if (_extendedProperties == nullptr)
        {
            _extendedProperties = new QVariantMap;
        }

        return _extendedProperties;
    }


    /// <summary>
    /// Clip
    /// </summary>
    /// <param name="cutAt">Fragment markers for clipping</param>
    QSharedPointer<StrokeCollection> Clip(QVector<StrokeFIndices> cutAt);

    /// <summary>
    ///
    /// </summary>
    /// <param name="cutAt">Fragment markers for clipping</param>
    /// <returns>Survived fragments of current Stroke as a StrokeCollection</returns>
    QSharedPointer<StrokeCollection> Erase(QVector<StrokeFIndices> cutAt);


    /// <summary>
    /// Creates a new stroke from a subset of the points
    /// </summary>
    QSharedPointer<Stroke> Copy(QSharedPointer<StylusPointCollection> sourceStylusPoints, double beginFIndex, double endFIndex);

    /// <summary>
    /// helper that will generate a new point between two points at an findex
    /// </summary>
    QPointF GetIntermediatePoint(StylusPoint const & p1, StylusPoint const & p2, double findex);


#ifdef DEBUG
    /// <summary>
    /// Helper method used to validate that the strokefindices in the array
    /// are sorted and there are no overlaps
    /// </summary>
    /// <param name="fragments">fragments</param>
    void AssertSortedNoOverlap(StrokeFIndices[] fragments);

    bool IsValidStrokeFIndices(StrokeFIndices findex);

#endif


    /// <summary>
    /// Method called whenever the Stroke's drawing attributes are changed.
    /// This method will trigger an event for any listeners interested in
    /// drawing attributes.
    /// </summary>
    /// <param name="sender">The Drawing Attributes object that was changed</param>
    /// <param name="e">More data about the change that occurred</param>
    void DrawingAttributes_Changed(PropertyDataChangedEventArgs& e);

    /// <summary>
    /// Method called whenever the Stroke's StylusPoints are changed.
    /// This method will trigger an event for any listeners interested in
    /// Invalidate
    /// </summary>
    /// <param name="sender">The StylusPoints object that was changed</param>
    /// <param name="e">event args</param>
    void StylusPoints_Changed();

    /// <summary>
    /// Computes the bounds of the stroke in the default rendering context
    /// </summary>
    /// <returns></returns>
public:
    virtual QRectF GetBounds();

    /// <summary>
    /// Render the Stroke under the specified DrawingContext. The draw method is a
    /// batch operationg that uses the rendering methods exposed off of DrawingContext
    /// </summary>
    /// <param name="context"></param>
    void Draw(DrawingContext & context);


    /// <summary>
    /// Render the StrokeCollection under the specified DrawingContext. This draw method uses the
    /// passing in drawing attribute to override that on the stroke.
    /// </summary>
    /// <param name="drawingContext"></param>
    /// <param name="drawingAttributes"></param>
    void Draw(DrawingContext & drawingContext, QSharedPointer<DrawingAttributes> drawingAttributes);


    /// <summary>
    /// Clip with rect. Calculate the after-clipping Strokes. Only the "in-segments" are left after this operation.
    /// </summary>
    /// <param name="bounds">A Rect to clip with</param>
    /// <returns>The after-clipping strokes.</returns>
    QSharedPointer<StrokeCollection> GetClipResult(QRectF const & bounds);


    /// <summary>
    /// Clip with lasso. Calculate the after-clipping Strokes. Only the "in-segments" are left after this operation.
    /// </summary>
    /// <param name="lassoPoints">The lasso points to clip with</param>
    /// <returns>The after-clipping strokes</returns>
    QSharedPointer<StrokeCollection> GetClipResult(QVector<QPointF> const & lassoPoints);


    /// <summary>
    /// Erase with a rect. Calculate the after-erasing Strokes. Only the "out-segments" are left after this operation.
    /// </summary>
    /// <param name="bounds">A Rect to clip with</param>
    /// <returns>The after-erasing strokes</returns>
    QSharedPointer<StrokeCollection> GetEraseResult(QRectF const & bounds);

    /// <summary>
    /// Erase with lasso points.
    /// </summary>
    /// <param name="lassoPoints">Lasso points to erase with</param>
    /// <returns>The after-erasing strokes</returns>
    QSharedPointer<StrokeCollection> GetEraseResult(QVector<QPointF> const & lassoPoints);

    /// <summary>
    /// Erase with an eraser with passed in shape
    /// </summary>
    /// <param name="eraserPath">The path to erase</param>
    /// <param name="eraserShape">Shape of the eraser</param>
    /// <returns></returns>
    QSharedPointer<StrokeCollection> GetEraseResult(QVector<QPointF> const & eraserPath, StylusShape& eraserShape);


    /// <summary>
    /// Tap-hit. Hit tests with a point. Internally does Stroke.HitTest(Point, 1pxlRectShape).
    /// </summary>
    /// <param name="point">The location to do the hitest</param>
    /// <returns>True is this stroke is hit, false otherwise</returns>
    bool HitTest(QPointF const & point);

    /// <summary>
    /// Tap-hit. Hit tests with a point.
    /// </summary>
    /// <param name="point">The location to do the hittest</param>
    /// <param name="diameter">diameter of the tip</param>
    /// <returns>true if hit, false otherwise</returns>
    bool HitTest(QPointF const & point, double diameter);

    /// <summary>
    /// Check whether a certain percentage of the stroke is within the Rect passed in.
    /// </summary>
    /// <param name="bounds"></param>
    /// <param name="percentageWithinBounds"></param>
    /// <returns></returns>
    bool HitTest(QRectF const & bounds, int percentageWithinBounds);

    /// <summary>
    /// Check whether a certain percentage of the stroke is within the lasso
    /// </summary>
    /// <param name="lassoPoints"></param>
    /// <param name="percentageWithinLasso"></param>
    /// <returns></returns>
    bool HitTest(QVector<QPointF> const & lassoPoints, int percentageWithinLasso);
    /// <summary>
    ///
    /// </summary>
    /// <param name="path"></param>
    /// <param name="stylusShape"></param>
    /// <returns></returns>
    bool HitTest(QVector<QPointF> const & path, StylusShape& stylusShape);

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
protected:
    virtual void DrawCore(DrawingContext & drawingContext, QSharedPointer<DrawingAttributes>  drawingAttributes);

    /// <summary>
    /// Returns the Geometry of this stroke.
    /// </summary>
    /// <returns></returns>
    Geometry * GetGeometry();

    /// <summary>
    /// Get the Geometry of the Stroke
    /// </summary>
    /// <param name="drawingAttributes"></param>
    /// <returns></returns>
    Geometry * GetGeometry(QSharedPointer<DrawingAttributes> drawingAttributes);

    /// <summary>
    /// our code - StrokeVisual.OnRender and StrokeCollection.Draw - always calls this
    /// so we can assume the correct opacity has already been pushed on dc. The flag drawAsHollow is set
    /// to true when this function is called from Renderer and this.IsSelected == true.
    /// </summary>
    void DrawInternal(DrawingContext & dc, QSharedPointer<DrawingAttributes> DrawingAttributes, bool drawAsHollow);

public:
    /// <summary>
    /// Used by Inkcanvas to draw selected stroke as hollow.
    /// </summary>
    bool IsSelected()
    {
        return _isSelected;
    }
    void SetIsSelected(bool value)
    {
        if (value != _isSelected)
        {
            _isSelected = value;

            // Raise Invalidated event. This will cause Renderer to repaint and call back DrawCore
            OnInvalidated();
        }
    }

    /// <summary>
    /// Set the path geometry
    /// </summary>
    void SetGeometry(Geometry* geometry)
    {
        //System.Diagnostics.Debug.Assert(geometry != null);
        _cachedGeometry = geometry;
    }

    /// <summary>
    /// Set the bounds
    /// </summary>
    void SetBounds(QRectF const & newBounds)
    {
        //System.Diagnostics.Debug.Assert(newBounds.IsEmpty == false);
        _cachedBounds = newBounds;
    }

    /// <summary>Hit tests all segments within a contour generated with shape and path</summary>
    /// <param name="shape"></param>
    /// <param name="path"></param>
    /// <returns>StrokeIntersection array for these segments</returns>
    QVector<StrokeIntersection> EraseTest(QVector<QPointF> const &, StylusShape &shape);

    /// <summary>
    /// Hit tests all segments within the lasso loops
    /// </summary>
    /// <returns> a StrokeIntersection array for these segments</returns>
    QVector<StrokeIntersection> HitTest(Lasso & lasso);


    /// <summary>
    /// Calculate the after-erasing Strokes. Only the "out-segments" are left after this operation.
    /// </summary>
    /// <param name="cutAt">Array of intersections indicating the erasing locations</param>
    /// <returns></returns>
    QSharedPointer<StrokeCollection> Erase(QVector<StrokeIntersection> cutAt);

    /// <summary>
    /// Calculate the after-clipping Strokes. Only the "in-segments" are left after this operation.
    /// </summary>
    /// <param name="cutAt">Array of intersections indicating the clipping locations</param>
    /// <returns>The resulting StrokeCollection</returns>
    QSharedPointer<StrokeCollection> Clip(QVector<StrokeIntersection> cutAt);

    static constexpr double TapHitPointSize = 1.0;
    static constexpr double TapHitRotation = 0;


    /// <summary>
    /// Calculate the two transforms for two-pass rendering used to draw as hollow. The resulting outerTransform will make the
    /// first-pass-rendering 1 avalon-unit wider/heigher. The resulting innerTransform will make the second-pass-rendering 1 avalon-unit
    /// narrower/shorter.
    /// </summary>
private:
    static void CalcHollowTransforms(QSharedPointer<DrawingAttributes> originalDa, QMatrix & innerTransform, QMatrix & outerTransform);

private:
        // Custom attributes associated with this stroke
    QVariantMap * _extendedProperties = nullptr;

        // Drawing attributes associated with this stroke
    QSharedPointer<DrawingAttributes> _drawingAttributes;

    QSharedPointer<StylusPointCollection> _stylusPoints;

private:
    Geometry * _cachedGeometry     = nullptr;
    bool _isSelected         = false;
    bool _drawAsHollow       = false;
    bool _cloneStylusPoints  = true;
    bool _delayRaiseInvalidated  = false;
    static constexpr double  HollowLineSize      = 1.0;
    QRectF _cachedBounds;

    static constexpr char const * DrawingAttributesName = "DrawingAttributes";
    static constexpr char const * StylusPointsName = "StylusPoints";

public:
    static constexpr double PercentageTolerance = 0.0001;
};


//helper to determine if a matix contains invalid values
class MatrixHelper
{
public:
    //returns true if any member is NaN
    static bool ContainsNaN(QMatrix & matrix)
    {
        if (qIsNaN(matrix.m11()) ||
            qIsNaN(matrix.m12()) ||
            qIsNaN(matrix.m21()) ||
            qIsNaN(matrix.m22()) ||
            qIsNaN(matrix.dx()) ||
            qIsNaN(matrix.dy()))
        {
            return true;
        }
        return false;
    }

    //returns true if any member is negative or positive infinity
    static bool ContainsInfinity(QMatrix & matrix)
    {
        if (qIsInf(matrix.m11()) ||
            qIsInf(matrix.m12()) ||
            qIsInf(matrix.m21()) ||
            qIsInf(matrix.m22()) ||
            qIsInf(matrix.dx()) ||
            qIsInf(matrix.dy()))
        {
            return true;
        }
        return false;
    }
};

/*
/// <summary>
/// Helper for dealing with IEnumerable of Points
/// </summary>
class IEnumerablePointHelper
{
    /// <summary>
    /// Returns the count of an IEumerable of Points by trying to cast
    /// to an ICollection of Points
    /// </summary>
    static int GetCount(IEnumerable<QPointF> ienum)
    {
        Debug.Assert(ienum != null);
        ICollection<Point> icol = ienum as ICollection<Point>;
        if (icol != null)
        {
            return icol.Count;
        }
        int count = 0;
        foreach (Point point in ienum)
        {
            count++;
        }
        return count;
    }

    /// <summary>
    /// Returns a Point[] for a given IEnumerable of Points.
    /// </summary>
    static Point[] GetPointArray(IEnumerable<Point> ienum)
    {
        Debug.Assert(ienum != null);
        Point[] points = ienum as Point[];
        if (points != null)
        {
            return points;
        }

        //
        // fall back to creating an array
        //
        points = new Point[GetCount(ienum)];
        int index = 0;
        foreach (Point point in ienum)
        {
            points[index++] = point;
        }
        return points;
    }
};
*/

#endif // STROKE_H
