#include "Windows/Ink/incrementalhittester.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Input/styluspointcollection.h"
#include "Internal/Ink/lasso.h"
#include "Windows/Input/styluspoint.h"
#include "Windows/Ink/events.h"
#include "Internal/debug.h"

#include <QDebug>

IncrementalHitTester::IncrementalHitTester()
{

}

IncrementalHitTester::~IncrementalHitTester()
{
    for (auto i : _strokeInfos)
        delete i;
    _strokeInfos.clear();
}

/// <summary>
/// Adds a point representing an incremental move of the hit-testing tool
/// </summary>
/// <param name="point">a point that represents an incremental move of the hitting tool</param>
void IncrementalHitTester::AddPoint(QPointF const & point)
{
    AddPoints({ point });
}

/// <summary>
/// Adds an array of points representing an incremental move of the hit-testing tool
/// </summary>
/// <param name="points">points representing an incremental move of the hitting tool</param>
void IncrementalHitTester::AddPoints(QVector<QPointF> const & points)
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

    Debug::Assert(_strokes != nullptr);

    AddPointsCore(points);
}

/// <summary>
/// Adds a StylusPacket representing an incremental move of the hit-testing tool
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
void IncrementalHitTester::AddPoints(QSharedPointer<StylusPointCollection> const & stylusPoints)
{
    //if (stylusPoints == nullptr)
    //{
    //    throw new System.ArgumentNullException("stylusPoints");
    //}

    if (stylusPoints->size() == 0)
    {
        throw new std::exception("stylusPoints");
    }

    if (false == _fValid)
    {
        throw std::exception("SR.Get(SRID.EndHitTestingCalled)");
    }

    Debug::Assert(_strokes != nullptr);

    QVector<QPointF> points(stylusPoints->size());
    for (int x = 0; x < stylusPoints->size(); x++)
    {
        points[x] = (*stylusPoints)[x];
    }

    AddPointsCore(points);
}


/// <summary>
/// Release as many resources as possible for this enumerator
/// </summary>
void IncrementalHitTester::EndHitTesting()
{
    if (_strokes != nullptr)
    {
        // Detach the event handler
        //_strokes.StrokesChangedInternal -= new StrokeCollectionChangedEventHandler(OnStrokesChanged);
        QObject::disconnect(_strokes.get(), &StrokeCollection::StrokesChangedInternal,
                            this, &IncrementalHitTester::OnStrokesChanged);
        _strokes = nullptr;
        int count = _strokeInfos.size();
        for ( int i = 0; i < count; i++)
        {
            _strokeInfos[i]->Detach();
        }
        _strokeInfos.clear();// = nullptr;
    }
    _fValid = false;
}


/// <summary>
/// C-tor.
/// </summary>
/// <param name="strokes">strokes to hit-test</param>
IncrementalHitTester::IncrementalHitTester(QSharedPointer<StrokeCollection> strokes)
{
    Debug::Assert(strokes != nullptr);

    // Create a StrokeInfo object for each stroke.
    _strokeInfos.reserve(strokes->Count());
    for (int x = 0; x < strokes->Count(); x++)
    {
        QSharedPointer<Stroke> stroke = (*strokes)[x];
        _strokeInfos.append(new StrokeInfo(stroke));
    }

    _strokes = strokes;

    // Attach an event handler to the strokes' changed event
    //_strokes.StrokesChangedInternal += new StrokeCollectionChangedEventHandler(OnStrokesChanged);
    QObject::connect(_strokes.get(), &StrokeCollection::StrokesChangedInternal,
                        this, &IncrementalHitTester::OnStrokesChanged);
}


/// <summary>
/// Event handler associated with the stroke collection.
/// </summary>
/// <param name="sender">Stroke collection that was modified</param>
/// <param name="args">Modification that occurred</param>
/// <remarks>
/// Update our _strokeInfos cache.  We get notified on StrokeCollection.StrokesChangedInternal which
/// is raised first so we can assume we're the first delegate in the call chain
/// </remarks>
void IncrementalHitTester::OnStrokesChanged(StrokeCollectionChangedEventArgs& args)
{
    Debug::Assert((_strokes != nullptr)/* && (_strokeInfos != nullptr) && (_strokes == sender)*/);

    QSharedPointer<StrokeCollection> added = args.Added();
    QSharedPointer<StrokeCollection> removed = args.Removed();

    if (added->Count() > 0)
    {
        int firstIndex = _strokes->indexOf((*added)[0]);
        Debug::Assert(firstIndex + added->Count() == args.Index());
        for (int i = 0; i < added->Count(); i++)
        {
            _strokeInfos.insert(firstIndex, new StrokeInfo((*added)[i]));
            firstIndex++;
        }
    }

    if (removed->Count() > 0)
    {
        QSharedPointer<StrokeCollection> localRemoved(new StrokeCollection(*removed));
        //we have to assume that removed strokes can be in any order in _strokes
        for (int i = 0; i < _strokeInfos.size() && localRemoved->Count() > 0; )
        {
            bool found = false;
            for (int j = 0; j < localRemoved->Count(); j++)
            {
                if ((*localRemoved)[j] == _strokeInfos[i]->GetStroke())
                {
                    delete _strokeInfos[i];
                    _strokeInfos.removeAt(i);
                    localRemoved->RemoveItem(j);

                    found = true;
                    break;
                }
            }
            //we didn't find a removed stroke at index i in _strokeInfos, so advance i
            if (!found)
            {
                i++;
            }
        }

        Debug::Assert(localRemoved->Count() == 0);
    }

    //validate our cache
    if (_strokes->Count() != _strokeInfos.size())
    {
        Debug::Assert(false, "Benign assert.  IncrementalHitTester's _strokeInfos cache is out of sync, rebuilding.");
        RebuildStrokeInfoCache();
        return;
    }
    for (int i = 0; i < _strokeInfos.size(); i++)
    {
        if (_strokeInfos[i]->GetStroke() != (*_strokes)[i])
        {
            Debug::Assert(false, "Benign assert.  IncrementalHitTester's _strokeInfos cache is out of sync, rebuilding.");
            RebuildStrokeInfoCache();
            return;
        }
    }
}

void IncrementalHitTester::RebuildStrokeInfoCache()
{
    QList<StrokeInfo*> newStrokeInfos; newStrokeInfos.reserve(_strokes->count());
    for (QSharedPointer<Stroke> stroke : *_strokes)
    {
        bool found = false;
        for (int x = 0; x < _strokeInfos.size(); x++)
        {
            StrokeInfo* strokeInfo = _strokeInfos[x];
            if (strokeInfo != nullptr && stroke == strokeInfo->GetStroke())
            {
                newStrokeInfos.append(strokeInfo);
                //just set to nullptr instead of removing and shifting
                //we're about to GC _strokeInfos
                _strokeInfos[x] = nullptr;
                found = true;
                break;
            }
        }
        if (!found)
        {
            //we didn't find an existing strokeInfo
            newStrokeInfos.append(new StrokeInfo(stroke));
        }
    }

    //detach the remaining strokeInfo's from their strokes
    for (int x = 0; x < _strokeInfos.size(); x++)
    {
        StrokeInfo* strokeInfo = _strokeInfos[x];

        if (strokeInfo != nullptr)
        {
            strokeInfo->Detach();
        }
    }

    _strokeInfos = newStrokeInfos;

#if DEBUG
    Debug.Assert(_strokeInfos.size() == _strokes->count());
    for (int x = 0; x < _strokeInfos.size(); x++)
    {
        Debug.Assert(_strokeInfos[x].Stroke == (*_strokes)[x]);
    }
#endif
}



IncrementalLassoHitTester::IncrementalLassoHitTester(QSharedPointer<StrokeCollection> strokes, int percentageWithinLasso)
    : IncrementalHitTester(strokes)
{
    Debug::Assert((percentageWithinLasso >= 0) && (percentageWithinLasso <= 100));
    _lasso = new SingleLoopLasso();
    _percentIntersect = percentageWithinLasso;
}

IncrementalLassoHitTester::~IncrementalLassoHitTester()
{
    delete _lasso;
}

/// <summary>
/// The implementation behind the public methods AddPoint/AddPoints
/// </summary>
/// <param name="points">new points to add to the lasso</param>
void IncrementalLassoHitTester::AddPointsCore(QVector<QPointF> const & points)
{
    Debug::Assert(points.size() != 0);

    // Add the new points to the lasso
    int lastPointIndex = (0 != _lasso->PointCount()) ? (_lasso->PointCount() - 1) : 0;
    _lasso->AddPoints(points);

    // Do nothing if there's not enough points, or there's nobody listening
    // The points may be filtered out, so if all the points are filtered out, (lastPointIndex == (_lasso->PointCount() - 1).
    // For this case, check if the incremental lasso is disabled (i.e., points modified).
    if ((_lasso->IsEmpty()) || (lastPointIndex == (_lasso->PointCount() - 1) && false == _lasso->IsIncrementalLassoDirty())
        /*|| (SelectionChanged == null*/)
    {
        return;
    }

    // Variables for possible HitChanged events to fire
    QSharedPointer<StrokeCollection> strokesHit = nullptr;
    QSharedPointer<StrokeCollection> strokesUnhit = nullptr;

    // Create a lasso that represents the current increment
    Lasso lassoUpdate;

    if (false == _lasso->IsIncrementalLassoDirty())
    {
        if (0 < lastPointIndex)
        {
            lassoUpdate.AddPoint((*_lasso)[0]);
        }

        // Only the points the have been successfully added to _lasso will be added to
        // lassoUpdate.
        for (; lastPointIndex < _lasso->PointCount(); lastPointIndex++)
        {
            lassoUpdate.AddPoint((*_lasso)[lastPointIndex]);
        }
    }

    // Enumerate through the strokes and update their hit-test results
    for (StrokeInfo *strokeInfo : StrokeInfos())
    {
        Lasso * lasso;
        if (true == strokeInfo->IsDirty() || true == _lasso->IsIncrementalLassoDirty())
        {
            // If this is the first time this stroke gets hit-tested with this lasso,
            // or if the stroke (or its DAs) has changed since the last hit-testing,
            // or if the lasso points have been modified,
            // then (re)hit-test this stroke against the entire lasso->
            lasso = _lasso;
            strokeInfo->SetIsDirty(false);
        }
        else
        {
            // Otherwise, hit-test it against the lasso increment first and then only
            // those ink points that are in that small lasso need to be hit-tested
            // against the big (entire) lasso->
            // This is supposed to be a significant piece of optimization, since
            // lasso increments are usually very small, they are defined by just
            // a few points and they don't capture and/or release too many ink nodes.
            lasso = &lassoUpdate;
        }

        // Skip those stroke which bounding box doesn't even intersects with the lasso bounds
        double hitWeightChange = 0;
        if (lasso->Bounds().intersects(strokeInfo->GetStroke()->GetBounds()))
        {
            // Get the stroke node points for the hit-testing.
            QSharedPointer<StylusPointCollection> stylusPoints = strokeInfo->StylusPoints();

            // Find out if the lasso update has changed the hit count of the stroke.
            for (int i = 0; i < stylusPoints->count(); i++)
            {
                // Consider only the points that become captured/released with this particular update
                if (true == lasso->Contains((*stylusPoints)[i]))
                {
                    double weight = strokeInfo->GetPointWeight(i);

                    if (lasso == _lasso || _lasso->Contains((*stylusPoints)[i]))
                    {
                        hitWeightChange += weight;
                    }
                    else
                    {
                        hitWeightChange -= weight;
                    }
                }
            }
        }

        // Update the stroke hit weight and check whether it has crossed the margin
        // in either direction since the last update.
        if ((hitWeightChange != 0) || (lasso == _lasso))
        {
            strokeInfo->SetHitWeight((lasso == _lasso) ? hitWeightChange : (strokeInfo->HitWeight() + hitWeightChange));
            bool isHit = DoubleUtil::GreaterThanOrClose(strokeInfo->HitWeight(), strokeInfo->TotalWeight() * _percentIntersect / 100 - Stroke::PercentageTolerance);

            if (strokeInfo->IsHit() != isHit)
            {
                strokeInfo->SetIsHit(isHit);
                if (isHit)
                {
                    // The hit count became greater than the margin percentage, the stroke
                    // needs to be reported for selection
                    if (nullptr == strokesHit)
                    {
                        strokesHit.reset(new StrokeCollection());
                    }
                    strokesHit->AddItem(strokeInfo->GetStroke());
                }
                else
                {
                    // The hit count just became less than the margin percentage,
                    // the stroke needs to be reported for de-selection
                    if (nullptr == strokesUnhit)
                    {
                        strokesUnhit.reset(new StrokeCollection());
                    }
                    strokesUnhit->AddItem(strokeInfo->GetStroke());
                }
            }
        }
    }

    _lasso->SetIsIncrementalLassoDirty(false);
    // Raise StrokesHitChanged event if any strokes has changed thier
    // hit status and there're the event subscribers.
    if ((nullptr != strokesHit) || (nullptr != strokesUnhit))
    {
        LassoSelectionChangedEventArgs e(strokesHit, strokesUnhit);
        OnSelectionChanged(e);
    }
}

/// <summary>
/// SelectionChanged event raiser
/// </summary>
/// <param name="eventArgs"></param>
void IncrementalLassoHitTester::OnSelectionChanged(LassoSelectionChangedEventArgs&  eventArgs)
{
    //Debug::Assert(eventArgs != nullptr);
    //if (SelectionChanged != nullptr)
    //{
        emit SelectionChanged(eventArgs);
    //}
}



/// <summary>
/// C-tor
/// </summary>
/// <param name="strokes">strokes to hit-test for erasing</param>
/// <param name="eraserShape">erasing shape</param>
IncrementalStrokeHitTester::IncrementalStrokeHitTester(QSharedPointer<StrokeCollection> strokes, StylusShape& eraserShape)
    : IncrementalHitTester(strokes)
    , _erasingStroke(eraserShape)
{
    //Debug::Assert(eraserShape != nullptr);

    // Create an ErasingStroke objects that implements the actual hit-testing
    //_erasingStroke = new ErasingStroke(eraserShape);
}

//static QDebug & operator<<(QDebug & d, StrokeIntersection const & i)
//{
//    d << i.ToString();
//    return d;
//}

/// <summary>
/// The implementation behind the public methods AddPoint/AddPoints
/// </summary>
/// <param name="points">a set of points representing the last increment
/// in the moving of the erasing shape</param>
void IncrementalStrokeHitTester::AddPointsCore(QVector<QPointF> const & points)
{
    Debug::Assert((points.size() != 0));
    //Debug::Assert(_erasingStroke != nullptr);

    // Move the shape through the new points and build the contour of the move.
    _erasingStroke.MoveTo(points);
    QRectF erasingBounds = _erasingStroke.Bounds();
    if (erasingBounds.isEmpty())
    {
        return;
    }

    QList<StrokeHitEventArgs> strokeHitEventArgCollection;
    // Do nothing if there's nobody listening to the events
    //if (StrokeHit != nullptr)
    //{
        QList<StrokeIntersection> eraseAt;

        // Test stroke by stroke and collect the results.
        for (int x = 0; x < StrokeInfos().count(); x++)
        {
            StrokeInfo* strokeInfo = StrokeInfos()[x];

            // Skip the stroke if its bounding box doesn't intersect with the one of the hitting shape.
            if ((erasingBounds.intersects(strokeInfo->GetStroke()->GetBounds()) == false) ||
                (_erasingStroke.EraseTest(StrokeNodeIterator::GetIterator(*strokeInfo->GetStroke(), *strokeInfo->GetStroke()->GetDrawingAttributes()), eraseAt) == false))
            {
                continue;
            }

            // Create an event args to raise after done with hit-testing
            // We don't fire these events right away because user is expected to
            // modify the stroke collection in her event handler, and that would
            // invalidate this foreach loop.
            //if (strokeHitEventArgCollection == nullptr)
            //{
            //    strokeHitEventArgCollection = new List<StrokeHitEventArgs>();
            //}
            //qDebug() << "StrokeHitEventArgs" << eraseAt;
            strokeHitEventArgCollection.append(StrokeHitEventArgs(strokeInfo->GetStroke(), eraseAt.toVector()));
            // We must clear eraseAt or it will contain invalid results for the next strokes
            eraseAt.clear();
        }
    //}

    // Raise StrokeHit event if needed.
    //if (strokeHitEventArgCollection != nullptr)
    //{
        //Debug::Assert(strokeHitEventArgCollection.size() != 0);
        for (int x = 0; x < strokeHitEventArgCollection.count(); x++)
        {
            StrokeHitEventArgs& eventArgs = strokeHitEventArgCollection[x];

            Debug::Assert(eventArgs.HitStroke() != nullptr);
            OnStrokeHit(eventArgs);
        }
    //}
}

/// <summary>
/// Event raiser for StrokeHit
/// </summary>
void IncrementalStrokeHitTester::OnStrokeHit(StrokeHitEventArgs& eventArgs)
{
    //Debug::Assert(eventArgs != nullptr);
    //if (StrokeHit != nullptr)
    //{
        emit StrokeHit(eventArgs);
    //}
}


/// <summary>
/// StrokeInfo
/// </summary>
StrokeInfo::StrokeInfo(QSharedPointer<Stroke> stroke)
    :_stroke(stroke)
{
    Debug::Assert(stroke != nullptr);
    //_stroke = stroke;
    _bounds = stroke->GetBounds();

    // Start listening to the stroke events
    //_stroke.DrawingAttributesChanged += new PropertyDataChangedEventHandler(OnStrokeDrawingAttributesChanged);
    //_stroke.StylusPointsReplaced += new StylusPointsReplacedEventHandler(OnStylusPointsReplaced);
    //_stroke.StylusPoints.Changed += new EventHandler(OnStylusPointsChanged);
    //_stroke.DrawingAttributesReplaced += new DrawingAttributesReplacedEventHandler(OnDrawingAttributesReplaced);
    QObject::connect(_stroke.get(), &Stroke::DrawingAttributesChanged, this, &StrokeInfo::OnStrokeDrawingAttributesChanged);
    QObject::connect(_stroke.get(), &Stroke::StylusPointsReplaced, this, &StrokeInfo::OnStylusPointsReplaced);
    QObject::connect(_stroke->StylusPoints().get(), &StylusPointCollection::Changed, this, &StrokeInfo::OnStylusPointsChanged);
    QObject::connect(_stroke.get(), &Stroke::DrawingAttributesReplaced, this, &StrokeInfo::OnDrawingAttributesReplaced);
}

/// <summary>
/// Cache teh stroke points
/// </summary>
QSharedPointer<StylusPointCollection> StrokeInfo::StylusPoints()
{
    if (_stylusPoints == nullptr)
    {
        if (_stroke->GetDrawingAttributes()->FitToCurve())
        {
            _stylusPoints = _stroke->GetBezierStylusPoints();
        }
        else
        {
            _stylusPoints = _stroke->StylusPoints();
        }
    }
    return _stylusPoints;
}


void StrokeInfo::SetHitWeight(double value)
{
    // it is ok to clamp this off, rounding error sends it over or under by a minimal amount.
    if (DoubleUtil::GreaterThan(value, TotalWeight()))
    {
        _hitWeight = TotalWeight();
    }
    else if (DoubleUtil::LessThan(value, 0))
    {
        _hitWeight = 0;
    }
    else
    {
        _hitWeight = value;
    }
}

/// <summary>
/// Get the total weight of the stroke. For this implementation, it is the total length of the stroke.
/// </summary>
/// <returns></returns>
double StrokeInfo::TotalWeight()
{

    if (!_totalWeightCached)
    {
        _totalWeight = 0;
        for (int i = 0; i < StylusPoints()->count(); i++)
        {
            _totalWeight += GetPointWeight(i);
        }
        _totalWeightCached = true;
    }
    return _totalWeight;
}

/// <summary>
/// Calculate the weight of a point.
/// </summary>
double StrokeInfo::GetPointWeight(int index)
{
    QSharedPointer<StylusPointCollection> stylusPoints = StylusPoints();
    QSharedPointer<DrawingAttributes> da = GetStroke()->GetDrawingAttributes();
    Debug::Assert(stylusPoints != nullptr && index >= 0 && index < stylusPoints->count());

    double weight = 0;
    if (index == 0)
    {
        weight += sqrt(da->Width()*da->Width() + da->Height()*da->Height()) / 2.0;
    }
    else
    {
        QPointF spine = (*stylusPoints)[index] - (*stylusPoints)[index - 1];
        weight += sqrt(LengthSquared(spine)) / 2.0;
    }

    if (index == stylusPoints->count() - 1)
    {
        weight += sqrt(da->Width()*da->Width() + da->Height()*da->Height()) / 2.0;
    }
    else
    {
        QPointF spine = (*stylusPoints)[index + 1] - (*stylusPoints)[index];
        weight += sqrt(LengthSquared(spine)) / 2.0;
    }

    return weight;
}
/// <summary>
/// A kind of disposing method
/// </summary>
void StrokeInfo::Detach()
{
    if (_stroke != nullptr)
    {
        // Detach the event handlers
        //_stroke.DrawingAttributesChanged -= new PropertyDataChangedEventHandler(OnStrokeDrawingAttributesChanged);
        //_stroke.StylusPointsReplaced -= new StylusPointsReplacedEventHandler(OnStylusPointsReplaced);
        //_stroke.StylusPoints.Changed -= new EventHandler(OnStylusPointsChanged);
        //_stroke.DrawingAttributesReplaced -= new DrawingAttributesReplacedEventHandler(OnDrawingAttributesReplaced);
        QObject::disconnect(_stroke.get(), &Stroke::DrawingAttributesChanged, this, &StrokeInfo::OnStrokeDrawingAttributesChanged);
        QObject::disconnect(_stroke.get(), &Stroke::StylusPointsReplaced, this, &StrokeInfo::OnStylusPointsReplaced);
        QObject::disconnect(_stroke->StylusPoints().get(), &StylusPointCollection::Changed, this, &StrokeInfo::OnStylusPointsChanged);
        QObject::disconnect(_stroke.get(), &Stroke::DrawingAttributesReplaced, this, &StrokeInfo::OnDrawingAttributesReplaced);
        _stroke = nullptr;
    }
}


/// <summary>Event handler for stroke data changed events</summary>
void StrokeInfo::OnStylusPointsChanged()
{
    Invalidate();
}

/// <summary>Event handler for stroke data changed events</summary>
void StrokeInfo::OnStylusPointsReplaced(StylusPointsReplacedEventArgs& args)
{
    (void) args;
    Invalidate();
}

/// <summary>
/// Event handler for stroke's drawing attributes changes.
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void StrokeInfo::OnStrokeDrawingAttributesChanged(PropertyDataChangedEventArgs args)
{
    // Only enforce rehittesting of the whole stroke when the DrawingAttribute change may affect hittesting
    if(DrawingAttributes::IsGeometricalDaGuid(args.PropertyGuid()))
    {
        Invalidate();
    }
}

void StrokeInfo::OnDrawingAttributesReplaced(DrawingAttributesReplacedEventArgs& args)
{
    // If the drawing attributes change involves Width, Height, StylusTipTransform, IgnorePressure, or FitToCurve,
    // we need to invalidate
    if (false == DrawingAttributes::GeometricallyEqual(*args.NewDrawingAttributes(), *args.PreviousDrawingAttributes()))
    {
        Invalidate();
    }
}

/// <summary>Implementation for the event handlers above</summary>
void StrokeInfo::Invalidate()
{
    _totalWeightCached = false;
    _stylusPoints = nullptr;
    _hitWeight = 0;

    // Let the hit-tester know that it should not use incremental hit-testing
    _isDirty = true;

    // The Stroke.GetBounds may be overriden in the 3rd party code.
    // The out-side code could throw exception. If an exception is thrown, _bounds will keep the original value.
    // Re-compute the stroke bounds
    _bounds = _stroke->GetBounds();
}


QSharedPointer<StrokeCollection> StrokeHitEventArgs::GetPointEraseResults()
{
    return _stroke->Erase(_hitFragments);
}

void StrokeHitEventArgs::Clip(QVector<StrokeIntersection> &fragments)
{
    _hitFragments = StrokeIntersection::GetClippedHitSegments(_hitFragments, fragments);
}
