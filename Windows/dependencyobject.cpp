#include "Windows/dependencyobject.h"
#include "Windows/dependencyproperty.h"
#include "Windows/dispatcher.h"
#include "Windows/Media/visual.h"

#include <QGraphicsScene>
#include <QThread>

INKCANVAS_BEGIN_NAMESPACE

DependencyObject::DependencyObject()
{
}

DependencyObject::~DependencyObject()
{
}

void DependencyObject::SetValue(DependencyProperty const * prop, QVariant value)
{
    QVariant old = props_.value(prop);
    if (value == old)
        return;
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

void DependencyObject::InvalidateSubProperty(DependencyProperty const *)
{

}

Dispatcher* DependencyObject::GetDispatcher() const
{
    return Dispatcher::from(QThread::currentThread());
}

void DependencyObject::VerifyAccess() const
{
    GetDispatcher()->VerifyAccess();
}

bool DependencyObject::CheckAccess() const
{
    return GetDispatcher()->CheckAccess();
}

void DependencyObject::OnPropertyChanged(DependencyPropertyChangedEventArgs &)
{
}

INKCANVAS_END_NAMESPACE
