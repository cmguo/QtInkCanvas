#ifndef DEPENDENCYOBJECT_H
#define DEPENDENCYOBJECT_H

#include <QVariant>

class DependencyProperty;

enum RequestFlags
{
    FullyResolved = 0x00,
    AnimationBaseValue = 0x01,
    CoercionBaseValue = 0x02,
    DeferredReferences = 0x04,
    SkipDefault = 0x08,
    RawEntry = 0x10,
};

class DependencyObject
{
public:
    DependencyObject();

public:
    void SetValue(DependencyProperty * prop, QVariant value);

    template<typename T>
    void SetValue(DependencyProperty * prop, T value)
    {
        SetValue(prop, QVariant::fromValue(value));
    }

    QVariant GetValue(DependencyProperty * prop);

    template<typename T>
    T GetValue(DependencyProperty * prop)
    {
        QVariant value = GetValue(prop);
        if (value.isNull())
            return T();
        return value.value<T>();
    }

protected:
    void VerifyAccess() const;

    template<typename TEventArgs>
    void RaiseEvent(TEventArgs & e);
};

#endif // DEPENDENCYOBJECT_H
