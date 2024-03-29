#include "Internal/Ink/styluseditingbehavior.h"
#include "Internal/finallyhelper.h"

INKCANVAS_BEGIN_NAMESPACE

//-------------------------------------------------------------------------------
//
// Constructors
//
//-------------------------------------------------------------------------------

//#region Constructors

/// <summary>
/// Constructor
/// </summary>
/// <param name="editingCoordinator"></param>
/// <param name="inkCanvas"></param>
StylusEditingBehavior::StylusEditingBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : EditingBehavior(editingCoordinator, inkCanvas)
{
}

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
void StylusEditingBehavior::SwitchToMode(InkCanvasEditingMode mode)
{
    // NTRAID:WINDOWSOS#1464481-2006/01/30/-WAYNEZEN,
    // The dispather frames can be entered. If one calls InkCanvas.Select/Paste from a dispather frame
    // during the user editing, this method will be called. But before the method is processed completely,
    // the user input could kick in AddStylusPoints. So GetEditingCoordinator().UserIsEditing() flag may be messed up.
    // Now we use _disableInput to disable the input during changing the mode in mid-stroke.
    _disableInput = true;
    FinallyHelper final([this](){
        _disableInput = false;
    });
    //try
    {
        OnSwitchToMode(mode);
    }
    //finally
    //{
    //    _disableInput = false;
    //}
}

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
void StylusEditingBehavior::AddStylusPoints(SharedPointer<StylusPointCollection> stylusPoints, bool userInitiated)
{
    GetEditingCoordinator().DebugCheckActiveBehavior(this);

    // Don't process if SwitchToMode is called during the mid-stroke.
    if ( _disableInput )
    {
        return;
    }

    if ( !GetEditingCoordinator().UserIsEditing() )
    {
        GetEditingCoordinator().SetUserIsEditing(true);
        StylusInputBegin(stylusPoints, userInitiated);
    }
    else
    {
        StylusInputContinue(stylusPoints, userInitiated);
    }
}

//#endregion IStylusEditing Interface

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods


/// <summary>
/// OnCommit
/// </summary>
/// <param name="commit"></param>
void StylusEditingBehavior::OnCommit(bool commit)
{
    // Make sure that user is still editing
    if ( GetEditingCoordinator().UserIsEditing() )
    {
        GetEditingCoordinator().SetUserIsEditing(false);

        // The follow code raises variety editing events.
        // The out-side code could throw exception in the their handlers. We use try/finally block to protect our status.
        StylusInputEnd(commit);
    }
    else
    {
        // If user isn't editing, we should still call the derive class.
        // So the dynamic behavior like LSB can be self deactivated when it has been commited.
        OnCommitWithoutStylusInput(commit);
    }
}

INKCANVAS_END_NAMESPACE
