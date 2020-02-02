#ifndef INKCOLLECTIONBEHAVIOR_H
#define INKCOLLECTIONBEHAVIOR_H

#include "styluseditingbehavior.h"

#include <QMap>

class DrawingAttributes;

// namespace MS.Internal.Ink

/// <summary>
/// InkCollectionBehavior
/// </summary>
/// <SecurityNote>
///     This class is sealed, I was worried that StylusEndInput could be overridden
///     and bypass the security check, but it turns out that concern is not valid.
///     Even if StylusEndInput was overridden and called the base method, we're still fine
///     here since we don't make a security decision based on the bool that is passed
///     to the method.  We make the security decision based on the _userInitiated security critical
///     member.  All the same, we'll leave this class as sealed.
/// </SecurityNote>
class InkCollectionBehavior : public StylusEditingBehavior
{
public:
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors

    /// <SecurityNote>
    ///     Critical: Touches critical member _stylusPoints, which is passed to
    ///         SecurityCritical method GetInkCanvas().RaiseGestureOrStrokeCollected
    ///
    ///     TreatAsSafe: only initializes _stylusPoints to nullptr , _userInitialize to false
    ///
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    InkCollectionBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

    //#endregion Constructors

    /// <summary>
    /// A method which flags InkCollectionBehavior when it needs to reset the dynamic renderer.
    ///    Called By:
    ///         GetEditingCoordinator().OnInkCanvasDeviceDown
    /// </summary>
    void ResetDynamicRenderer();

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

protected:
    /// <summary>
    /// Overrides SwitchToMode
    /// As the following expected results
    ///  1. From Ink To InkAndGesture
    ///     Packets between StylusDown and StylusUp are sent to the gesture reco. On StylusUp gesture event fires. If it’s not a gesture, StrokeCollected event fires.
    ///  2. From Ink To GestureOnly
    ///     Packets between StylusDown and StylusUp are send to the gesture reco. On StylusUp gesture event fires. Stroke gets removed on StylusUp even if it’s not a gesture.
    ///  3. From Ink To EraseByPoint
    ///     Stroke is discarded. PointErasing is performed after changing the mode.
    ///  4. From Ink To EraseByStroke
    ///      Stroke is discarded. StrokeErasing is performed after changing the mode.
    ///  5. From Ink To Select
    ///     Stroke is discarded. Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
    ///  6. From Ink To None
    ///     Stroke is discarded.
    ///  7. From InkAndGesture To Ink
    ///     Stroke is collected for all packets between StylusDown and StylusUp. Gesture event does not fire.
    ///  8. From InkAndGesture To GestureOnly
    ///     Packets between StylusDown and StylusUp are sent to the gesture reco. Stroke gets removed on StylusUp even if it’s not a gesture.
    ///  9. From InkAndGesture To EraseByPoint
    ///     Stroke is discarded. PointErasing is performed after changing the mode, gesture event does not fire.
    /// 10. From InkAndGesture To EraseByStroke
    ///     Stroke is discarded. StrokeErasing is performed after changing the mode, gesture event does not fire.
    /// 11. From InkAndGesture To Select
    ///     Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
    /// 12. From InkAndGesture To None
    ///     Stroke is discarded, no gesture is recognized.
    /// 13. From GestureOnly To InkAndGesture
    ///     Packets between StylusDown and StylusUp are sent to the gesture reco. On StylusUp gesture event fires. If it’s not a gesture, StrokeCollected event fires.
    /// 14. From GestureOnly To Ink
    ///     Stroke is collected. Gesture event does not fire.
    /// 15. From GestureOnly To EraseByPoint
    ///     Stroke is discarded PointErasing is performed after changing the mode, gesture event does not fire
    /// 16. From GestureOnly To EraseByStroke
    ///     Stroke is discarded. StrokeErasing is performed after changing the mode, gesture event does not fire.
    /// 17. From GestureOnly To Select
    ///     Lasso is drawn for all packets between StylusDown and StylusUp. Strokes/elements within the lasso will be selected.
    /// 18. From GestureOnly To None
    ///     Stroke is discarded. Gesture event does not fire.
    /// </summary>
    /// <param name="mode"></param>
    /// <SecurityNote>
    ///     Critical: Clones SecurityCritical member _stylusPoints, which is only critical
    ///         as an argument in StylusInputEnd to critical method GetInkCanvas().RaiseGestureOrStrokeCollected
    ///
    ///     TreatAsSafe: This method simply clones critical member _stylusPoints and passes that to transparent code
    ///         _stylusPoints is only critical when passed to GetInkCanvas().RaiseGestureOrStrokeCollected.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    virtual void OnSwitchToMode(InkCanvasEditingMode mode);

    /// <summary>
    /// OnActivate
    /// </summary>
    virtual void OnActivate();

    /// <summary>
    /// OnDeactivate
    /// </summary>
    virtual void OnDeactivate();

    /// <summary>
    /// Get Pen Cursor
    /// </summary>
    /// <returns></returns>
    virtual QCursor GetCurrentCursor();


    /// <summary>
    /// StylusInputBegin
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
    /// <SecurityNote>
    ///     Critical: Constructs SecurityCritical member _stylusPoints, which is passed
    ///         as an argument in StylusInputEnd to critical method GetInkCanvas().RaiseGestureOrStrokeCollected
    ///
    ///         Also accesses critical member _userInitiated, which is used to determine if all of the
    ///         stylusPoints were user initiated
    /// </SecurityNote>
    //[SecurityCritical]
    virtual void StylusInputBegin(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated);

    /// <summary>
    /// StylusInputContinue
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the source eventArgs.UserInitiated flag was set to true</param>
    /// <SecurityNote>
    ///     Critical: Adds to SecurityCritical member _stylusPoints, which are passed
    ///         as an argument in StylusInputEnd to critical method GetInkCanvas().RaiseGestureOrStrokeCollected
    ///
    ///         Also accesses critical member _userInitiated, which is used to determine if all of the
    ///         stylusPoints were user initiated
    /// </SecurityNote>
    //[SecurityCritical]
    virtual void StylusInputContinue(QSharedPointer<StylusPointCollection> stylusPoints, bool userInitiated);

    /// <summary>
    /// StylusInputEnd
    /// </summary>
    /// <param name="commit">commit</param>
    /// <SecurityNote>
    ///     Critical: Calls SecurityCritical method GetInkCanvas().RaiseGestureOrStrokeCollected with
    ///         critical data _stylusPoints
    ///
    ///         Also accesses critical member _userInitiated, which is used to determine if all of the
    ///         stylusPoints were user initiated
    ///
    ///     TreatAsSafe: The critical methods that build up the critical argument _stylusPoints which
    ///         is passed to critical method GetInkCanvas().RaiseGestureOrStrokeCollected are already marked
    ///         critical.  No critical data is passed in this method.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
    virtual void StylusInputEnd(bool commit);

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

private:
    //#region Methods

    QCursor PenCursor();

    //#endregion Methods


    //-------------------------------------------------------------------------------
    //
    // Fields
    //
    //-------------------------------------------------------------------------------

    //#region Fields

private:
    // A flag indicates that the dynamic renderer needs to be reset when StylusInputs begins.
    bool                                           _resetDynamicRenderer;

    /// <SecurityNote>
    ///     Critical: This is critical data passed as an argument to
    ///         critical method GetInkCanvas().RaiseGestureOrStrokeCollected.
    /// </SecurityNote>
    //[SecurityCritical]
    QMap<int, QSharedPointer<StylusPointCollection>>                           _stylusPoints;

    /// <SecurityNote>
    ///     Critical: We use this to track if the input that makes up _stylusPoints
    ///         100% came frome the user (using the RoutedEventArgs.UserInitiated property)
    /// </SecurityNote>
    //[SecurityCritical]
    bool                                            _userInitiated;

    /// <summary>
    /// We clone the GetInkCanvas().DefaultDrawingAttributes on down, in case they are
    /// changed mid-stroke
    /// </summary>
    QSharedPointer<DrawingAttributes>                               _strokeDrawingAttributes;

    /// <summary>
    /// The cached DrawingAttributes and Cursor instances for rendering the pen cursor.
    /// </summary>
    QSharedPointer<DrawingAttributes>                               _cursorDrawingAttributes;
    QCursor                                          _cachedPenCursor;

    //#endregion Fields
};

#endif // INKCOLLECTIONBEHAVIOR_H
