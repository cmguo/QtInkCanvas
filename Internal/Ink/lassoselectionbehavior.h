#ifndef LASSOSELECTIONBEHAVIOR_H
#define LASSOSELECTIONBEHAVIOR_H

#include "styluseditingbehavior.h"

INKCANVAS_BEGIN_NAMESPACE

class LassoHelper;
class IncrementalLassoHitTester;
class LassoSelectionChangedEventArgs;
class InkCanvasInnerCanvas;
class Stroke;
class UIElement;

// namespace MS.Internal.Ink

class LassoSelectionBehavior : public StylusEditingBehavior
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    struct ElementCornerPoints;
    ////#region Constructors
public:
    LassoSelectionBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    ////#region Protected Methods

protected:
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
    virtual void OnSwitchToMode(InkCanvasEditingMode mode);

    /// <summary>
    /// StylusInputBegin
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
    virtual void StylusInputBegin(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated);

    /// <summary>
    /// StylusInputContinue
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
    virtual void StylusInputContinue(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated);


    /// <summary>
    /// StylusInputEnd
    /// </summary>
    /// <param name="commit">commit</param>
    virtual void StylusInputEnd(bool commit);

    /// <summary>
    /// OnCommitWithoutStylusInput
    /// </summary>
    /// <param name="commit"></param>
    virtual void OnCommitWithoutStylusInput(bool commit);

    /// <summary>
    /// Get the current cursor for this editing mode
    /// </summary>
    /// <returns></returns>
    virtual QCursor GetCurrentCursor();

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
    void OnSelectionChanged(LassoSelectionChangedEventArgs& e);


    /// <summary>
    /// Private helper that will hit test for elements
    /// </summary>
    QList<UIElement*> HitTestForElements();

    /// <summary>
    /// Private helper that will turn an element in any nesting level into a stroke
    /// in the InkCanvas's coordinate space.  This method calls itself recursively
    /// </summary>
    void HitTestElement(InkCanvasInnerCanvas& parent, UIElement* uiElement, QList<UIElement*> elementsToSelect);

    /// <summary>
    /// Private helper that takes an element and transforms it's 4 points
    /// into the InkCanvas's space
    /// </summary>
    static ElementCornerPoints GetTransformedElementCornerPoints(InkCanvasInnerCanvas& canvas, UIElement* childElement);

    /// <summary>
    /// Private helper that will generate a grid of points 5 px apart given the elements bounding points
    /// this works with any affline transformed points
    /// </summary>
    QVector<QPointF> GeneratePointGrid(ElementCornerPoints elementPoints);

    /// <summary>
    /// Private helper that fills in the points between two points by calling itself
    /// recursively in a divide and conquer fashion
    /// </summary>
    void FillInPoints(QVector<QPointF> & pointArray, QPointF const & point1, QPointF const & point2);

    /// <summary>
    /// Private helper that fills in the points between four points by calling itself
    /// recursively in a divide and conquer fashion
    /// </summary>
    void FillInGrid(QVector<QPointF> & pointArray,
                            QPointF const & upperLeft,
                            QPointF const & upperRight,
                            QPointF const & lowerRight,
                            QPointF const & lowerLeft);

    /// <summary>
    /// Private helper that will generate a new QPointF const & between two points
    /// </summary>
    static QPointF GeneratePointBetweenPoints(QPointF const & point1, QPointF const & point2);

    /// <summary>
    /// Private helper used to determine if we're close enough between two points
    /// </summary>
    bool PointsAreCloseEnough(QPointF const & point1, QPointF const & point2);

    /// <summary>
    /// Used to calc the diff between points on the x and y axis
    /// </summary>
    void UpdatePointDistances(ElementCornerPoints elementPoints);

    /// <summary>
    /// StartLasso
    /// </summary>
    /// <param name="points"></param>
    void StartLasso(QList<QPointF> points);

    /// <summary>
    /// Pre-Select the object which user taps on.
    /// </summary>
    /// <param name="point"></param>
    /// <param name="tappedStroke"></param>
    /// <param name="tappedElement"></param>
    void TapSelectObject(QPointF const & point, QSharedPointer<Stroke>& tappedStroke, UIElement*& tappedElement);

    //#endregion Private Methods

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------

    ////#region Private Fields

private:
    QPointF                     _startPoint;
    bool                        _disableLasso;

    LassoHelper*                 _lassoHelper = nullptr;
    IncrementalLassoHitTester*   _incrementalLassoHitTester;
    double                  _xDiff;
    double                  _yDiff;
    const double            _maxThreshold = 50;
    const double            _minThreshold = 15;
    const int               _percentIntersectForInk = 80;
    const int               _percentIntersectForElements = 60;

#if DEBUG_LASSO_FEEDBACK
    ContainerVisual _tempVisual;
#endif
    /// <summary>
    /// Private struct
    /// </summary>
    struct ElementCornerPoints
    {
        QPointF UpperLeft;
        QPointF UpperRight;
        QPointF LowerRight;
        QPointF LowerLeft;
        bool Set;
    };

    //#endregion Private Fields

};

INKCANVAS_END_NAMESPACE

#endif // LASSOSELECTIONBEHAVIOR_H
