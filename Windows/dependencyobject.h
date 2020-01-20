#ifndef DEPENDENCYOBJECT_H
#define DEPENDENCYOBJECT_H

#include <QVariant>
#include <QMap>

class Dispatcher;
class DependencyProperty;
class DependencyPropertyChangedEventArgs;

enum RequestFlags
{
    FullyResolved = 0x00,
    AnimationBaseValue = 0x01,
    CoercionBaseValue = 0x02,
    DeferredReferences = 0x04,
    SkipDefault = 0x08,
    RawEntry = 0x10,
};

// namespace System.Windows

class DependencyObject
{
public:
    DependencyObject();

public:
    void SetValue(DependencyProperty const * prop, QVariant value);

    template<typename T>
    void SetValue(DependencyProperty const * prop, T value)
    {
        SetValue(prop, QVariant::fromValue(value));
    }

    QVariant GetValue(DependencyProperty const * prop);

    template<typename T>
    T GetValue(DependencyProperty const * prop)
    {
        QVariant value = GetValue(prop);
        if (value.isNull())
            return T();
        return value.value<T>();
    }

    void InvalidateSubProperty(DependencyProperty const * prop);

    Dispatcher* GetDispatcher() const;

    void VerifyAccess() const;

    bool CheckAccess() const;

protected:
    virtual void OnPropertyChanged(DependencyPropertyChangedEventArgs& e);

private:
    QMap<DependencyProperty const *, QVariant> props_;
};

#endif // DEPENDENCYOBJECT_H
