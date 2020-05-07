#ifndef DEPENDENCYPROPERTYCHANGEDEVENTARGS_H
#define DEPENDENCYPROPERTYCHANGEDEVENTARGS_H

#include "effectivevalueentry.h"
#include "dependencyobject.h"
#include "eventargs.h"

#include <QVariant>

class DependencyProperty;
class PropertyMetadata;

//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.

enum class OperationType
{
    Unknown                     = 0,
    AddChild                    = 1,
    RemoveChild                 = 2,
    Inherit                     = 3,
    ChangeMutableDefaultValue   = 4,
};

// namespace System.Windows

class DependencyPropertyChangedEventArgs : public EventArgs
{
public:
    enum PrivateFlag
    {
        IsAValueChange        = 0x01,
        IsASubPropertyChange  = 0x02,
    };

    Q_DECLARE_FLAGS(PrivateFlags, PrivateFlag)


public:
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
    DependencyPropertyChangedEventArgs(DependencyProperty const * property, QVariant oldValue, QVariant newValue);

    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    DependencyPropertyChangedEventArgs(DependencyProperty const * property, PropertyMetadata* metadata, QVariant oldValue, QVariant newValue);

    DependencyPropertyChangedEventArgs(DependencyProperty const * property, PropertyMetadata* metadata, QVariant value);

    DependencyPropertyChangedEventArgs(
        DependencyProperty const *  property,
        PropertyMetadata*    metadata,
        bool                isAValueChange,
        EffectiveValueEntry oldEntry,
        EffectiveValueEntry newEntry,
        OperationType       operationType);


    /// <summary>
    ///     The property whose value changed.
    /// </summary>
    DependencyProperty const * Property();

    /// <summary>
    ///     Whether or not this change indicates a change to the property value
    /// </summary>
    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    bool GetIsAValueChange();
    void SetIsAValueChange(bool value);

    /// <summary>
    ///     Whether or not this change indicates a change to the subproperty
    /// </summary>
    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    bool GetIsASubPropertyChange();
    void SetIsASubPropertyChange(bool value);

    /// <summary>
    ///     Metadata for the property
    /// </summary>
    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    PropertyMetadata* Metadata();

    /// <summary>
    ///     Says what operation caused this property change
    /// </summary>
    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    OperationType GetOperationType();


    /// <summary>
    ///     The old value of the property.
    /// </summary>
    QVariant OldValue();

    /// <summary>
    ///     The entry for the old value (contains value and all modifier info)
    /// </summary>
    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    EffectiveValueEntry OldEntry();

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
    QVariant NewValue();

    template<typename T>
    T NewValueT()
    {
        return NewValue().value<T>();
    }

    /// <summary>
    ///     The entry for the new value (contains value and all modifier info)
    /// </summary>
    //[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
    EffectiveValueEntry NewEntry();

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
    bool Equals(DependencyPropertyChangedEventArgs const & args) const;

    /// <summary>
    /// </summary>
    friend bool operator ==(DependencyPropertyChangedEventArgs const & left, DependencyPropertyChangedEventArgs const & right)
    {
        return left.Equals(right);
    }

    /// <summary>
    /// </summary>
    friend bool operator !=(DependencyPropertyChangedEventArgs const & left, DependencyPropertyChangedEventArgs const & right)
    {
        return !left.Equals(right);
    }


private:
    void WritePrivateFlag(PrivateFlags bit, bool value)
    {
        if (value)
        {
            _flags |= bit;
        }
        else
        {
            _flags &= ~bit;
        }
    }

    bool ReadPrivateFlag(PrivateFlags bit)
    {
        return (_flags & bit) != 0;
    }



private:
    DependencyProperty const *  _property;
    PropertyMetadata*    _metadata;

    PrivateFlags        _flags;

    EffectiveValueEntry _oldEntry;
    EffectiveValueEntry _newEntry;

    OperationType       _operationType;

};

#endif // DEPENDENCYPROPERTYCHANGEDEVENTARGS_H
