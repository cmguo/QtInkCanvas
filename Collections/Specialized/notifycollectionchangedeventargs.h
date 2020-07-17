#ifndef NOTIFYCOLLECTIONCHANGEDEVENTARGS_H
#define NOTIFYCOLLECTIONCHANGEDEVENTARGS_H

#include "eventargs.h"
#include "variant.h"
#include "Collections/Generic/list.h"

INKCANVAS_BEGIN_NAMESPACE

enum NotifyCollectionChangedAction
{
    /// <summary> One or more items were added to the collection. </summary>
    Add,
    /// <summary> One or more items were removed from the collection. </summary>
    Remove,
    /// <summary> One or more items were replaced in the collection. </summary>
    Replace,
    /// <summary> One or more items were moved within the collection. </summary>
    Move,
    /// <summary> The contents of the collection changed dramatically. </summary>
    Reset,
};

// namespace System.Collections.Specialized

class NotifyCollectionChangedEventArgs : public EventArgs
{
public:
    //------------------------------------------------------
    //
    //  Constructors
    //
    //------------------------------------------------------
    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a reset change.
    /// </summary>
    /// <param name="action">The action that caused the event (must be Reset).</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action)
    {
        if (action != NotifyCollectionChangedAction::Reset)
            throw std::runtime_error("action");

        InitializeAdd(action, List<Variant>(), -1);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item change.
    /// </summary>
    /// <param name="action">The action that caused the event; can only be Reset, Add or Remove action.</param>
    /// <param name="changedItem">The item affected by the change.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, Variant changedItem)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::runtime_error("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (changedItem == nullptr)
                throw std::runtime_error("action");

            InitializeAdd(action, List<Variant>(), -1);
        }
        else
        {
            InitializeAddOrRemove(action, List<Variant>({changedItem}), -1);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item change.
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItem">The item affected by the change.</param>
    /// <param name="index">The index where the change occurred.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, Variant changedItem, int index)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::runtime_error("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (changedItem == nullptr)
                throw std::runtime_error("action");
            if (index != -1)
                throw std::runtime_error("action");

            InitializeAdd(action, List<Variant>(), -1);
        }
        else
        {
            InitializeAddOrRemove(action, List<Variant>({changedItem}), index);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item change.
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItems">The items affected by the change.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, List<Variant> changedItems)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::runtime_error("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (changedItems.Count() == 0)
                throw std::runtime_error("action");

            InitializeAdd(action, List<Variant>(), -1);
        }
        else
        {
            if (changedItems.Count() == 0)
                throw std::runtime_error("changedItems");

            InitializeAddOrRemove(action, changedItems, -1);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item change (or a reset).
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItems">The items affected by the change.</param>
    /// <param name="startingIndex">The index where the change occurred.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, List<Variant> changedItems, int startingIndex)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::runtime_error("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (changedItems.Count() == 0)
                throw std::runtime_error("action");
            if (startingIndex != -1)
                throw std::runtime_error("action");

            InitializeAdd(action, List<Variant>(), -1);
        }
        else
        {
            if (changedItems.Count() == 0)
                throw std::runtime_error("changedItems");
            if (startingIndex < -1)
                throw std::runtime_error("startingIndex");

            InitializeAddOrRemove(action, changedItems, startingIndex);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItem">The new item replacing the original item.</param>
    /// <param name="oldItem">The original item that is replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, Variant newItem, Variant oldItem)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::runtime_error("action");

        InitializeMoveOrReplace(action, List<Variant>({newItem}), List<Variant>({oldItem}), -1, -1);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItem">The new item replacing the original item.</param>
    /// <param name="oldItem">The original item that is replaced.</param>
    /// <param name="index">The index of the item being replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, Variant newItem, Variant oldItem, int index)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::runtime_error("action");

        int oldStartingIndex = index;

#if FEATURE_LEGACYNETCF
        if (CompatibilitySwitches.IsAppEarlierThanWindowsPhone8)
        {
            // Dev11 444113 quirk:
            // This is a "Replace" so the old and new index should both be set to the index passed in however
            // NetCF on Mango incorrectly leaves OldStartingIndex at -1 and Mango apps depend on this behavior.
            oldStartingIndex = -1;
        }
#endif
        InitializeMoveOrReplace(action, List<Variant>({newItem}), List<Variant>({oldItem}), index, oldStartingIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItems">The new items replacing the original items.</param>
    /// <param name="oldItems">The original items that are replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, List<Variant> newItems, List<Variant> oldItems)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::runtime_error("action");
        if (newItems.Count() == 0)
            throw std::runtime_error("newItems");
        if (oldItems.Count() == 0)
            throw std::runtime_error("oldItems");

        InitializeMoveOrReplace(action, newItems, oldItems, -1, -1);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItems">The new items replacing the original items.</param>
    /// <param name="oldItems">The original items that are replaced.</param>
    /// <param name="startingIndex">The starting index of the items being replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, List<Variant> newItems, List<Variant> oldItems, int startingIndex)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::runtime_error("action");
        if (newItems.Count() == 0)
            throw std::runtime_error("newItems");
        if (oldItems.Count() == 0)
            throw std::runtime_error("oldItems");

        InitializeMoveOrReplace(action, newItems, oldItems, startingIndex, startingIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item Move event.
    /// </summary>
    /// <param name="action">Can only be a Move action.</param>
    /// <param name="changedItem">The item affected by the change.</param>
    /// <param name="index">The new index for the changed item.</param>
    /// <param name="oldIndex">The old index for the changed item.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, Variant changedItem, int index, int oldIndex)
    {
        if (action != NotifyCollectionChangedAction::Move)
            throw std::runtime_error("action");
        if (index < 0)
            throw std::runtime_error("index");

        List<Variant> changedItems= {changedItem};
        InitializeMoveOrReplace(action, changedItems, changedItems, index, oldIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item Move event.
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItems">The items affected by the change.</param>
    /// <param name="index">The new index for the changed items.</param>
    /// <param name="oldIndex">The old index for the changed items.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, List<Variant> changedItems, int index, int oldIndex)
    {
        if (action != NotifyCollectionChangedAction::Move)
            throw std::runtime_error("action");
        if (index < 0)
            throw std::runtime_error("index");

        InitializeMoveOrReplace(action, changedItems, changedItems, index, oldIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs with given fields (no validation). Used by WinRT marshaling.
    /// </summary>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, List<Variant> newItems, List<Variant> oldItems, int newIndex, int oldIndex)
    {
        _action = action;
#if FEATURE_LEGACYNETCF
        if (CompatibilitySwitches.IsAppEarlierThanWindowsPhone8)
        {
            _newItems = newItems;
        }
        else
#endif
        {
            _newItems = newItems;
        }
        _oldItems = oldItems;
        _newStartingIndex = newIndex;
        _oldStartingIndex = oldIndex;
    }

private:
    void InitializeAddOrRemove(NotifyCollectionChangedAction action, List<Variant> changedItems, int startingIndex)
    {
        if (action == NotifyCollectionChangedAction::Add)
            InitializeAdd(action, changedItems, startingIndex);
        else if (action == NotifyCollectionChangedAction::Remove)
            InitializeRemove(action, changedItems, startingIndex);
        else
            throw std::runtime_error("Unsupported action");
    }

    void InitializeAdd(NotifyCollectionChangedAction action, List<Variant> newItems, int newStartingIndex)
    {
        _action = action;
#if FEATURE_LEGACYNETCF
        if (CompatibilitySwitches.IsAppEarlierThanWindowsPhone8)
        {
            _newItems = newItems;
        }
        else
#endif // !FEATURE_LEGACYNETCF
        {
            _newItems = newItems;
        }
        _newStartingIndex = newStartingIndex;
    }

    void InitializeRemove(NotifyCollectionChangedAction action, List<Variant> oldItems, int oldStartingIndex)
    {
        _action = action;
        _oldItems = oldItems;
        _oldStartingIndex= oldStartingIndex;
    }

    void InitializeMoveOrReplace(NotifyCollectionChangedAction action, List<Variant> newItems, List<Variant> oldItems, int startingIndex, int oldStartingIndex)
    {
        InitializeAdd(action, newItems, startingIndex);
        InitializeRemove(action, oldItems, oldStartingIndex);
    }

    //------------------------------------------------------
    //
    //  Properties
    //
    //------------------------------------------------------

    /// <summary>
    /// The action that caused the event.
    /// </summary>
    NotifyCollectionChangedAction Action()
    {
        return _action;
    }

    /// <summary>
    /// The items affected by the change.
    /// </summary>
    List<Variant> NewItems()
    {
        return _newItems;
    }

    /// <summary>
    /// The old items affected by the change (for Replace events).
    /// </summary>
    List<Variant> OldItems()
    {
        return _oldItems;
    }

    /// <summary>
    /// The index where the change occurred.
    /// </summary>
    int NewStartingIndex()
    {
        return _newStartingIndex;
    }

    /// <summary>
    /// The old index where the change occurred (for Move events).
    /// </summary>
    int OldStartingIndex()
    {
        return _oldStartingIndex;
    }

    //------------------------------------------------------
    //
    //  Fields
    //
    //------------------------------------------------------

    NotifyCollectionChangedAction _action;
    List<Variant> _newItems, _oldItems;
    int _newStartingIndex = -1;
    int _oldStartingIndex = -1;
};

INKCANVAS_END_NAMESPACE

#endif // NOTIFYCOLLECTIONCHANGEDEVENTARGS_H
