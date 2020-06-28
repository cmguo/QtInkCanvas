#ifndef STYLUSEDITINGBEHAVIOR_H
#define STYLUSEDITINGBEHAVIOR_H

#include "editingbehavior.h"
#include "sharedptr.h"
#include "Collections/Generic/list.h"

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class StylusPointCollection;

/// <summary>
/// IStylusEditing Interface
/// </summary>
class IStylusEditing
{
public:
    virtual ~IStylusEditing() {}
    /// <summary>
    /// AddStylusPoints
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">only true if eventArgs.UserInitiated is true</param>
    virtual void AddStylusPoints(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated) = 0;
};

/// <summary>
/// StylusEditingBehavior - a base class for all stylus related editing behaviors
/// </summary>
class StylusEditingBehavior : public EditingBehavior, public IStylusEditing
{
    Q_OBJECT
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors

public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="editingCoordinator"></param>
    /// <param name="inkCanvas"></param>
    StylusEditingBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

    /// <summary>
    /// An method which performs a mode change in mid-stroke.
    /// </summary>
    /// <param name="mode"></param>
    void SwitchToMode(InkCanvasEditingMode mode);

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // IStylusEditing Interface
    //
    //-------------------------------------------------------------------------------

    //#region IStylusEditing Interface

    /// <summary>
    /// IStylusEditing.AddStylusPoints
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the eventArgs source had UserInitiated set to true</param>
    /// <SecurityNote>
    ///     Critical: Calls critical methods StylusInputBegin and StylusInputContinue
    ///
    ///     Note that for InkCollectionBehavior, which inherits from this class, the stylusPoints
    ///     passed through this API are critical.  For EraserBehavior and LassoSelectionBehavior, which
    ///     also inherit from this class, the stylusPoints are not critical.  This is because only
    ///     InkCollectionBehavior calls a critical method with the stylusPoints as an argument.
    /// </SecurityNote>
    //[SecurityCritical]
    virtual void AddStylusPoints(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated);

    //#endregion IStylusEditing Interface

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

    /// <summary>
    /// An abstract method which performs a mode change in mid-stroke.
    /// </summary>
    /// <param name="mode"></param>
    virtual void OnSwitchToMode(InkCanvasEditingMode mode) = 0;

    /// <summary>
    /// Called when the InkEditingBehavior is activated.
    /// </summary>
    virtual void OnActivate()
    {
    }

    /// <summary>
    /// Called when the InkEditingBehavior is deactivated.
    /// </summary>
    virtual void OnDeactivate()
    {
    }

    /// <summary>
    /// OnCommit
    /// </summary>
    /// <param name="commit"></param>
    virtual void OnCommit(bool commit);

    /// <summary>
    /// StylusInputBegin
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the source eventArgs.UserInitiated flag is true</param>
    /// <SecurityNote>
    ///     Critical: Handles critical data stylusPoints, which is critical
    ///         when InkCollectionBehavior uses it to call critical method
    ///         InkCanvas.RaiseGestureOrStrokeCollected
    ///
    ///     Note that for InkCollectionBehavior, which inherits from this class, the stylusPoints
    ///     passed through this API are critical.  For EraserBehavior and LassoSelectionBehavior, which
    ///     also inherit from this class, the stylusPoints are not critical.  This is because only
    ///     InkCollectionBehavior calls a critical method with the stylusPoints as an argument.
    /// </SecurityNote>
    //[SecurityCritical]
    virtual void StylusInputBegin(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
    {
        (void)stylusPoints;
        (void)userInitiated;
        //defer to derived classes
    }

    /// <summary>
    /// StylusInputContinue
    /// </summary>
    /// <param name="stylusPoints">stylusPoints</param>
    /// <param name="userInitiated">true if the source eventArgs.UserInitiated flag is true</param>
    /// <SecurityNote>
    ///     Critical: Handles critical data stylusPoints, which is critical
    ///         when InkCollectionBehavior uses it to call critical method InkCanvas.RaiseGestureOrStrokeCollected
    ///
    ///     Note that for InkCollectionBehavior, which inherits from this class, the stylusPoints
    ///     passed through this API are critical.  For EraserBehavior and LassoSelectionBehavior, which
    ///     also inherit from this class, the stylusPoints are not critical.  This is because only
    ///     InkCollectionBehavior calls a critical method with the stylusPoints as an argument.
    /// </SecurityNote>
    //[SecurityCritical]
    virtual void StylusInputContinue(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
    {
        (void)stylusPoints;
        (void)userInitiated;
        //defer to derived classes
    }

    /// <summary>
    /// StylusInputEnd
    /// </summary>
    /// <param name="commit"></param>
    virtual void StylusInputEnd(bool commit)
    {
        (void)commit;
        //defer to derived classes
    }

    /// <summary>
    /// OnCommitWithoutStylusInput
    /// </summary>
    /// <param name="commit"></param>
    virtual void OnCommitWithoutStylusInput(bool commit)
    {
        (void)commit;
        //defer to derived classes
    }

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------

    //#region Private Fields

private:
    bool    _disableInput = false;  // No need for initializing. The default value is false.

    //#endregion Private Fields
};

INKCANVAS_END_NAMESPACE

#endif // STYLUSEDITINGBEHAVIOR_H
