#include "Windows/dependencyobject.h"

DependencyObject::DependencyObject()
{

}

void DependencyObject::SetValue(DependencyProperty const * prop, QVariant value)
{
    QVariant old = props_.value(prop);
    props_[prop] = value;
}


QVariant DependencyObject::GetValue(DependencyProperty const * prop)
{
    return props_.value(prop);
}

void DependencyObject::InvalidateSubProperty(DependencyProperty const * prop)
{

}

Dispatcher* DependencyObject::GetDispatcher()
{
    return nullptr;
}

void DependencyObject::VerifyAccess() const
{

}
