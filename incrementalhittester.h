#ifndef INCREMENTALHITTESTER_H
#define INCREMENTALHITTESTER_H

#include "strokecollection.h"
#include "styluspointcollection.h"
#include "erasingstroke.h"
#include "events.h"
#include "strokeintersection.h"

#include <QPointF>
#include <QList>

class StylusPointCollection;
class StrokeInfo;
class StrokeIntersection;
class StrokeCollectionChangedEventArgs;
class StylusShape;

class IncrementalHitTester : public QObject
{
    Q_OBJECT
public:
    IncrementalHitTester();

    virtual ~IncrementalHitTester();

    /// <summary>
    /// Adds a point representing an incremental move of the hit-testing tool
    /// </summary>
    /// <param name="point">a point that represents an incremental move of the hitting tool</param>
    void AddPoint(QPointF const & point)
    {
        AddPoints({ point });
    }

    /// <summary>
    /// Adds an array of points representing an incremental move of the hit-testing tool
    /// </summary>
    /// <param name="points">points representing an incremental move of the hitting tool</param>
    void AddPoints(QVector<QPointF> const & points)
    {
        //if (points == nullptr)
        //{
        //    throw new System.ArgumentNullException("points");
        //}

        if (points.size() == 0)
        {
            throw std::exception("points");
        }

        if (false == _fValid)
        {
            throw std::exception("SR.Get(SRID.EndHitTestingCalled");
        }

        //System.Diagnostics.Debug.Assert(_strokes != nullptr);

        AddPointsCore(points);
    }

    /// <summary>
    /// Adds a StylusPacket representing an incremental move of the hit-testing tool
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    void AddPoints(StylusPointCollection const & stylusPoints);


    /// <summary>
    /// Release as many resources as possible for this enumerator
    /// </summary>
    void EndHitTesting();

    /// <summary>
    /// Accessor to see if the Hit Tester is still valid
    /// </summary>
    bool IsValid() { return _fValid; }


    /// <summary>
    /// C-tor.
    /// </summary>
    /// <param name="strokes">strokes to hit-test</param>
    IncrementalHitTester(QSharedPointer<StrokeCollection> strokes);

    /// <summary>
    /// The implementation behind AddPoint/AddPoints.
    /// Derived classes are supposed to override this method.
    /// </summary>
    virtual void AddPointsCore(QVector<QPointF> const & points) = 0;


    /// <summary>
    /// Accessor to the internal collection of StrokeInfo objects
    /// </summary>
    QList<StrokeInfo*> & StrokeInfos() { return _strokeInfos; }

private slots:
    /// <summary>
    /// Event handler associated with the stroke collection.
    /// </summary>
    /// <param name="sender">Stroke collection that was modified</param>
    /// <param name="args">Modification that occurred</param>
    /// <remarks>
    /// Update our _strokeInfos cache.  We get notified on StrokeCollection.StrokesChangedInternal which
    /// is raised first so we can assume we're the first delegate in the call chain
    /// </remarks>
    void OnStrokesChanged(StrokeCollectionChangedEventArgs& args);

private:
    /// <summary>
    /// IHT's can get into a state where their StrokeInfo cache is too
    /// out of sync with the StrokeCollection to incrementally update it.
    /// When we detect this has happened, we just rebuild the entire cache.
    /// </summary>
    void RebuildStrokeInfoCache();


    /// <summary> Reference to the stroke collection under test </summary>
private:
    QSharedPointer<StrokeCollection> _strokes;
    /// <summary> A collection of helper objects mapped to the stroke colection</summary>
    QList<StrokeInfo*> _strokeInfos;

    bool _fValid = true;

};

class LassoSelectionChangedEventArgs;
class Lasso;

/// <summary>
/// IncrementalHitTester implementation for hit-testing with lasso
/// </summary>
class IncrementalLassoHitTester : public IncrementalHitTester
{
    Q_OBJECT
    /// Event
    /// </summary>
signals:
    void SelectionChanged(LassoSelectionChangedEventArgs& e);

public:
    /// <summary>
    /// C-tor.
    /// </summary>
    /// <param name="strokes">strokes to hit-test</param>
    /// <param name="percentageWithinLasso">a hit-testing parameter that defines the minimal
    /// percent of nodes of a stroke to be inside the lasso to consider the stroke hit</param>
    IncrementalLassoHitTester(QSharedPointer<StrokeCollection> strokes, int percentageWithinLasso);

    /// <summary>
    /// The implementation behind the public methods AddPoint/AddPoints
    /// </summary>
    /// <param name="points">new points to add to the lasso</param>
    virtual void AddPointsCore(QVector<QPointF> const & points);

    /// <summary>
    /// SelectionChanged event raiser
    /// </summary>
    /// <param name="eventArgs"></param>
    void OnSelectionChanged(LassoSelectionChangedEventArgs & eventArgs);


private:
    Lasso *  _lasso;
    int     _percentIntersect;
};

class StrokeHitEventArgs;

/// <summary>
/// IncrementalHitTester implementation for hit-testing with a shape, PointErasing .
/// </summary>
class IncrementalStrokeHitTester : public IncrementalHitTester
{
    Q_OBJECT
    /// <summary>
    ///
    /// </summary>
signals:
    void StrokeHit(StrokeHitEventArgs& e);

public:
    /// <summary>
    /// C-tor
    /// </summary>
    /// <param name="strokes">strokes to hit-test for erasing</param>
    /// <param name="eraserShape">erasing shape</param>
    IncrementalStrokeHitTester(QSharedPointer<StrokeCollection>strokes, StylusShape& eraserShape);

    /// <summary>
    /// The implementation behind the public methods AddPoint/AddPoints
    /// </summary>
    /// <param name="points">a set of points representing the last increment
    /// in the moving of the erasing shape</param>
    virtual void AddPointsCore(QVector<QPointF> const & points);

    /// <summary>
    /// Event raiser for StrokeHit
    /// </summary>
    void OnStrokeHit(StrokeHitEventArgs& eventArgs);

private:
    ErasingStroke _erasingStroke;

};


/// <summary>
/// Declaration for LassoSelectionChanged event handler. Used in lasso-selection
/// </summary>
//public delegate void LassoSelectionChangedEventHandler(object sender, LassoSelectionChangedEventArgs e);


/// <summary>
/// Declaration for StrokeHit event handler. Used in point-erasing
/// </summary>
//public delegate void StrokeHitEventHandler(object sender, StrokeHitEventArgs e);


/// <summary>
/// Event arguments for LassoSelectionChanged event
/// </summary>
class LassoSelectionChangedEventArgs
{
public:
    LassoSelectionChangedEventArgs(QSharedPointer<StrokeCollection> selectedStrokes, QSharedPointer<StrokeCollection> deselectedStrokes)
        : _selectedStrokes(selectedStrokes)
        , _deselectedStrokes(deselectedStrokes)
    {
        //_selectedStrokes = selectedStrokes;
        //_deselectedStrokes = deselectedStrokes;
    }

    /// <summary>
    /// Collection of strokes which were hit with the last increment
    /// </summary>
    QSharedPointer<StrokeCollection> SelectedStrokes()
    {
        if (_selectedStrokes != nullptr)
        {
            QSharedPointer<StrokeCollection> sc;
            sc->append(*_selectedStrokes);
            return sc;
        }
        else
        {
            return  QSharedPointer<StrokeCollection>();
        }
    }

    /// <summary>
    /// Collection of strokes which were unhit with the last increment
    /// </summary>
    QSharedPointer<StrokeCollection> DeselectedStrokes()
    {
        if (_deselectedStrokes != nullptr)
        {
            QSharedPointer<StrokeCollection> sc;
            sc->append(*_deselectedStrokes);
            return sc;
        }
        else
        {
            return QSharedPointer<StrokeCollection>();
        }
    }

private:
    QSharedPointer<StrokeCollection> _selectedStrokes;
    QSharedPointer<StrokeCollection> _deselectedStrokes;
};

/// <summary>
/// Event arguments for StrokeHit event
/// </summary>
class StrokeHitEventArgs
{
public:
    /// <summary>
    /// C-tor
    /// </summary>
    StrokeHitEventArgs(QSharedPointer<Stroke> stroke, QVector<StrokeIntersection> const & hitFragments)
        : _stroke(stroke)
        , _hitFragments(hitFragments)
    {
        //System.Diagnostics.Debug.Assert(stroke != null && hitFragments != null && hitFragments.Length > 0);
        //_stroke = stroke;
        //_hitFragments = hitFragments;
    }

    /// <summary>Stroke that was hit</summary>
    QSharedPointer<Stroke> HitStroke() { return _stroke; }

    /// <summary>
    ///
    /// </summary>
    /// <returns></returns>
    QSharedPointer<StrokeCollection> GetPointEraseResults();

private:
    QSharedPointer<Stroke>         _stroke;
    QVector<StrokeIntersection>    _hitFragments;
};

class StylusPointsReplacedEventArgs;
class PropertyDataChangedEventArgs;
class DrawingAttributesReplacedEventArgs;

/// <summary>
/// A helper class associated with a stroke. Used for caching the stroke's
/// bounding box, hit-testing results, and for keeping an eye on the stroke changes
/// </summary>
class StrokeInfo : public QObject
{
    Q_OBJECT
public:
    /// <summary>
    /// StrokeInfo
    /// </summary>
    StrokeInfo(QSharedPointer<Stroke> stroke);

    ~StrokeInfo() { Detach(); }

    /// <summary>The stroke object associated with this helper structure</summary>
    QSharedPointer<Stroke> GetStroke() { return _stroke; }

    /// <summary>Pre-calculated bounds of the stroke </summary>
    QRectF & StrokeBounds() { return _bounds; }

    /// <summary>Tells whether the stroke or its drawing attributes have been modified
    /// since the last use (hit-testing)</summary>
    bool IsDirty()
    {
        return _isDirty;
    }
    void SetIsDirty(bool value)
    {
        _isDirty = value;
    }

    /// <summary>Tells whether the stroke was found (and reported) as hit </summary>
    bool IsHit()
    {
        return _isHit;
    }
    void SetIsHit(bool value)
    {
        _isHit = value;
    }

    /// <summary>
    /// Cache teh stroke points
    /// </summary>
    QSharedPointer<StylusPointCollection> StylusPoints();

    /// <summary>
    /// Holds the current hit-testing result for the stroke. Represents the length of
    /// the stroke "inside" and "hit" by the lasso
    /// </summary>
    double HitWeight()
    {
        return _hitWeight;
    }
    void SetHitWeight(double value);

    /// <summary>
    /// Get the total weight of the stroke. For this implementation, it is the total length of the stroke.
    /// </summary>
    /// <returns></returns>
    double TotalWeight();

    /// <summary>
    /// Calculate the weight of a point.
    /// </summary>
    double GetPointWeight(int index);

    /// <summary>
    /// A kind of disposing method
    /// </summary>
    void Detach();

private slots:
    /// <summary>Event handler for stroke data changed events</summary>
    void OnStylusPointsChanged();

    /// <summary>Event handler for stroke data changed events</summary>
    void OnStylusPointsReplaced(StylusPointsReplacedEventArgs& args);

    /// <summary>
    /// Event handler for stroke's drawing attributes changes.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    void OnStrokeDrawingAttributesChanged(PropertyDataChangedEventArgs args);

    void OnDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& args);

    /// <summary>Implementation for the event handlers above</summary>
    void Invalidate();

private:
    QSharedPointer<Stroke>      _stroke;
    QRectF                      _bounds;
    double                      _hitWeight = 0;
    bool                        _isHit = false;
    bool                        _isDirty = true;
    QSharedPointer<StylusPointCollection>     _stylusPoints;   // Cache the stroke rendering points
    double                      _totalWeight = 0;
    bool                        _totalWeightCached = false;
};

#endif // INCREMENTALHITTESTER_H
