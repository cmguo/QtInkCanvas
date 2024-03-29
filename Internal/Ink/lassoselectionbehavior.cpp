#include "Internal/Ink/lassoselectionbehavior.h"
#include "Windows/Input/styluspoint.h"
#include "Windows/Controls/inkcanvas.h"
#include "Internal/doubleutil.h"
#include "Internal/Ink/lassohelper.h"
#include "Windows/Ink/incrementalhittester.h"
#include "Internal/Ink/lassoselectionbehavior.h"
#include "Internal/Controls/inkcanvasinnercanvas.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors

LassoSelectionBehavior::LassoSelectionBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : StylusEditingBehavior(editingCoordinator, inkCanvas)
{
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Protected Methods
//
//-------------------------------------------------------------------------------

////#region Protected Methods

/// <summary>
/// Overrides SwitchToMode as the following expectations
/// 31. From Select To InkAndGesture
///     Lasso is discarded. After mode change ink is being collected, gesture event fires. If its not a gesture, StrokeCollected event fires.
/// 32. From Select To GestureOnly
///     Lasso is discarded. After mode change ink is being collected. On StylusUp gesture event fires. Stroke gets removed on StylusUp even if its not a gesture.
/// 33. From Select To EraseByPoint
///     Lasso is discarded. PointErasing is performed after changing the mode.
/// 34. From Select To EraseByStroke
///     Lasso is discarded. StrokeErasing is performed after changing the mode.
/// 35. From Select To Ink
///     Ink collection starts when changing the mode.
/// 36. From Select To None
///     Nothing gets selected.
/// </summary>
/// <param name="mode"></param>
void LassoSelectionBehavior::OnSwitchToMode(InkCanvasEditingMode mode)
{
    Debug::Assert(GetEditingCoordinator().IsInMidStroke(), "SwitchToMode should only be called in a mid-stroke");

    switch ( mode )
    {
        case InkCanvasEditingMode::Ink:
        case InkCanvasEditingMode::InkAndGesture:
        case InkCanvasEditingMode::GestureOnly:
            {
                // Discard the lasso
                Commit(false);

                // Change the mode. The dynamic renderer will be reset automatically.
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);
                break;
            }
        case InkCanvasEditingMode::EraseByPoint:
        case InkCanvasEditingMode::EraseByStroke:
            {
                // Discard the lasso
                Commit(false);

                // Change the mode
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);

                break;
            }
        case InkCanvasEditingMode::Select:
            {
                Debug::Assert(false, "Cannot switch from Select to Select in mid-stroke");
                break;
            }
        case InkCanvasEditingMode::None:
            {
                // Discard the lasso.
                Commit(false);

                // Change to the None mode
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);
                break;
            }
        default:
            Debug::Assert(false, "Unknown InkCanvasEditingMode!");
            break;
    }
}

/// <summary>
/// StylusInputBegin
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
/// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
void LassoSelectionBehavior::StylusInputBegin(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    (void) userInitiated;
    Debug::Assert(stylusPoints->Count() != 0, "An empty stylusPoints has been passed in.");

    _disableLasso = false;

    bool startLasso = false;

    List<Point> points;
    for ( int x = 0; x < stylusPoints->Count(); x++ )
    {
        Point point = ( (*stylusPoints)[x] );
        if ( x == 0 )
        {
            _startPoint = point;
            points.Add(point);
            continue;
        }

        if ( !startLasso )
        {
            // If startLasso hasn't be flagged, we should check if the distance between two points is greater than
            // our tolerance. If so, we should flag startLasso.
            Vector vector = point - _startPoint;
            double distanceSquared = vector.LengthSquared();

            if ( DoubleUtil::GreaterThan(distanceSquared, LassoHelper::MinDistanceSquared) )
            {
                points.Add(point);
                startLasso = true;
            }
        }
        else
        {
            // The flag is set. We just add the point.
            points.Add(point);
        }
    }

    // Start Lasso if it isn't a tap selection.
    if ( startLasso )
    {
        StartLasso(points);
    }
}

/// <summary>
/// StylusInputContinue
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
/// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
void LassoSelectionBehavior::StylusInputContinue(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    // Check whether Lasso has started.
    if ( _lassoHelper != nullptr )
    {
        //
        // pump packets to the LassoHelper, it will convert them into an array of equidistant
        // lasso points, render those lasso point and return them to hit test against.
        //
        List<Point> points;
        for ( int x = 0; x < stylusPoints->Count(); x++ )
        {
            points.Add((*stylusPoints)[x]);
        }
        Array<Point> lassoPoints = _lassoHelper->AddPoints(points);
        if ( 0 != lassoPoints.Length() )
        {
            _incrementalLassoHitTester->AddPoints(lassoPoints);
        }
    }
    else if ( !_disableLasso )
    {
        // If Lasso hasn't start and been disabled, we should try to start it when it is needed.
        bool startLasso = false;

        List<Point> points ;
        for ( int x = 0; x < stylusPoints->Count(); x++ )
        {
            Point point = (*stylusPoints)[x];

            if ( !startLasso )
            {
                // If startLasso hasn't be flagged, we should check if the distance between two points is greater than
                // our tolerance. If so, we should flag startLasso.
                Vector vector = point - _startPoint;
                double distanceSquared = vector.LengthSquared();

                if ( DoubleUtil::GreaterThan(distanceSquared, LassoHelper::MinDistanceSquared) )
                {
                    points.Add(point);
                    startLasso = true;
                }
            }
            else
            {
                // The flag is set. We just add the point.
                points.Add(point);
            }
        }

        // Start Lasso if it isn't a tap selection.
        if ( startLasso )
        {
            StartLasso(points);
        }
    }
}


/// <summary>
/// StylusInputEnd
/// </summary>
/// <param name="commit">commit</param>
void LassoSelectionBehavior::StylusInputEnd(bool commit)
{
    // Initialize with empty selection
    SharedPointer<StrokeCollection> selectedStrokes(new StrokeCollection());
    List<UIElement*> elementsToSelect;

    if ( _lassoHelper != nullptr )
    {
        // This is a lasso selection.

        //
        // end dynamic rendering
        //
        selectedStrokes = GetInkCanvas().EndDynamicSelection(_lassoHelper->GetVisual());

        //
        // hit test for elements
        //
        // NOTE: HitTestForElements uses the _lassoHelper so it must be alive at this point
        elementsToSelect = HitTestForElements();

        QObject::disconnect(_incrementalLassoHitTester, &IncrementalLassoHitTester::SelectionChanged,
                            this, &LassoSelectionBehavior::OnSelectionChanged);
        //_incrementalLassoHitTester->SelectionChanged() -= new LassoSelectionChangedEventHandler(OnSelectionChanged);
        _incrementalLassoHitTester->EndHitTesting();
        delete _incrementalLassoHitTester;
        _incrementalLassoHitTester = nullptr;
        delete _lassoHelper;
        _lassoHelper = nullptr;
    }
    else
    {
        // This is a tap selection.

        // Now try the tap selection
        SharedPointer<Stroke> tappedStroke;
        UIElement* tappedElement;

        TapSelectObject(_startPoint,  tappedStroke,  tappedElement);

        // If we have a pre-selected object, we should select it now.
        if ( tappedStroke != nullptr )
        {
            Debug::Assert(tappedElement == nullptr);
            selectedStrokes.reset(new StrokeCollection());
            selectedStrokes->Add(tappedStroke);
        }
        else if ( tappedElement != nullptr )
        {
            Debug::Assert(tappedStroke == nullptr);
            elementsToSelect.Add(tappedElement);
        }
    }

    SelfDeactivate();

    if ( commit )
    {
        GetInkCanvas().ChangeInkCanvasSelection(selectedStrokes, elementsToSelect);
    }
}

/// <summary>
/// OnCommitWithoutStylusInput
/// </summary>
/// <param name="commit"></param>
void LassoSelectionBehavior::OnCommitWithoutStylusInput(bool commit)
{
    (void) commit;
    // We only deactivate LSB in this case.
    SelfDeactivate();
}

/// <summary>
/// Get the current cursor for this editing mode
/// </summary>
/// <returns></returns>
QCursor LassoSelectionBehavior::GetCurrentCursor()
{
    // By default return cross cursor.
    return Qt::CrossCursor;
}

//#endregion Protected Methods

//-------------------------------------------------------------------------------
//
// Private Methods
//
//-------------------------------------------------------------------------------

////#region Private Methods

/// <summary>
/// Private event handler that updates which strokes are actually selected
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void LassoSelectionBehavior::OnSelectionChanged(LassoSelectionChangedEventArgs& e)
{
    GetInkCanvas().UpdateDynamicSelection(e.SelectedStrokes(), e.DeselectedStrokes());
}


/// <summary>
/// Private helper that will hit test for elements
/// </summary>
List<UIElement*> LassoSelectionBehavior::HitTestForElements()
{
    List<UIElement*> elementsToSelect;

    List<UIElement *> children = GetInkCanvas().Children();
    if ( children.Count() == 0 )
    {
        return elementsToSelect;
    }

    for (int x = 0; x < children.Count(); x++)
    {
        UIElement* uiElement = children[x];
        HitTestElement(GetInkCanvas().InnerCanvas(), uiElement, elementsToSelect);
    }

    return elementsToSelect;
}

/// <summary>
/// Private helper that will turn an element in any nesting level into a stroke
/// in the InkCanvas's coordinate space.  This method calls itself recursively
/// </summary>
void LassoSelectionBehavior::HitTestElement(InkCanvasInnerCanvas& parent, UIElement* uiElement, List<UIElement*> elementsToSelect)
{
    ElementCornerPoints elementPoints = LassoSelectionBehavior::GetTransformedElementCornerPoints(parent, uiElement);
    if (elementPoints.Set != false)
    {
        Array<Point> points = GeneratePointGrid(elementPoints);

        //
        // perform hit testing against our lasso
        //
        //System.Diagnostics.Debug::Assert(null != _lassoHelper);
        if (_lassoHelper->ArePointsInLasso(points, _percentIntersectForElements))
        {
            elementsToSelect.Add(uiElement);
        }
    }
    //
    // we used to recurse into the childrens children.  That is no longer necessary
    //
}

/// <summary>
/// Private helper that takes an element and transforms it's 4 points
/// into the InkCanvas's space
/// </summary>
LassoSelectionBehavior::ElementCornerPoints LassoSelectionBehavior::GetTransformedElementCornerPoints(InkCanvasInnerCanvas& canvas, UIElement* childElement)
{
    //Debug::Assert(canvas != nullptr);
    Debug::Assert(childElement != nullptr);

    Debug::Assert(canvas.CheckAccess());

    ElementCornerPoints elementPoints;
    elementPoints.Set = false;

    if (!childElement->isVisible())
    {
        //
        // this little one's not worth it...
        //
        return elementPoints;
    }

    //
    // get the transform from us to our parent InkCavas
    //
    GeneralTransform parentTransform = childElement->TransformToAncestor(&canvas);

    //

    elementPoints.UpperLeft = parentTransform.Transform(Point(0, 0));
    elementPoints.UpperRight = parentTransform.Transform(Point(childElement->RenderSize().Width(), 0));
    elementPoints.LowerLeft = parentTransform.Transform(Point(0, childElement->RenderSize().Height()));
    elementPoints.LowerRight = parentTransform.Transform(Point(childElement->RenderSize().Width(), childElement->RenderSize().Height()));

    elementPoints.Set = true;
    return elementPoints;
}

/// <summary>
/// Private helper that will generate a grid of points 5 px apart given the elements bounding points
/// this works with any affline transformed points
/// </summary>
Array<Point> LassoSelectionBehavior::GeneratePointGrid(ElementCornerPoints elementPoints)
{
    if (!elementPoints.Set)
    {
        return QVector<Point>();
    }
    List<Point> pointArray;

    UpdatePointDistances(elementPoints);

    //
    // add our original points
    //
    pointArray.Add(elementPoints.UpperLeft);
    pointArray.Add(elementPoints.UpperRight);
    FillInPoints(pointArray, elementPoints.UpperLeft, elementPoints.UpperRight);

    pointArray.Add(elementPoints.LowerLeft);
    pointArray.Add(elementPoints.LowerRight);
    FillInPoints(pointArray, elementPoints.LowerLeft, elementPoints.LowerRight);

    FillInGrid( pointArray,
                elementPoints.UpperLeft,
                elementPoints.UpperRight,
                elementPoints.LowerRight,
                elementPoints.LowerLeft);

    //Point[] retPointArray = Point[pointArray.Count];
    //pointArray.CopyTo(retPointArray);
    return pointArray.ToArray();
}

/// <summary>
/// Private helper that fills in the points between two points by calling itself
/// recursively in a divide and conquer fashion
/// </summary>
void LassoSelectionBehavior::FillInPoints(List<Point> & pointArray, Point const & point1, Point const & point2)
{
    // this algorithm improves perf by 20%
    if(!PointsAreCloseEnough(point1, point2))
    {
        Point midPoint = LassoSelectionBehavior::GeneratePointBetweenPoints(point1, point2);
        pointArray.Add(midPoint);

        if(!PointsAreCloseEnough(point1, midPoint))
        {
            FillInPoints(pointArray, point1, midPoint);
        }

        //sort the right
        if(!PointsAreCloseEnough(midPoint, point2))
        {
            FillInPoints(pointArray, midPoint, point2);
        }
    }
}

/// <summary>
/// Private helper that fills in the points between four points by calling itself
/// recursively in a divide and conquer fashion
/// </summary>
void LassoSelectionBehavior::FillInGrid(List<Point> & pointArray,
                                        Point const & upperLeft,
                                        Point const & upperRight,
                                        Point const & lowerRight,
                                        Point const & lowerLeft)
{
    // this algorithm improves perf by 20%
    if(!PointsAreCloseEnough(upperLeft, lowerLeft))
    {
        Point midPointLeft = LassoSelectionBehavior::GeneratePointBetweenPoints(upperLeft, lowerLeft);
        Point midPointRight = LassoSelectionBehavior::GeneratePointBetweenPoints(upperRight, lowerRight);
        pointArray.Add(midPointLeft);
        pointArray.Add(midPointRight);
        FillInPoints(pointArray, midPointLeft, midPointRight);

        if(!PointsAreCloseEnough(upperLeft, midPointLeft))
        {
            FillInGrid(pointArray, upperLeft, upperRight, midPointRight, midPointLeft);
        }

        //sort the right
        if(!PointsAreCloseEnough(midPointLeft, lowerLeft))
        {
            FillInGrid(pointArray, midPointLeft, midPointRight, lowerRight, lowerLeft);
        }
    }
}

/// <summary>
/// Private helper that will generate a Point between two points
/// </summary>
Point LassoSelectionBehavior::GeneratePointBetweenPoints(Point const & point1, Point const & point2)
{
    //
    // compute the Point in the middle of the previous two
    //
    double maxX = point1.X() > point2.X() ? point1.X() : point2.X();
    double minX = point1.X() < point2.X() ? point1.X() : point2.X();
    double maxY = point1.Y() > point2.Y() ? point1.Y() : point2.Y();
    double minY = point1.Y() < point2.Y() ? point1.Y() : point2.Y();

    return Point( (minX + ((maxX - minX) * 0.5)),
                        (minY + ((maxY - minY) * 0.5)));
}

/// <summary>
/// Private helper used to determine if we're close enough between two points
/// </summary>
bool LassoSelectionBehavior::PointsAreCloseEnough(Point const & point1, Point const & point2)
{
    double x = point1.X() - point2.X();
    double y = point1.Y() - point2.Y();
    if ((x < _xDiff && x > -_xDiff) && (y < _yDiff && y > -_yDiff))
    {
        return true;
    }
    return false;
}

/// <summary>
/// Used to calc the diff between points on the x and y axis
/// </summary>
void LassoSelectionBehavior::UpdatePointDistances(ElementCornerPoints elementPoints)
{
    //
    // calc the x and y diffs
    //
    double width = elementPoints.UpperLeft.X() - elementPoints.UpperRight.X();
    if (width < 0)
    {
        width = -width;
    }

    double height = elementPoints.UpperLeft.Y() - elementPoints.LowerLeft.Y();
    if (height < 0)
    {
        height = -height;
    }

    _xDiff = width * 0.25;
    if (_xDiff > _maxThreshold)
    {
        _xDiff = _maxThreshold;
    }
    else if (_xDiff < _minThreshold)
    {
        _xDiff = _minThreshold;
    }

    _yDiff = height * 0.25;
    if (_yDiff > _maxThreshold)
    {
        _yDiff = _maxThreshold;
    }
    else if (_yDiff < _minThreshold)
    {
        _yDiff = _minThreshold;
    }
}

/// <summary>
/// StartLasso
/// </summary>
/// <param name="points"></param>
void LassoSelectionBehavior::StartLasso(List<Point> points)
{
    Debug::Assert(!_disableLasso && _lassoHelper == nullptr, "StartLasso is called unexpectedly.");

    if ( GetInkCanvas().ClearSelectionRaiseSelectionChanging() // If user cancels clearing the selection, we shouldn't initiate Lasso.
        // NTRAID:WINDOWS#1534264-2006/02/28-WAYNEZEN
        // If the active editng mode is no longer as Select, we shouldn't activate LassoSelectionBehavior.
        // Note the order really matters here. This checking has to be done
        // after ClearSelectionRaiseSelectionChanging is invoked.
        && GetEditingCoordinator().ActiveEditingMode() == InkCanvasEditingMode::Select )
    {

        //
        // obtain a dynamic hit-tester for selecting with lasso
        //
        _incrementalLassoHitTester =
                            GetInkCanvas().Strokes()->GetIncrementalLassoHitTester(_percentIntersectForInk);
        //
        // add event handler
        //
        QObject::connect(_incrementalLassoHitTester, &IncrementalLassoHitTester::SelectionChanged,
                            this, &LassoSelectionBehavior::OnSelectionChanged);
        //_incrementalLassoHitTester->SelectionChanged += new LassoSelectionChangedEventHandler(OnSelectionChanged);

        //
        // start dynamic rendering
        //

        _lassoHelper = new LassoHelper();
        GetInkCanvas().BeginDynamicSelection(_lassoHelper->GetVisual());

        Array<Point> lassoPoints = _lassoHelper->AddPoints(points);
        if ( 0 != lassoPoints.Length() )
        {
            _incrementalLassoHitTester->AddPoints(lassoPoints);
        }
    }
    else
    {
        // If we fail on clearing the selection or switching to Select mode, we should just disable lasso.
        _disableLasso = true;
    }
}

/// <summary>
/// Pre-Select the object which user taps on.
/// </summary>
/// <param name="point"></param>
/// <param name="tappedStroke"></param>
/// <param name="tappedElement"></param>
void LassoSelectionBehavior::TapSelectObject(Point const & point,  SharedPointer<Stroke>& tappedStroke,  UIElement*& tappedElement)
{
    tappedStroke = nullptr;
    tappedElement = nullptr;

    SharedPointer<StrokeCollection> hitTestStrokes = GetInkCanvas().Strokes()->HitTest(point, 5.0);
    if ( hitTestStrokes->Count() > 0 )
    {
        tappedStroke = (*hitTestStrokes)[hitTestStrokes->Count() - 1];
    }
    else
    {
        GeneralTransform transformToInnerCanvas = GetInkCanvas().TransformToVisual(&GetInkCanvas().InnerCanvas());
        Point pointOnInnerCanvas = transformToInnerCanvas.Transform(point);

        // Try to find  whether we have a pre-select object.
        tappedElement = GetInkCanvas().InnerCanvas().HitTestOnElements(pointOnInnerCanvas);
    }

}

INKCANVAS_END_NAMESPACE
