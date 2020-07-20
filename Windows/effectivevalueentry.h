#ifndef EFFECTIVEVALUEENTRY_H
#define EFFECTIVEVALUEENTRY_H

#include "dependencyobject.h"
#include "dependencyproperty.h"

#include <QVariant>

INKCANVAS_BEGIN_NAMESPACE

//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
enum class FullValueSource
{
    // Bit used to store BaseValueSource= 0x01
    // Bit used to store BaseValueSource= 0x02
    // Bit used to store BaseValueSource= 0x04
    // Bit used to store BaseValueSource= 0x08

    ValueSourceMask     = 0x000F,
    ModifiersMask       = 0x0070,
    IsExpression        = 0x0010,
    IsAnimated          = 0x0020,
    IsCoerced           = 0x0040,
    IsPotentiallyADeferredReference = 0x0080,
    HasExpressionMarker = 0x0100,
    IsCoercedWithCurrentValue = 0x200,
};

inline FullValueSource operator ~ (FullValueSource hs)
{
    using T = std::underlying_type_t <FullValueSource>;
    return static_cast<FullValueSource>(~static_cast<T>(hs));
}

inline FullValueSource operator & (FullValueSource lhs, FullValueSource rhs)
{
    using T = std::underlying_type_t <FullValueSource>;
    return static_cast<FullValueSource>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline FullValueSource& operator &= (FullValueSource& lhs, FullValueSource rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

inline FullValueSource operator | (FullValueSource lhs, FullValueSource rhs)
{
    using T = std::underlying_type_t <FullValueSource>;
    return static_cast<FullValueSource>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline FullValueSource& operator |= (FullValueSource& lhs, FullValueSource rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

// Note that these enum values are arranged in the reverse order of
// precendence for these sources. Local value has highest
// precedence and Default value has the least. Note that we do not
// store default values in the _effectiveValues cache unless it is
// being coerced/animated.
//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
enum class BaseValueSourceInternal
{
    Unknown                 = 0,
    Default                 = 1,
    Inherited               = 2,
    ThemeStyle              = 3,
    ThemeStyleTrigger       = 4,
    Style                   = 5,
    TemplateTrigger         = 6,
    StyleTrigger            = 7,
    ImplicitReference       = 8,
    ParentTemplate          = 9,
    ParentTemplateTrigger   = 10,
    Local                   = 11,
};

class DependencyProperty;
class ModifiedValue;

// namespace System.Windows

class EffectiveValueEntry
{
public:
    static EffectiveValueEntry CreateDefaultValueEntry(DependencyProperty const * dp, QVariant value);

    EffectiveValueEntry(DependencyProperty const * dp);

    EffectiveValueEntry(DependencyProperty const * dp, BaseValueSourceInternal valueSource);

    EffectiveValueEntry(DependencyProperty const * dp, FullValueSource fullValueSource);

    void SetExpressionValue(QVariant value, QVariant baseValue);

    void SetAnimatedValue(QVariant value, QVariant baseValue);

    void SetCoercedValue(QVariant value, QVariant baseValue, bool skipBaseValueChecks, bool coerceWithCurrentValue);

    void ResetAnimatedValue();

    void ResetCoercedValue();

    // remove all modifiers, retain value source, and set value to supplied value
    void ResetValue(QVariant value, bool hasExpressionMarker);

    // add an expression marker back as the base value for an expression value
    void RestoreExpressionMarker();

private:
    // Computes and set the IsDeferred hint flag.
    // This take into account all flags and should only be used sparingly.
    void ComputeIsDeferred() const;



public:
    int PropertyIndex()
    {
        return _propertyIndex;
    }
    void SetPropertyIndex(int value)
    {
        _propertyIndex = (short)value;
    }

    /// <summary>
    ///     If HasModifiers is true then it holds the value
    ///     else it holds the modified value instance
    /// </summary>
    QVariant Value() const;
    void SetValue(QVariant value);

    BaseValueSourceInternal GetBaseValueSourceInternal() const;
    void SetBaseValueSourceInternal(BaseValueSourceInternal value);

    bool IsDeferredReference() const;
    void SetIsDeferredReference(bool value);

    //set { WritePrivateFlag(FullValueSource::IsPotentiallyADeferredReference, value); }

    bool IsExpression() const;
    void SetIsExpression(bool value);

    bool IsAnimated() const;
    void SetIsAnimated(bool value);


    bool IsCoerced() const;
    void SetIsCoerced(bool value);

    bool HasModifiers() const;

    FullValueSource GetFullValueSource()
    {
        return _source;
    }


    bool HasExpressionMarker() const;
    void SetHasExpressionMarker(bool value);

    bool IsCoercedWithCurrentValue() const;
    void SetIsCoercedWithCurrentValue(bool value);

    EffectiveValueEntry GetFlattenedEntry(RequestFlags requests);

    void SetAnimationBaseValue(QVariant animationBaseValue);

    void SetCoersionBaseValue(QVariant coersionBaseValue);

    QVariant LocalValue();

    void SetLocalValue(QVariant value);


    ModifiedValue GetModifiedValue();

    ModifiedValue EnsureModifiedValue(bool useWeakReferenceForBaseValue=false);

    void Clear();

    void WritePrivateFlag(FullValueSource bit, bool value);

    bool ReadPrivateFlag(FullValueSource bit) const;


private:
    QVariant                      _value;
    short                       _propertyIndex;
    FullValueSource             _source;

};


//[FriendAccessAllowed] // Built into Base, also used by Core & Framework.
class ModifiedValue
{
    //#region InternalProperties
public:
    ModifiedValue() {}

    QVariant BaseValue();

    void SetBaseValue(QVariant value)
    {
        _baseValue = value;
    }

    QVariant ExpressionValue()
    {
        return _expressionValue;
    }
    void SetExpressionValue(QVariant value)
    {
        _expressionValue = value;
    }

    QVariant AnimatedValue()
    {
        return _animatedValue;
    }
    void SetAnimatedValue(QVariant value)
    {
        _animatedValue = value;
    }


    QVariant CoercedValue()
    {
        return _coercedValue;
    }
    void SetCoercedValue(QVariant value)
    {
        _coercedValue = value;
    }


    void SetBaseValue(QVariant value, bool useWeakReference);

    //#endregion InternalProperties

    //#region Data
private:
    QVariant _baseValue;
    QVariant _expressionValue;
    QVariant _animatedValue;
    QVariant _coercedValue;

    class BaseValueWeakReference
    {
    public:
        BaseValueWeakReference(QVariant) {}
    };

    //#endregion Data
};

INKCANVAS_END_NAMESPACE

#endif // EFFECTIVEVALUEENTRY_H
