#ifndef EVENTS_H
#define EVENTS_H

#include "strokecollection.h"
#include "styluspointcollection.h"
#include "extendedproperty.h"

#include <QUuid>
#include <QVariant>
#include <QSharedPointer>

class StrokeCollectionChangedEventArgs
{
private:
    QSharedPointer<StrokeCollection> _added;
    QSharedPointer<StrokeCollection> _removed;
    int _index = -1;

public:
    /// <summary>Constructor</summary>
    StrokeCollectionChangedEventArgs(QSharedPointer<StrokeCollection> added, QSharedPointer<StrokeCollection> removed, int index) :
        StrokeCollectionChangedEventArgs(added, removed)
    {
        _index = index;
    }

    /// <summary>Constructor</summary>
    StrokeCollectionChangedEventArgs(QSharedPointer<StrokeCollection> added, QSharedPointer<StrokeCollection> removed)
        : _added(added)
        , _removed(removed)
    {
        //if ( added == null && removed == null )
        //{
        //    throw std::exception(SR.Get(SRID.CannotBothBeNull, "added", "removed"));
        //}
        //_added = ( added == null ) ? null : new StrokeCollection.ReadOnlyStrokeCollection(added);
        //_removed = ( removed == null ) ? null : new StrokeCollection.ReadOnlyStrokeCollection(removed);
    }

    /// <summary>Set of strokes that where added, result may be an empty collection</summary>
    QSharedPointer<StrokeCollection> Added()
    {
        //if ( _added == null )
        //{
        //    _added = new StrokeCollection.ReadOnlyStrokeCollection(new StrokeCollection());
        //}
        return _added;
    }

    /// <summary>Set of strokes that where removed, result may be an empty collection</summary>
    QSharedPointer<StrokeCollection> Removed()
    {
        //if ( _removed == null )
        //{
        //    _removed = new StrokeCollection.ReadOnlyStrokeCollection(new StrokeCollection());
        //}
        return _removed;
    }

    /// <summary>
    /// The zero based starting index that was affected
    /// </summary>
    int Index()
    {
        return _index;
    }
};


/// <summary>
/// Event arg used a change to the drawing attributes associated with one or more strokes has occurred.
/// </summary>
class PropertyDataChangedEventArgs
{
private:
    QUuid _propertyGuid;
    QVariant _newValue;
    QVariant _previousValue;

public:
    /// <summary>Constructor</summary>
    PropertyDataChangedEventArgs(QUuid propertyGuid,
                                        QVariant newValue,
                                        QVariant previousValue)
    {
        if ( newValue.isNull() && previousValue.isNull() )
        {
            throw std::exception("newValue, previousValue");
        }

        _propertyGuid = propertyGuid;
        _newValue = newValue;
        _previousValue = previousValue;
    }

    /// <summary>
    /// Gets the property QUuid that represents the DrawingAttribute that changed
    /// </summary>
    QUuid PropertyGuid()
    {
        return _propertyGuid;
    }

    /// <summary>
    /// Gets the new value of the DrawingAttribute
    /// </summary>
    QVariant NewValue()
    {
        return _newValue;
    }

    /// <summary>
    /// Gets the previous value of the DrawingAttribute
    /// </summary>
    QVariant PreviousValue()
    {
        return _previousValue;
    }
};



/// <summary>
/// Event Arg used when the Custom attributes associated with an QVariant have changed.
/// </summary>
class ExtendedPropertiesChangedEventArgs
{
private:
    ExtendedProperty _oldProperty;
    ExtendedProperty _newProperty;

public:
    /// <summary>Constructor</summary>
    ExtendedPropertiesChangedEventArgs(ExtendedProperty oldProperty,
                                                ExtendedProperty newProperty)
        : _oldProperty(oldProperty)
        , _newProperty(newProperty)
    {
        //if ( oldProperty == null && newProperty == null )
        //{
        //    throw std::exception("oldProperty");
        //}
        //_oldProperty = oldProperty;
        //_newProperty = newProperty;
    }

    /// <summary>
    /// The value of the previous property.  If the Changed event was caused
    /// by an ExtendedProperty being added, this value is null
    /// </summary>
    ExtendedProperty& OldProperty()
    {
        return _oldProperty;
    }

    /// <summary>
    /// The value of the new property.  If the Changed event was caused by
    /// an ExtendedProperty being removed, this value is null
    /// </summary>
    ExtendedProperty& NewProperty()
    {
        return _newProperty;
    }
};

class DrawingAttributes;

/// <summary>
///    DrawingAttributesReplacedEventArgs
/// </summary>
class DrawingAttributesReplacedEventArgs
{
public:
    /// <summary>
    /// DrawingAttributesReplacedEventArgs
    /// </summary>
    /// <remarks>
    /// This must be so InkCanvas can instance it
    /// </remarks>
    DrawingAttributesReplacedEventArgs(QSharedPointer<DrawingAttributes> newDrawingAttributes, QSharedPointer<DrawingAttributes> previousDrawingAttributes)
        : _newDrawingAttributes(newDrawingAttributes)
        , _previousDrawingAttributes(previousDrawingAttributes)
    {
        //if ( newDrawingAttributes == null )
        //{
        //    throw std::exception("newDrawingAttributes");
        //}
        //if ( previousDrawingAttributes == null )
        //{
        //    throw std::exception("previousDrawingAttributes");
        //}
        //_newDrawingAttributes = newDrawingAttributes;
        //_previousDrawingAttributes = previousDrawingAttributes;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<DrawingAttributes> NewDrawingAttributes()
    {
        return _newDrawingAttributes;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<DrawingAttributes> PreviousDrawingAttributes()
    {
        return _previousDrawingAttributes;
    }

private:
    QSharedPointer<DrawingAttributes> _newDrawingAttributes;
    QSharedPointer<DrawingAttributes> _previousDrawingAttributes;
};


/// <summary>
///    StylusPointsReplacedEventArgs
/// </summary>
class StylusPointsReplacedEventArgs
{
public:
    /// <summary>
    /// StylusPointsReplacedEventArgs
    /// </summary>
    /// <remarks>
    /// This must be so InkCanvas can instance it
    /// </remarks>
    StylusPointsReplacedEventArgs(QSharedPointer<StylusPointCollection> newStylusPoints, QSharedPointer<StylusPointCollection> previousStylusPoints)
        : _newStylusPoints(newStylusPoints)
        , _previousStylusPoints(previousStylusPoints)
    {
        //if ( newStylusPoints == null )
        //{
        //    throw std::exception("newStylusPoints");
        //}
        //if ( previousStylusPoints == null )
        //{
        //    throw std::exception("previousStylusPoints");
        //}
        //_newStylusPoints = newStylusPoints;
        //_previousStylusPoints = previousStylusPoints;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<StylusPointCollection> NewStylusPoints()
    {
        return _newStylusPoints;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<StylusPointCollection> PreviousStylusPoints()
    {
        return _previousStylusPoints;
    }

private:
    QSharedPointer<StylusPointCollection> _newStylusPoints;
    QSharedPointer<StylusPointCollection> _previousStylusPoints;
};

class PropertyChangedEventArgs
{
private:
    QString propertyName;

public:
    /// <devdoc>
    /// <para>Initializes a new instance of the <see cref='System.ComponentModel.PropertyChangedEventArgs'/>
    /// class.</para>
    /// </devdoc>
    PropertyChangedEventArgs(QString propertyName)
        : propertyName(propertyName)
    {
        //propertyName = propertyName;
    }

    /// <devdoc>
    ///    <para>Indicates the name of the property that changed.</para>
    /// </devdoc>
    virtual QString PropertyName() {
        return propertyName;
    }
};


class CancelEventArgs
{

private:
    /// <devdoc>
    ///     Indicates, on return, whether or not the operation should be cancelled
    ///     or not.  'true' means cancel it, 'false' means don't.
    /// </devdoc>
    bool cancel;

public:
    /// <devdoc>
    ///    <para>
    ///       Initializes a new instance of the <see cref='System.ComponentModel.CancelEventArgs'/> class with
    ///       cancel set to <see langword='false'/>.
    ///    </para>
    /// </devdoc>
    CancelEventArgs() : CancelEventArgs(false) {
    }

    /// <devdoc>
    ///    <para>
    ///       Initializes a new instance of the <see cref='System.ComponentModel.CancelEventArgs'/> class with
    ///       cancel set to the given value.
    ///    </para>
    /// </devdoc>
    CancelEventArgs(bool cancel)
        : cancel(cancel)
    {
    }

    /// <devdoc>
    ///    <para>
    ///       Gets or sets a value
    ///       indicating whether the operation should be cancelled.
    ///    </para>
    /// </devdoc>
    bool Cancel() {
        return cancel;
    }
    void SetCancel(bool value) {
        cancel = value;
    }
};


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

class NotifyCollectionChangedEventArgs
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
            throw std::exception("action");

        InitializeAdd(action, QVariantList(), -1);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item change.
    /// </summary>
    /// <param name="action">The action that caused the event; can only be Reset, Add or Remove action.</param>
    /// <param name="changedItem">The item affected by the change.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariant changedItem)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::exception("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (!changedItem.isNull())
                throw std::exception("action");

            InitializeAdd(action, QVariantList(), -1);
        }
        else
        {
            InitializeAddOrRemove(action, QVariantList({changedItem}), -1);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item change.
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItem">The item affected by the change.</param>
    /// <param name="index">The index where the change occurred.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariant changedItem, int index)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::exception("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (!changedItem.isNull())
                throw std::exception("action");
            if (index != -1)
                throw std::exception("action");

            InitializeAdd(action, QVariantList(), -1);
        }
        else
        {
            InitializeAddOrRemove(action, QVariantList({changedItem}), index);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item change.
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItems">The items affected by the change.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariantList changedItems)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::exception("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (changedItems.isEmpty())
                throw std::exception("action");

            InitializeAdd(action, QVariantList(), -1);
        }
        else
        {
            if (changedItems.isEmpty())
                throw std::exception("changedItems");

            InitializeAddOrRemove(action, changedItems, -1);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item change (or a reset).
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItems">The items affected by the change.</param>
    /// <param name="startingIndex">The index where the change occurred.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariantList changedItems, int startingIndex)
    {
        if ((action != NotifyCollectionChangedAction::Add) && (action != NotifyCollectionChangedAction::Remove)
                && (action != NotifyCollectionChangedAction::Reset))
            throw std::exception("action");

        if (action == NotifyCollectionChangedAction::Reset)
        {
            if (!changedItems.isEmpty())
                throw std::exception("action");
            if (startingIndex != -1)
                throw std::exception("action");

            InitializeAdd(action, QVariantList(), -1);
        }
        else
        {
            if (changedItems.isEmpty())
                throw std::exception("changedItems");
            if (startingIndex < -1)
                throw std::exception("startingIndex");

            InitializeAddOrRemove(action, changedItems, startingIndex);
        }
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItem">The new item replacing the original item.</param>
    /// <param name="oldItem">The original item that is replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariant newItem, QVariant oldItem)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::exception("action");

        InitializeMoveOrReplace(action, QVariantList({newItem}), QVariantList({oldItem}), -1, -1);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItem">The new item replacing the original item.</param>
    /// <param name="oldItem">The original item that is replaced.</param>
    /// <param name="index">The index of the item being replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariant newItem, QVariant oldItem, int index)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::exception("action");

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
        InitializeMoveOrReplace(action, QVariantList({newItem}), QVariantList({oldItem}), index, oldStartingIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItems">The new items replacing the original items.</param>
    /// <param name="oldItems">The original items that are replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariantList newItems, QVariantList oldItems)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::exception("action");
        if (newItems.isEmpty())
            throw std::exception("newItems");
        if (oldItems.isEmpty())
            throw std::exception("oldItems");

        InitializeMoveOrReplace(action, newItems, oldItems, -1, -1);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item Replace event.
    /// </summary>
    /// <param name="action">Can only be a Replace action.</param>
    /// <param name="newItems">The new items replacing the original items.</param>
    /// <param name="oldItems">The original items that are replaced.</param>
    /// <param name="startingIndex">The starting index of the items being replaced.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariantList newItems, QVariantList oldItems, int startingIndex)
    {
        if (action != NotifyCollectionChangedAction::Replace)
            throw std::exception("action");
        if (newItems.isEmpty())
            throw std::exception("newItems");
        if (oldItems.isEmpty())
            throw std::exception("oldItems");

        InitializeMoveOrReplace(action, newItems, oldItems, startingIndex, startingIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a one-item Move event.
    /// </summary>
    /// <param name="action">Can only be a Move action.</param>
    /// <param name="changedItem">The item affected by the change.</param>
    /// <param name="index">The new index for the changed item.</param>
    /// <param name="oldIndex">The old index for the changed item.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariant changedItem, int index, int oldIndex)
    {
        if (action != NotifyCollectionChangedAction::Move)
            throw std::exception("action");
        if (index < 0)
            throw std::exception("index");

        QVariantList changedItems= {changedItem};
        InitializeMoveOrReplace(action, changedItems, changedItems, index, oldIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs that describes a multi-item Move event.
    /// </summary>
    /// <param name="action">The action that caused the event.</param>
    /// <param name="changedItems">The items affected by the change.</param>
    /// <param name="index">The new index for the changed items.</param>
    /// <param name="oldIndex">The old index for the changed items.</param>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariantList changedItems, int index, int oldIndex)
    {
        if (action != NotifyCollectionChangedAction::Move)
            throw std::exception("action");
        if (index < 0)
            throw std::exception("index");

        InitializeMoveOrReplace(action, changedItems, changedItems, index, oldIndex);
    }

    /// <summary>
    /// Construct a NotifyCollectionChangedEventArgs with given fields (no validation). Used by WinRT marshaling.
    /// </summary>
    NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction action, QVariantList newItems, QVariantList oldItems, int newIndex, int oldIndex)
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
    void InitializeAddOrRemove(NotifyCollectionChangedAction action, QVariantList changedItems, int startingIndex)
    {
        if (action == NotifyCollectionChangedAction::Add)
            InitializeAdd(action, changedItems, startingIndex);
        else if (action == NotifyCollectionChangedAction::Remove)
            InitializeRemove(action, changedItems, startingIndex);
        else
            throw std::exception(QString("Unsupported action: %s").arg(action).toUtf8());
    }

    void InitializeAdd(NotifyCollectionChangedAction action, QVariantList newItems, int newStartingIndex)
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

    void InitializeRemove(NotifyCollectionChangedAction action, QVariantList oldItems, int oldStartingIndex)
    {
        _action = action;
        _oldItems = oldItems;
        _oldStartingIndex= oldStartingIndex;
    }

    void InitializeMoveOrReplace(NotifyCollectionChangedAction action, QVariantList newItems, QVariantList oldItems, int startingIndex, int oldStartingIndex)
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
    QVariantList NewItems()
    {
        return _newItems;
    }

    /// <summary>
    /// The old items affected by the change (for Replace events).
    /// </summary>
    QVariantList OldItems()
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
    QVariantList _newItems, _oldItems;
    int _newStartingIndex = -1;
    int _oldStartingIndex = -1;
};

#endif // EVENTS_H
