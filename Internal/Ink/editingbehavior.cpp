#include "Internal/Ink/editingbehavior.h"
#include "Windows/Controls/inkcanvas.h"

/// <summary>
/// Constructor
/// </summary>
/// <param name="editingCoordinator">EditngCoordinator instance</param>
/// <param name="inkCanvas">InkCanvas instance</param>
EditingBehavior::EditingBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas)
    : _inkCanvas(inkCanvas)
    , _editingCoordinator(editingCoordinator)
{
    //if (inkCanvas == nullptr)
    //{
    //    throw std::exception("inkCanvas");
    //}
    //if (editingCoordinator == nullptr)
    //{
    //    throw std::exception("editingCoordinator");
    //}
    //_inkCanvas = inkCanvas;
    //_editingCoordinator = editingCoordinator;
}

EditingBehavior::~EditingBehavior()
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
/// An method which should be called from:
///     GetEditingCoordinator().PushEditingBehavior
///     GetEditingCoordinator().PopEditingBehavior
/// The mehod's called when the behavior switching occurs.
/// </summary>
void EditingBehavior::Activate()
{
    // Debug verification which will never be compiled into release bits.
    GetEditingCoordinator().DebugCheckActiveBehavior(this);

    // Invoke the virtual OnActivate method.
    OnActivate();
}

/// <summary>
/// An method which should be called from:
///     GetEditingCoordinator().PushEditingBehavior
///     GetEditingCoordinator().PopEditingBehavior
/// The mehod's called when the behavior switching occurs.
/// </summary>
void EditingBehavior::Deactivate()
{
    // Debug verification which will never be compiled into release bits.
    GetEditingCoordinator().DebugCheckActiveBehavior(this);

    // Invoke the virtual OnDeactivate method.
    OnDeactivate();
}

/// <summary>
/// An method which should be called from:
///     GetEditingCoordinator().OnInkCanvasStylusUp
///     GetEditingCoordinator().OnInkCanvasLostStylusCapture
///     GetEditingCoordinator().UpdateEditingState
/// The mehod's called when the current editing state is committed or discarded.
/// </summary>
/// <param name="commit">A flag which indicates either editing is committed or discarded</param>
void EditingBehavior::Commit(bool commit)
{
    // Debug verification which will never be compiled into release bits.
    GetEditingCoordinator().DebugCheckActiveBehavior(this);

    // Invoke the virtual OnCommit method.
    OnCommit(commit);
}

/// <summary>
/// UpdateTransform
///     Called by: GetEditingCoordinator().InvalidateBehaviorCursor
/// </summary>
void EditingBehavior::UpdateTransform()
{
    if ( !GetEditingCoordinator().IsTransformValid(this) )
    {
        OnTransformChanged();
    }
}

//#endregion Methods

//-------------------------------------------------------------------------------
//
// Properties
//
//-------------------------------------------------------------------------------

//#region Properties

/// <summary>
/// Retrieve the QCursor which is associated to this behavior.
/// The method which should be called from:
///     GetEditingCoordinator().InvalidateQCursor
///     GetEditingCoordinator().PushEditingBehavior
///     GetEditingCoordinator().PopEditingBehavior
/// </summary>
/// <returns>The current QCursor</returns>
QCursor EditingBehavior::Cursor()
{
     // If the QCursor instance hasn't been created or is dirty, we will create a new instance.
     if ( !GetEditingCoordinator().IsCursorValid(this) )
     {
         // Invoke the virtual method to get QCursor. Then cache it.
         _cachedCursor = GetCurrentCursor();
     }
     return _cachedCursor;
}


/// <summary>
/// Unload the dynamic behavior. The method should be only called from:
///     SelectionEditingBehavior
///     LassoSelectionBehavior
/// </summary>
void EditingBehavior::SelfDeactivate()
{
    GetEditingCoordinator().DeactivateDynamicBehavior();
}

/// <summary>
/// Calculate the transform which is accumalated with the InkCanvas' XXXTransform properties.
/// </summary>
/// <returns></returns>
QMatrix EditingBehavior::GetElementTransformMatrix()
{
    QTransform layoutTransform = GetInkCanvas().LayoutTransform();
    QTransform renderTransform = GetInkCanvas().RenderTransform();
    QTransform xf = layoutTransform;
    xf *= renderTransform;

    return xf.toAffine();
}

/// <summary>
/// OnTransformChanged
/// </summary>
void EditingBehavior::OnTransformChanged()
{
    // Defer to derived.
}

//#endregion Protected Methods


