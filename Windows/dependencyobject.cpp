#include "Windows/dependencyobject.h"
#include "Windows/dependencyproperty.h"
#include "Windows/dispatcher.h"
#include "Windows/Media/visual.h"

DependencyObject::DependencyObject()
{

}

void DependencyObject::SetValue(DependencyProperty const * prop, QVariant value)
{
    QVariant old = props_.value(prop);
    props_[prop] = value;
    prop->Changed(*this, old, value);
}


QVariant DependencyObject::GetValue(DependencyProperty const * prop)
{
    QVariant v = props_.value(prop);
    if (v.isValid())
        return v;
    v = prop->DefaultValue();
    if (v.isValid()) {
        props_.insert(prop, v);
        //prop->Changed(*this, QVariant(), v);
    }
    return v;
}

void DependencyObject::InvalidateSubProperty(DependencyProperty const * prop)
{

}

Dispatcher* DependencyObject::GetDispatcher()
{
    return Dispatcher::from(static_cast<Visual*>(this)->thread());
}

void DependencyObject::VerifyAccess() const
{

}
