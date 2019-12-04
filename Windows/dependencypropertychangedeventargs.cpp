#include "Windows/dependencypropertychangedeventargs.h"

/// <summary>
///     Initializes a new instance of the DependencyPropertyChangedEventArgs class.
/// </summary>
/// <param name="property">
///     The property whose value changed.
/// </param>
/// <param name="oldValue">
///     The value of the property before the change.
/// </param>
/// <param name="newValue">
///     The value of the property after the change.
/// </param>
DependencyPropertyChangedEventArgs::DependencyPropertyChangedEventArgs(DependencyProperty const * property, QVariant oldValue, QVariant newValue)
    : _oldEntry(property)
    , _newEntry(property)
{
    _property = property;
    _metadata = nullptr;
    _oldEntry.SetValue(oldValue);
    _newEntry.SetValue(newValue);

    _flags = nullptr;
    _operationType = OperationType::Unknown;
    SetIsAValueChange(true);
}

//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
DependencyPropertyChangedEventArgs::DependencyPropertyChangedEventArgs(DependencyProperty const * property, PropertyMetadata* metadata, QVariant oldValue, QVariant newValue)
    : _oldEntry(property)
    , _newEntry(property)
{
    _property = property;
    _metadata = metadata;
    _oldEntry.SetValue(oldValue);
    _newEntry.SetValue(newValue);

    _flags = nullptr;
    _operationType = OperationType::Unknown;
    SetIsAValueChange(true);
}

DependencyPropertyChangedEventArgs::DependencyPropertyChangedEventArgs(DependencyProperty const * property, PropertyMetadata* metadata, QVariant value)
    : _oldEntry(property)
    , _newEntry(property)
{
    _property = property;
    _metadata = metadata;
    _oldEntry.SetValue(value);
    _newEntry = _oldEntry;

    _flags = nullptr;
    _operationType = OperationType::Unknown;
    SetIsASubPropertyChange(true);
}

DependencyPropertyChangedEventArgs::DependencyPropertyChangedEventArgs(
    DependencyProperty const *  property,
    PropertyMetadata*    metadata,
    bool                isAValueChange,
    EffectiveValueEntry oldEntry,
    EffectiveValueEntry newEntry,
    OperationType       operationType)
    : _oldEntry(oldEntry)
    , _newEntry(newEntry)
{
    _property             = property;
    _metadata             = metadata;
    //_oldEntry             = oldEntry;
    //_newEntry             = newEntry;

    _flags = nullptr;
    _operationType        = operationType;
    SetIsAValueChange(isAValueChange);

    // This is when a mutable default is promoted to a local value. On this operation mutable default
    // value acquires a freezable context. However this value promotion operation is triggered
    // whenever there has been a sub property change to the mutable default. Eg. Adding a TextEffect
    // to a TextEffectCollection instance which is the mutable default. Since we missed the sub property
    // change due to this add, we flip the IsASubPropertyChange bit on the following change caused by
    // the value promotion to coalesce these operations.
    SetIsASubPropertyChange(operationType == OperationType::ChangeMutableDefaultValue);
}


/// <summary>
///     The property whose value changed.
/// </summary>
DependencyProperty const * DependencyPropertyChangedEventArgs::Property()
{
    return _property;
}

/// <summary>
///     Whether or not this change indicates a change to the property value
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
bool DependencyPropertyChangedEventArgs::GetIsAValueChange()
{
    return ReadPrivateFlag(PrivateFlag::IsAValueChange);
}
void DependencyPropertyChangedEventArgs::SetIsAValueChange(bool value)
{
    WritePrivateFlag(PrivateFlag::IsAValueChange, value);
}

/// <summary>
///     Whether or not this change indicates a change to the subproperty
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
bool DependencyPropertyChangedEventArgs::GetIsASubPropertyChange()
{
    return ReadPrivateFlag(PrivateFlag::IsASubPropertyChange);
}
void DependencyPropertyChangedEventArgs::SetIsASubPropertyChange(bool value)
{
    WritePrivateFlag(PrivateFlag::IsASubPropertyChange, value);
}

/// <summary>
///     Metadata for the property
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
PropertyMetadata* DependencyPropertyChangedEventArgs::Metadata()
{
    return _metadata;
}

/// <summary>
///     Says what operation caused this property change
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
OperationType DependencyPropertyChangedEventArgs::GetOperationType()
{
    return _operationType;
}


/// <summary>
///     The old value of the property.
/// </summary>
QVariant DependencyPropertyChangedEventArgs::OldValue()
{
    EffectiveValueEntry oldEntry = OldEntry().GetFlattenedEntry(RequestFlags::FullyResolved);
    if (oldEntry.IsDeferredReference())
    {
        // The value for this property was meant to come from a dictionary
        // and the creation of that value had been deferred until this
        // time for better performance. Now is the time to actually instantiate
        // this value by querying it from the dictionary. Once we have the
        // value we can actually replace the deferred reference marker
        // with the actual value.
        //oldEntry.Value() = ((DeferredReference) oldEntry.Value()).GetValue(oldEntry.GetBaseValueSourceInternal()Internal);
    }

    return oldEntry.Value();
}

/// <summary>
///     The entry for the old value (contains value and all modifier info)
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
EffectiveValueEntry DependencyPropertyChangedEventArgs::OldEntry()
{
    return _oldEntry;
}

/// <summary>
///     The source of the old value
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
//BaseValueSourceOldValueSource
//{
//    get { return _oldEntry.GetBaseValueSourceInternal()Internal; }
//}

/// <summary>
///     Says if the old value was a modified value (coerced, animated, expression)
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
//bool IsOldValueModified
//{
//     get { return _oldEntry.HasModifiers(); }
//}

/// <summary>
///     Says if the old value was a deferred value
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
//bool IsOldValueDeferred
//{
//    get { return _oldEntry.IsDeferredReference(); }
//}

/// <summary>
///     The new value of the property.
/// </summary>
QVariant DependencyPropertyChangedEventArgs::NewValue()
{
    EffectiveValueEntry newEntry = NewEntry().GetFlattenedEntry(RequestFlags::FullyResolved);
    if (newEntry.IsDeferredReference())
    {
        // The value for this property was meant to come from a dictionary
        // and the creation of that value had been deferred until this
        // time for better performance. Now is the time to actually instantiate
        // this value by querying it from the dictionary. Once we have the
        // value we can actually replace the deferred reference marker
        // with the actual value.
        //newEntry.Value() = ((DeferredReference) newEntry.Value()).GetValue(newEntry.GetBaseValueSourceInternal()Internal);
    }

    return newEntry.Value();
}

/// <summary>
///     The entry for the new value (contains value and all modifier info)
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
EffectiveValueEntry DependencyPropertyChangedEventArgs::NewEntry()
{
    return _newEntry;
}

/// <summary>
///     The source of the new value
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
//BaseValueSourceNewValueSource
//{
//    get { return _newEntry.GetBaseValueSourceInternal()Internal; }
//}

/// <summary>
///     Says if the new value was a modified value (coerced, animated, expression)
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
//bool IsNewValueModified
//{
//    get { return _newEntry.HasModifiers(); }
//}

/// <summary>
///     Says if the new value was a deferred value
/// </summary>
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
//bool IsNewValueDeferred
//{
//    get { return _newEntry.IsDeferredReference(); }
//}


/// <summary>
/// </summary>
//int GetHashCode()
//{
//    return base.GetHashCode();
//}

/// <summary>
/// </summary>
//bool Equals(QVariant obj)
//{
//    return Equals((DependencyPropertyChangedEventArgs)obj);
//}

/// <summary>
/// </summary>
bool DependencyPropertyChangedEventArgs::Equals(DependencyPropertyChangedEventArgs const & args) const
{
    return (_property == args._property &&
            _metadata == args._metadata &&
            _oldEntry.Value() == args._oldEntry.Value() &&
            _newEntry.Value() == args._newEntry.Value() &&
            _flags == args._flags &&
            _oldEntry.GetBaseValueSourceInternal()== args._oldEntry.GetBaseValueSourceInternal()&&
            _newEntry.GetBaseValueSourceInternal()== args._newEntry.GetBaseValueSourceInternal()&&
            _oldEntry.HasModifiers() == args._oldEntry.HasModifiers() &&
            _newEntry.HasModifiers() == args._newEntry.HasModifiers() &&
            _oldEntry.IsDeferredReference() == args._oldEntry.IsDeferredReference() &&
            _newEntry.IsDeferredReference() == args._newEntry.IsDeferredReference() &&
            _operationType == args._operationType);
}

