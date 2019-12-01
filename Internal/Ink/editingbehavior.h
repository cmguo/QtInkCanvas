#ifndef EDITINGBEHAVIOR_H
#define EDITINGBEHAVIOR_H

#include "editingcoordinator.h"

class EditingCoordinator;
class InkCanvas;

#include <QCursor>

#include <exception>

// namespace MS.Internal.Ink

/// <summary>
/// Base class for all EditingBehaviors in the InkCanvas
/// Please see the design detain at http://tabletpc/longhorn/Specs/Mid-Stroke%20and%20Pen%20Cursor%20Dev%20Design.mht
/// </summary>
class EditingBehavior : public QObject
{
    Q_OBJECT

public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="editingCoordinator">EditngCoordinator instance</param>
    /// <param name="inkCanvas">InkCanvas instance</param>
    EditingBehavior(EditingCoordinator& editingCoordinator, InkCanvas& inkCanvas);

    virtual ~EditingBehavior();

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods

    /// <summary>
    /// An method which should be called from:
    ///     GetEditingCoordinator()->PushEditingBehavior
    ///     GetEditingCoordinator()->PopEditingBehavior
    /// The mehod's called when the behavior switching occurs.
    /// </summary>
    void Activate();

    /// <summary>
    /// An method which should be called from:
    ///     GetEditingCoordinator()->PushEditingBehavior
    ///     GetEditingCoordinator()->PopEditingBehavior
    /// The mehod's called when the behavior switching occurs.
    /// </summary>
    void Deactivate();

    /// <summary>
    /// An method which should be called from:
    ///     GetEditingCoordinator()->OnInkCanvasStylusUp
    ///     GetEditingCoordinator()->OnInkCanvasLostStylusCapture
    ///     GetEditingCoordinator()->UpdateEditingState
    /// The mehod's called when the current editing state is committed or discarded.
    /// </summary>
    /// <param name="commit">A flag which indicates either editing is committed or discarded</param>
    void Commit(bool commit);

    /// <summary>
    /// UpdateTransform
    ///     Called by: GetEditingCoordinator()->InvalidateBehaviorCursor
    /// </summary>
    void UpdateTransform();

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
    ///     GetEditingCoordinator()->InvalidateQCursor
    ///     GetEditingCoordinator()->PushEditingBehavior
    ///     GetEditingCoordinator()->PopEditingBehavior
    /// </summary>
    /// <returns>The current QCursor</returns>
    QCursor Cursor();


    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

protected:
    /// <summary>
    /// Called when the EditingBehavior is activated.  EditingBehaviors
    /// should register for event handlers in this call
    /// Called from:
    ///     EditingBehavior.Activate
    /// </summary>
    virtual void OnActivate() = 0;

    /// <summary>
    /// Called when the EditingBehavior is deactivated.  EditingBehaviors
    /// should unregister from event handlers in this call
    /// Called from:
    ///     EditingBehavior.Deactivate
    /// </summary>
    virtual void OnDeactivate() = 0;

    /// <summary>
    /// Called when the user editing is committed or discarded.
    /// Called from:
    ///     EditingBehavior.Commit
    /// </summary>
    /// <param name="commit">A flag which indicates either editing is committed or discarded</param>
    virtual void OnCommit(bool commit) = 0;

    /// <summary>
    /// Called when the new Cursor instance is required.
    /// Called from:
    ///     EditingBehavior.GetCursor
    /// </summary>
    /// <returns></returns>
    virtual QCursor GetCurrentCursor() = 0;

    /// <summary>
    /// Unload the dynamic behavior. The method should be only called from:
    ///     SelectionEditingBehavior
    ///     LassoSelectionBehavior
    /// </summary>
    void SelfDeactivate();

    /// <summary>
    /// Calculate the transform which is accumalated with the InkCanvas' XXXTransform properties.
    /// </summary>
    /// <returns></returns>
    QMatrix GetElementTransformMatrix();

    /// <summary>
    /// OnTransformChanged
    /// </summary>
    virtual void OnTransformChanged();

    //#endregion Protected Methods

    //-------------------------------------------------------------------------------
    //
    // Protected Properties
    //
    //-------------------------------------------------------------------------------

    //#region Protected Properties

    /// <summary>
    /// Provides access to the InkCanvas this EditingBehavior is attached to
    /// </summary>
    /// <value></value>
    InkCanvas& GetInkCanvas()
    {
        return _inkCanvas;
    }

    /// <summary>
    /// Provides access to the EditingStack this EditingBehavior is on
    /// </summary>
    /// <value></value>
    EditingCoordinator& GetEditingCoordinator()
    {
        return _editingCoordinator;
    }

    //#endregion Protected Properties


    //-------------------------------------------------------------------------------
    //
    // Protected Fields
    //
    //-------------------------------------------------------------------------------

    //#region Private Fields

private:
    /// <summary>
    /// The InkCanvas this EditingBehavior is operating on
    /// </summary>
    InkCanvas&           _inkCanvas;

    /// <summary>
    /// The EditingStack this EditingBehavior is on
    /// </summary>
    EditingCoordinator&  _editingCoordinator;

    /// <summary>
    /// Fields related to the Cursor
    /// </summary>
    QCursor              _cachedCursor;
};

#endif // EDITINGBEHAVIOR_H
