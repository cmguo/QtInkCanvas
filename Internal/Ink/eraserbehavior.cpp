#include "Internal/Ink/eraserbehavior.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Controls/inkcanvas.h"
#include "Windows/Ink/stylusshape.h"
#include "Windows/Ink/incrementalhittester.h"
#include "Windows/Controls/inkevents.h"
#include "Internal/Ink/pencursormanager.h"
#include "Internal/Ink/strokenode.h"
#include "Internal/Ink/strokenodeoperations.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QApplication>
#include <QScreen>

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors

EraserBehavior::EraserBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : StylusEditingBehavior(editingCoordinator, inkCanvas)
{
}

void EraserBehavior::SetClip(const QPolygonF &shape)
{
    if (shape.empty()) {
        _clipStroke.reset();
    } else {
        QPointF c = shape.boundingRect().center();
        StylusShape ss(shape.translated(-c));
        _clipStroke.reset(new ErasingStroke(ss));
        _clipStroke->MoveTo({c});
    }
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Protected Methods
//
//-------------------------------------------------------------------------------

//#region Protected Methods

/// <summary>
/// Overrides SwitchToMode as the following expectations
/// 19. From EraseByPoint To InkAndGesture
///     After mode change ink is being collected. On StylusUp gesture event fires. If it’s not a gesture, StrokeCollected event fires.
/// 20. From EraseByPoint To GestureOnly
///     After mode change ink is being collected. On StylusUp gesture event fires. Stroke gets removed on StylusUp even if it’s not a gesture.
/// 21. From EraseByPoint To Ink
///     Ink collection starts when changing the mode.
/// 22. From EraseByPoint To EraseByStroke
///     After mode change StrokeErasing is performed.
/// 23. From EraseByPoint To Select
///     Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
/// 24. From EraseByPoint To None
///     No erasing is performed after mode change.
/// 25. From EraseByStroke To InkAndGesture
///     After mode change ink is being collected. On StylusUp gesture event fires. If it’s not a gesture, StrokeCollected event fires.
/// 26. From EraseByStroke To GestureOnly
///     After mode change ink is being collected. On StylusUp gesture event fires. Stroke gets removed on StylusUp even if it’s not a gesture.
/// 27. From EraseByStroke To EraseByPoint
///     After mode change PointErasing is performed.
/// 28. From EraseByStroke To Ink
///     Ink collection starts when changing the mode.
/// 29. From EraseByStroke To Select
///     Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected
/// 30. From EraseByStroke To None
///     No erasing is performed after mode change.
/// </summary>
/// <param name="mode"></param>
void EraserBehavior::OnSwitchToMode(InkCanvasEditingMode mode)
{
    Debug::Assert(GetEditingCoordinator().IsInMidStroke(), "SwitchToMode should only be called in a mid-stroke");

    switch ( mode )
    {
        case InkCanvasEditingMode::Ink:
        case InkCanvasEditingMode::InkAndGesture:
        case InkCanvasEditingMode::GestureOnly:
            {
                // Commit the current behavior
                Commit(true);

                // Change the mode. The dynamic renderer will be reset automatically.
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);
                break;
            }
        case InkCanvasEditingMode::EraseByPoint:
        case InkCanvasEditingMode::EraseByStroke:
            {
                Debug::Assert(_cachedEraseMode != mode);

                // Commit the current behavior
                Commit(true);

                // Change the mode
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);

                break;
            }
        case InkCanvasEditingMode::Select:
            {
                // Make a copy of the current cached points.
                QSharedPointer<StylusPointCollection> cachedPoints = _stylusPoints != nullptr ?
                                                        _stylusPoints->Clone() : nullptr;

                // Commit the current behavior.
                Commit(true);

                // Change the Select mode
                IStylusEditing* newBehavior = GetEditingCoordinator().ChangeStylusEditingMode(this, mode);

                if ( cachedPoints != nullptr
                    // NOTICE-2006/04/27-WAYNEZEN,
                    // GetEditingCoordinator().ChangeStylusEditingMode raises external event.
                    // The user code could take any arbitrary action for instance calling GetInkCanvas().ReleaseMouseCapture()
                    // So there is no guarantee that we could receive the newBehavior->
                    && newBehavior != nullptr )
                {
                    // Now add the previous points to the lasso behavior
                    newBehavior->AddStylusPoints(cachedPoints, false/*userInitiated*/);
                }

                break;
            }
        case InkCanvasEditingMode::None:
            {
                // Discard the collected ink.
                Commit(true);

                // Change to the None mode
                GetEditingCoordinator().ChangeStylusEditingMode(this, mode);
                break;
            }
        default:
            Debug::Assert(false, "Unknown InkCanvasEditingMode!");
            break;
    }
}

void EraserBehavior::OnActivate()
{
    StylusEditingBehavior::OnActivate();
    InkCanvasEditingMode newEraseMode = GetEditingCoordinator().ActiveEditingMode();
    Debug::Assert(newEraseMode == InkCanvasEditingMode::EraseByPoint
                    || newEraseMode == InkCanvasEditingMode::EraseByStroke);

    // Check whether we have to update cursor.
    if ( _cachedEraseMode != newEraseMode )
    {
        // EraseMode is changed
        _cachedEraseMode = newEraseMode;
        GetEditingCoordinator().InvalidateBehaviorCursor(this);
    }
    else if ( newEraseMode == InkCanvasEditingMode::EraseByPoint )
    {
        // Invalidate the PointEraser if we don't have the cache yet.
        bool isPointEraserCursorValid = _cachedStylusShape != nullptr;

        // NTRAID:WINDOWSOS#1673398-2006/05/23-WAYNEZEN,
        // If the cached EraserShape is different from the current EraserShape, we shoud just reset the cache.
        // The new QCursor will be generated when it's needed later.
        if ( isPointEraserCursorValid
            && ( _cachedStylusShape->Width() != GetInkCanvas().EraserShape()->Width()
                || _cachedStylusShape->Height() != GetInkCanvas().EraserShape()->Height()
                || _cachedStylusShape->Rotation() != GetInkCanvas().EraserShape()->Rotation()
                || _cachedStylusShape->metaObject() != GetInkCanvas().EraserShape()->metaObject()) )
        {
            //Debug::Assert(_cachedPointEraserCursor != nullptr, "_cachedPointEraserCursor shouldn't be nullptr.");
            ResetCachedPointEraserCursor();
            isPointEraserCursorValid = false;
        }

        if ( !isPointEraserCursorValid )
        {
            // EraserShape is changed when the new mode is EraseByPoint
            GetEditingCoordinator().InvalidateBehaviorCursor(this);
        }
    }
}

/// <summary>
/// StylusInputBegin
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
/// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
void EraserBehavior::StylusInputBegin(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    //
    // get a disposable dynamic hit-tester and add event handler
    //
    _incrementalStrokeHitTester.reset(
                        GetInkCanvas().Strokes()->GetIncrementalStrokeHitTester(*GetInkCanvas().EraserShape()));


    if ( InkCanvasEditingMode::EraseByPoint == _cachedEraseMode )
    {
        QObject::connect(_incrementalStrokeHitTester.get(), &IncrementalStrokeHitTester::StrokeHit,
                         this, &EraserBehavior::OnPointEraseResultChanged);
        //_incrementalStrokeHitTester->StrokeHit += new StrokeHitEventHandler(OnPointEraseResultChanged);
    }
    else
    {
        //we're in stroke hit test mode
        QObject::connect(_incrementalStrokeHitTester.get(), &IncrementalStrokeHitTester::StrokeHit,
                         this, &EraserBehavior::OnStrokeEraseResultChanged);
        //_incrementalStrokeHitTester->StrokeHit += new StrokeHitEventHandler(OnStrokeEraseResultChanged);
    }

    _stylusPoints.reset(new StylusPointCollection(stylusPoints->Description(), 100));
    _stylusPoints->Add(*stylusPoints);

    //
    // start erasing
    //
    _incrementalStrokeHitTester->AddPoints(*stylusPoints);

    // NTRAID:WINDOWSOS#1642274-2006/05/10-WAYNEZEN,
    // Since InkCanvas will ignore the animated tranforms when it receives the property changes.
    // So we should update our QCursor when the stylus is down if there are animated transforms applied to GetInkCanvas().
    if ( InkCanvasEditingMode::EraseByPoint == _cachedEraseMode )
    {
        // Call InvalidateBehaviorCursor at the end of the routine. The method will cause an external event fired.
        // So it should be invoked after we set up our states.
        GetEditingCoordinator().InvalidateBehaviorCursor(this);
    }
}

/// <summary>
/// StylusInputContinue
/// </summary>
/// <param name="stylusPoints">stylusPoints</param>
/// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
void EraserBehavior::StylusInputContinue(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    _stylusPoints->Add(*stylusPoints);

    _incrementalStrokeHitTester->AddPoints(*stylusPoints);
}

/// <summary>
/// StylusInputEnd
/// </summary>
/// <param name="commit">commit</param>
void EraserBehavior::StylusInputEnd(bool commit)
{
    if ( InkCanvasEditingMode::EraseByPoint == _cachedEraseMode )
    {
        QObject::disconnect(_incrementalStrokeHitTester.get(), &IncrementalStrokeHitTester::StrokeHit,
                         this, &EraserBehavior::OnPointEraseResultChanged);
        //_incrementalStrokeHitTester->StrokeHit-= new StrokeHitEventHandler(OnPointEraseResultChanged);
    }
    else
    {
        //we're in stroke hit test mode
        QObject::disconnect(_incrementalStrokeHitTester.get(), &IncrementalStrokeHitTester::StrokeHit,
                         this, &EraserBehavior::OnStrokeEraseResultChanged);
        //_incrementalStrokeHitTester->StrokeHit -= new StrokeHitEventHandler(OnStrokeEraseResultChanged);
    }

    _stylusPoints = nullptr;

    // NTRAID#T2-26868-2004/11/1-WAYNEZEN,
    // Don't forget ending the current incremental hit testing.
    _incrementalStrokeHitTester->EndHitTesting();
    _incrementalStrokeHitTester = nullptr;

    GetEditingCoordinator().InvalidateBehaviorCursor(this);
}

/// <summary>
/// Get eraser cursor.
/// </summary>
/// <returns></returns>
QCursor EraserBehavior::GetCurrentCursor()
{
    if ( InkCanvasEditingMode::EraseByPoint == _cachedEraseMode )
    {
        if ( GetEditingCoordinator().UserIsEditing() == false )
        {
            return QCursor(Qt::ArrowCursor);
        }
        if ( _cachedStylusShape == nullptr )
        {
            _cachedStylusShape = GetInkCanvas().EraserShape();

            // NTRAID-WINDOWS#1430638-2006/01/04-WAYNEZEN,
            // The eraser QCursor should respect the InkCanvas' Transform properties as the pen tip.
            QMatrix xf = GetElementTransformMatrix();
            if ( !xf.isIdentity() )
            {
                // Zero the offsets if the element's transform in invertable.
                // Otherwise fallback the matrix to the identity.
                if ( xf.isInvertible() )
                {
                    //xf.OffsetX = 0;
                    //xf.OffsetY = 0;
                    xf = QMatrix(xf.m11(), xf.m12(), xf.m21(), xf.m22(), 0, 0);
                }
                else
                {
                    xf = QMatrix();
                }
            }
            //DpiScale dpi = GetInkCanvas().GetDpi();
            qreal dpi = QApplication::primaryScreen()->devicePixelRatio();
            _cachedPointEraserCursor = PenCursorManager::GetPointEraserCursor2(*_cachedStylusShape, xf, dpi, dpi);
        }

        return _cachedPointEraserCursor;
    }
    else
    {
        return PenCursorManager::GetStrokeEraserCursor();
    }
}

/// <summary>
/// ApplyTransformToCursor
/// </summary>
void EraserBehavior::OnTransformChanged()
{
    // Reset the cached point eraser cursor.
    ResetCachedPointEraserCursor();
}

//#endregion Protected Methods

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods

/// <summary>
/// Reset the cached point eraser cursor.
/// </summary>
void EraserBehavior::ResetCachedPointEraserCursor()
{
    _cachedPointEraserCursor = QCursor();
    _cachedStylusShape = nullptr;
}

/// <summary>
///
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void EraserBehavior::OnStrokeEraseResultChanged(StrokeHitEventArgs& e)
{
    Debug::Assert(nullptr != e.HitStroke());

    bool fSucceeded = false;

    // The below code calls out StrokeErasing or StrokeErased event.
    // The out-side code could throw exception.
    // We use try/finally block to protect our status.

    FinallyHelper final([this, &fSucceeded](){
        if ( !fSucceeded )
        {
            // Abort the editing.
            Commit(false);
        }
    });
    //try
    {
        InkCanvasStrokeErasingEventArgs args(e.HitStroke());
        GetInkCanvas().RaiseStrokeErasing(args);

        if ( !args.Cancel() )
        {
            // Erase only if the event wasn't cancelled
            GetInkCanvas().Strokes()->RemoveItem(e.HitStroke());
            GetInkCanvas().RaiseInkErased();
        }

        fSucceeded = true;
    }
    //finally
    //{
    //    if ( !fSucceeded )
    //    {
            // Abort the editing.
    //        Commit(false);
    //    }
    //}
}

/// <summary>
///
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void EraserBehavior::OnPointEraseResultChanged(StrokeHitEventArgs& e)
{
    Debug::Assert(nullptr != e.HitStroke(), "e.HitStroke cannot be nullptr");

    bool fSucceeded = false;

    // The below code might call out StrokeErasing or StrokeErased event.
    // The out-side code could throw exception.
    // We use try/finally block to protect our status.
    FinallyHelper final([this, &fSucceeded](){
        if ( !fSucceeded )
        {
            // Abort the editing.
            Commit(false);
        }
    });
    //try
    {

        InkCanvasStrokeErasingEventArgs args(e.HitStroke());
        GetInkCanvas().RaiseStrokeErasing(args);

        if ( !args.Cancel() )
        {
            if (_clipStroke) {
                QList<StrokeIntersection> clipAt;
                if (_clipStroke->Bounds().intersects(e.HitStroke()->GetBounds())
                        && _clipStroke->EraseTest(StrokeNodeIterator::GetIterator(*e.HitStroke(), *e.HitStroke()->GetDrawingAttributes()), clipAt)) {
                    QVector<StrokeIntersection> vec(clipAt.toVector());
                    e.Clip(vec);
                }
            }

            // Erase only if the event wasn't cancelled
            QSharedPointer<StrokeCollection> eraseResult = e.GetPointEraseResults();
            Debug::Assert(eraseResult != nullptr, "eraseResult cannot be nullptr");

            QSharedPointer<StrokeCollection> strokesToReplace(new StrokeCollection());
            strokesToReplace->AddItem(e.HitStroke());

            try
            {
                // replace or remove the stroke
                if (eraseResult->size() > 0)
                {
                    GetInkCanvas().Strokes()->Replace(strokesToReplace, eraseResult);
                }
                else
                {
                    GetInkCanvas().Strokes()->Remove(strokesToReplace);
                }
            }
            catch (std::exception ex)
            {
                //this can happen if someone sits in an event handler
                //for StrokeErasing and removes the stroke.
                //this to harden against failure here.
                //if (!ex.Data.Contains("System.Windows.Ink.StrokeCollection"))
                //{
                    //System.Windows.Ink.StrokeCollection didn't throw this,
                    //we need to just throw the original exception
                    throw;
                //}
            }


            //raise ink erased
            GetInkCanvas().RaiseInkErased();
        }

        fSucceeded = true;
    }
    //finally
    //{
    //    if ( !fSucceeded )
    //    {
            // Abort the editing.
    //        Commit(false);
    //    }
    //}
}
