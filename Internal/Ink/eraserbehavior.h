#ifndef ERASERBEHAVIOR_H
#define ERASERBEHAVIOR_H

#include "Internal/Ink/styluseditingbehavior.h"
#include "Windows/Ink/incrementalhittester.h"

class StylusShape;
class IncrementalStrokeHitTester;
class StrokeHitEventArgs;
class ErasingStroke;
class QPolygonF;

// namespace MS.Internal.Ink

/// <summary>
/// Eraser Behavior
/// </summary>
class EraserBehavior : public StylusEditingBehavior
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors

public:
    EraserBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

    //#endregion Constructors

    void SetClip(QPolygonF const & shape);

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

protected:
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
    virtual void OnSwitchToMode(InkCanvasEditingMode mode);

    virtual void OnActivate();

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
    /// Get eraser cursor.
    /// </summary>
    /// <returns></returns>
    virtual QCursor GetCurrentCursor();

    /// <summary>
    /// ApplyTransformToCursor
    /// </summary>
    virtual void OnTransformChanged();

    //#endregion Protected Methods

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

private:
    /// <summary>
    /// Reset the cached point eraser cursor.
    /// </summary>
    void ResetCachedPointEraserCursor();

    /// <summary>
    ///
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnStrokeEraseResultChanged(StrokeHitEventArgs& e);

    /// <summary>
    ///
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnPointEraseResultChanged(StrokeHitEventArgs& e);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

    //#region Fields

private:
    InkCanvasEditingMode            _cachedEraseMode;
    std::unique_ptr<IncrementalStrokeHitTester>      _incrementalStrokeHitTester = nullptr;
    QCursor                          _cachedPointEraserCursor;
    StylusShape*                     _cachedStylusShape = nullptr;
    QSharedPointer<StylusPointCollection>           _stylusPoints = nullptr;
    std::unique_ptr<ErasingStroke> _clipStroke;

    //#endregion Fields
};

#endif // ERASERBEHAVIOR_H
