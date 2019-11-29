#include "effectivevalueentry.h"

EffectiveValueEntry EffectiveValueEntry::CreateDefaultValueEntry(DependencyProperty* dp, QVariant value)
{
    EffectiveValueEntry entry(dp, BaseValueSourceInternal::Default);
    entry.SetValue(value);
    return entry;

}

EffectiveValueEntry::EffectiveValueEntry(DependencyProperty* dp)
{
    _propertyIndex = static_cast<short>(dp->GlobalIndex());
    //_value = null;
    _source = (FullValueSource) BaseValueSourceInternal::Unknown;
}

EffectiveValueEntry::EffectiveValueEntry(DependencyProperty* dp, BaseValueSourceInternal valueSource)
{
    _propertyIndex = static_cast<short>(dp->GlobalIndex());
    _value = DependencyProperty::UnsetValue;
    _source = (FullValueSource) valueSource;
}

EffectiveValueEntry::EffectiveValueEntry(DependencyProperty* dp, FullValueSource fullValueSource)
{
    _propertyIndex = static_cast<short>(dp->GlobalIndex());
    _value = DependencyProperty::UnsetValue;
    _source = fullValueSource;
}

void EffectiveValueEntry::SetExpressionValue(QVariant value, QVariant baseValue)
{
    //Debug.Asert(value != UnsetValue);

    ModifiedValue modifiedValue = EnsureModifiedValue();
    //modifiedValue.ExpressionValue = value;
    //IsExpression = true;
    //IsDeferredReference = value is DeferredReference;

    //Debug.Asert(Object.Equals(modifiedValue.BaseValue, baseValue));
    //Debug.Asert(!(baseValue is DeferredReference));
    //Debug.DependencyProperty::Assert(IsDeferredReference == (value is DeferredReference));
}

void EffectiveValueEntry::SetAnimatedValue(QVariant value, QVariant baseValue)
{
    //Debug.Asert((value != DependencyProperty::UnsetValue) &&
    //             !(value is DeferredReference));

    ModifiedValue modifiedValue = EnsureModifiedValue();
    //modifiedValue.AnimatedValue = value;
    //IsAnimated = true;

    // Animated values should never be deferred
    //IsDeferredReference = false;

    //Debug.Asert(!(modifiedValue.AnimatedValue is DeferredReference));
    //Debug.Asert(Object.Equals(modifiedValue.BaseValue, baseValue) ||
    //             Object.Equals(modifiedValue.ExpressionValue, baseValue));
    //Debug.Asert(!(baseValue is DeferredReference) &&
    //             ! (modifiedValue.BaseValue is DeferredReference) &&
    //             ! (modifiedValue.ExpressionValue is DeferredReference));
}
/*
void EffectiveValueEntry::SetCoercedValue(QVariant value, QVariant baseValue, bool skipBaseValueChecks, bool coerceWithCurrentValue)
{
    //Debug.Asert(value != DependencyProperty::UnsetValue &&
    //             !((value is DeferredReference) && !coerceWithCurrentValue));

    // if this is already a CoercedWithControlValue entry, we are applying a
    // second coercion (e.g. from the CoerceValueCallback).  The baseValue
    // passed in is the result of the control-value coercion, but for the
    // purposes of this method we should use the original base value instead.
    if (IsCoercedWithCurrentValue)
    {
        baseValue = ModifiedValue.BaseValue;
    }

    ModifiedValue modifiedValue = EnsureModifiedValue(coerceWithCurrentValue);
    //modifiedValue.CoercedValue = value;
    //IsCoerced = true;
    //IsCoercedWithCurrentValue = coerceWithCurrentValue;

    // The only CoercedValues that can be deferred are Control values.
    //if (coerceWithCurrentValue)
    //{
    //    IsDeferredReference = (value is DeferredReference);
    //}
    //else
    //{
    //    IsDeferredReference = false;
    //}


    //Debug.Asert(skipBaseValueChecks ||
    //             Object.Equals(modifiedValue.BaseValue, baseValue) ||
    //             Object.Equals(modifiedValue.ExpressionValue, baseValue) ||
    //             Object.Equals(modifiedValue.AnimatedValue, baseValue));
    //Debug.Asert(!(baseValue is DeferredReference) &&
    //             ! (modifiedValue.BaseValue is DeferredReference) &&
    //             ! (modifiedValue.ExpressionValue is DeferredReference) &&
    //             ! (modifiedValue.AnimatedValue is DeferredReference));
}

void EffectiveValueEntry::ResetAnimatedValue()
{
    if (IsAnimated)
    {
        ModifiedValue modifiedValue = ModifiedValue;
        //modifiedValue.AnimatedValue = null;
        //IsAnimated = false;

        if (!HasModifiers())
        {
            //Value = modifiedValue.BaseValue;
        }
        else
        {
            // The setter takes care of the IsDeferred flag no need to compute it twice.
            ComputeIsDeferred();
        }
    }
}

void EffectiveValueEntry::ResetCoercedValue()
{
    if (IsCoerced)
    {
        ModifiedValue modifiedValue = ModifiedValue;
        //modifiedValue.CoercedValue = null;
        //IsCoerced = false;

        if (!HasModifiers())
        {
            //Value = modifiedValue.BaseValue;
        }
        else
        {
            ComputeIsDeferred();
        }
    }
}

// remove all modifiers, retain value source, and set value to supplied value
void EffectiveValueEntry::ResetValue(QVariant value, bool hasExpressionMarker)
{
    //_source &= FullValueSource::ValueSourceMask;
    _value = value;
    if (hasExpressionMarker)
    {
        //HasExpressionMarker = true;
    }
    else
    {
        ComputeIsDeferred();
    }
    //Debug.Asert(hasExpressionMarker == (value == DependencyObject.ExpressionInAlternativeStore), "hasExpressionMarker flag must match value");
}

// add an expression marker back as the base value for an expression value
void EffectiveValueEntry::RestoreExpressionMarker()
{
    if (HasModifiers())
    {
        ModifiedValue entry = ModifiedValue;
        entry.ExpressionValue = entry.BaseValue;
        entry.BaseValue = DependencyObject.ExpressionInAlternativeStore;
        _source |= FullValueSource::IsExpression() | FullValueSource::HasExpressionMarker();

        //recompute the isDeferredReference flag as it may have changed
        ComputeIsDeferred();
    }
    else
    {
        QVariant value = Value;
        Value = DependencyObject.ExpressionInAlternativeStore;
        SetExpressionValue(value, DependencyObject.ExpressionInAlternativeStore);
        _source |= FullValueSource::HasExpressionMarker;
    }

}

// Computes and set the IsDeferred hint flag.
// This take into account all flags and should only be used sparingly.
void EffectiveValueEntry::ComputeIsDeferred()
{
    bool isDeferredReference = false;
    if (!HasModifiers)
    {
        isDeferredReference = Value is DeferredReference;
    }
    else if (ModifiedValue != null)
    {
        if (IsCoercedWithCurrentValue)
        {
            isDeferredReference = ModifiedValue.CoercedValue is DeferredReference;
        }
        else if (IsExpression)
        {
            isDeferredReference = ModifiedValue.ExpressionValue is DeferredReference;
        }

        // For animated values isDeferred will always be false.
    }

    IsDeferredReference = isDeferredReference;
}




/// <summary>
///     If HasModifiers is true then it holds the value
///     else it holds the modified value instance
/// </summary>
QVariant EffectiveValueEntry::Value() const
{
    return _value;
}
void EffectiveValueEntry::SetValue(QVariant value)
{
    _value = value;
    //IsDeferredReference = value is DeferredReference;
    //Debug.Asert(value is DeferredReference == IsDeferredReference);
}

BaseValueSourceInternal EffectiveValueEntry::GetBaseValueSourceInternal() const
{
    return (BaseValueSourceInternal)(_source & FullValueSource::ValueSourceMask);
}
void EffectiveValueEntry::SetBaseValueSourceInternal(BaseValueSourceInternal value)
{
    _source = (_source & ~FullValueSource::ValueSourceMask) | (FullValueSource)value;
}

bool EffectiveValueEntry::IsDeferredReference() const
{
    // When this flag is true we treat it as a hint rather than a guarantee and update
    // it if is out of sync. When the flag is false, however we expect it to guarantee
    // that the value isn't a DeferredReference.

    bool isDeferredReference = ReadPrivateFlag(FullValueSource::IsPotentiallyADeferredReference);
    if (isDeferredReference)
    {
        // Check if the value is really a deferred reference
        ComputeIsDeferred();
        isDeferredReference = ReadPrivateFlag(FullValueSource::IsPotentiallyADeferredReference);
    }

    return isDeferredReference;
}

//set { WritePrivateFlag(FullValueSource::IsPotentiallyADeferredReference, value); }

bool EffectiveValueEntry::IsExpression()
{
    return ReadPrivateFlag(FullValueSource::IsExpression);
}
void EffectiveValueEntry::SetIsExpression()
{
    WritePrivateFlag(FullValueSource::IsExpression, value);
}

//bool IsAnimated()
//{
//    get { return ReadPrivateFlag(FullValueSource::IsAnimated); }
//    set { WritePrivateFlag(FullValueSource::IsAnimated, value); }
//}

//bool IsCoerced()
//{
//    get { return ReadPrivateFlag(FullValueSource::IsCoerced); }
//    set { WritePrivateFlag(FullValueSource::IsCoerced, value); }
//}

bool EffectiveValueEntry::HasModifiers() const
{
    return (_source & FullValueSource::ModifiersMask) != 0;
}



//bool HasExpressionMarker
//{
//    get { return ReadPrivateFlag(FullValueSource::HasExpressionMarker); }
//    set { WritePrivateFlag(FullValueSource::HasExpressionMarker, value); }
//}

//bool IsCoercedWithCurrentValue
//{
//    get { return ReadPrivateFlag(FullValueSource::IsCoercedWithCurrentValue); }
//    set { WritePrivateFlag(FullValueSource::IsCoercedWithCurrentValue, value); }
//}

EffectiveValueEntry EffectiveValueEntry::GetFlattenedEntry(RequestFlags requests)
{
    if ((_source & (FullValueSource::ModifiersMask | FullValueSource::HasExpressionMarker)) == 0)
    {
        // If the property does not have any modifiers
        // then just return the base value.
        return *this;
    }

    if (!HasModifiers)
    {
        //Debug.Asert(HasExpressionMarker);

        // This is the case when some one stuck an expression into
        // an alternate store such as a style or a template but the
        // new value for the expression has not been evaluated yet.
        // In the intermediate we need to return the default value
        // for the property. This problem was manifested in DRTDocumentViewer.
        EffectiveValueEntry unsetEntry = new EffectiveValueEntry();
        unsetEntry.BaseValueSource= BaseValueSourceInternal;
        unsetEntry.PropertyIndex = PropertyIndex;
        return unsetEntry;
    }

    // else entry has modifiers
    EffectiveValueEntry entry = new EffectiveValueEntry();
    entry.BaseValueSource= BaseValueSourceInternal;
    entry.PropertyIndex = PropertyIndex;
    entry.IsDeferredReference = IsDeferredReference;

    // If the property has a modifier return the modified value
    //Debug.Asert(ModifiedValue != null);

    // outside of DO, we flatten modified value
    ModifiedValue modifiedValue = ModifiedValue;

    // Note that the modified values have an order of precedence
    // 1. Coerced Value (including Current value)
    // 2. Animated Value
    // 3. Expression Value
    // Also note that we support any arbitrary combinations of these
    // modifiers and will yet the precedence metioned above.
    if (IsCoerced)
    {
        if ((requests & RequestFlags.CoercionBaseValue) == 0)
        {
            entry.Value = modifiedValue.CoercedValue;
        }
        else
        {
            // This is the case when CoerceValue tries to query
            // the old base value for the property
            if (IsCoercedWithCurrentValue)
            {
                entry.Value = modifiedValue.CoercedValue;
            }
            else if (IsAnimated && ((requests & RequestFlags.AnimationBaseValue) == 0))
            {
                entry.Value = modifiedValue.AnimatedValue;
            }
            else if (IsExpression)
            {
                entry.Value = modifiedValue.ExpressionValue;
            }
            else
            {
                entry.Value = modifiedValue.BaseValue;
            }
        }
    }
    else if (IsAnimated)
    {
        if ((requests & RequestFlags.AnimationBaseValue) == 0)
        {
            entry.Value = modifiedValue.AnimatedValue;
        }
        else
        {
            // This is the case when [UI/Content]Element are
            // requesting the base value of an animation.
            if (IsExpression)
            {
                entry.Value = modifiedValue.ExpressionValue;
            }
            else
            {
                entry.Value = modifiedValue.BaseValue;
             }
        }
    }
    else
    {
        //Debug.Asert(IsExpression == true);

        QVariant expressionValue = modifiedValue.ExpressionValue;

        entry.Value = expressionValue;
    }

    //Debug.Asert(entry.IsDeferredReference == (entry.Value is DeferredReference), "Value and DeferredReference flag should be in sync; hitting this may mean that it's time to divide the DeferredReference flag into a set of flags, one for each modifier");

    return entry;
}

void EffectiveValueEntry::SetAnimationBaseValue(QVariant animationBaseValue)
{
    if (!HasModifiers)
    {
        Value = animationBaseValue;
    }
    else
    {
        ModifiedValue modifiedValue = ModifiedValue;

        if (IsExpression)
        {
            modifiedValue.ExpressionValue = animationBaseValue;
        }
        else
        {
            modifiedValue.BaseValue = animationBaseValue;
        }

        //the modified value may be a deferred reference so recompute this flag.
        ComputeIsDeferred();
    }
}

void EffectiveValueEntry::SetCoersionBaseValue(QVariant coersionBaseValue)
{
    if (!HasModifiers)
    {
        Value = coersionBaseValue;
    }
    else
    {
        ModifiedValue modifiedValue = ModifiedValue;

        if (IsAnimated)
        {
            modifiedValue.AnimatedValue = coersionBaseValue;
        }
        else if (IsExpression)
        {
            modifiedValue.ExpressionValue = coersionBaseValue;
        }
        else
        {
            modifiedValue.BaseValue = coersionBaseValue;
        }
        //the modified value may be a deferred reference so recompute this flag.
        ComputeIsDeferred();
    }
}

QVariant EffectiveValueEntry::LocalValue()
{
    if (BaseValueSource== BaseValueSourceInternal::Local)
    {
        if (!HasModifiers)
        {
            //Debug.Asert(Value != DependencyProperty::UnsetValue);
            return Value;
        }
        else
        {
            //Debug.Asert(ModifiedValue != null && ModifiedValue.BaseValue != DependencyProperty::UnsetValue);
            return ModifiedValue.BaseValue;
        }
    }
    else
    {
        return DependencyProperty::UnsetValue;
    }
}

void EffectiveValueEntry::SetLocalValue()
{
    //Debug.Asert(BaseValueSource== BaseValueSourceInternal::Local, "This can happen only on an entry already having a local value");

    if (!HasModifiers)
    {
        //Debug.Asert(Value != DependencyProperty::UnsetValue);
        Value = value;
    }
    else
    {
        //Debug.Asert(ModifiedValue != null && ModifiedValue.BaseValue != DependencyProperty::UnsetValue);
        ModifiedValue.BaseValue = value;
    }
}


ModifiedValue EffectiveValueEntry::ModifiedValue()
{
    if (_value != null)
    {
        return _value as ModifiedValue;
    }
    return null;
}

ModifiedValue EffectiveValueEntry::EnsureModifiedValue(bool useWeakReferenceForBaseValue=false)
{
    ModifiedValue modifiedValue = null;
    if (_value == null)
    {
        _value = modifiedValue = new ModifiedValue();
    }
    else
    {
        modifiedValue = _value as ModifiedValue;
        if (modifiedValue == null)
        {
            modifiedValue = new ModifiedValue();
            modifiedValue.SetBaseValue(_value, useWeakReferenceForBaseValue);
            _value = modifiedValue;
        }
    }
    return modifiedValue;
}

void EffectiveValueEntry::Clear()
{
    _propertyIndex = -1;
    _value = null;
    _source = 0;
}


void EffectiveValueEntry::WritePrivateFlag(FullValueSource bit, bool value)
{
    if (value)
    {
        _source |= bit;
    }
    else
    {
        _source &= ~bit;
    }
}

bool EffectiveValueEntry::ReadPrivateFlag(FullValueSource bit)
{
    return (_source & bit) != 0;
}



QVariant ModifiedValue::BaseValue()
{
     BaseValueWeakReference wr = _baseValue as BaseValueWeakReference;
     return (wr != null) ? wr.Target : _baseValue;
}


void ModifiedValue::SetBaseValue(QVariant value, bool useWeakReference)
{
    _baseValue = (useWeakReference && !value.GetType().IsValueType)
                ? new BaseValueWeakReference(value)
                : value;
}
*/
