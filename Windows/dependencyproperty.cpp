#include "Windows/dependencyproperty.h"
#include "Windows/dependencypropertychangedeventargs.h"

QVariant const DependencyProperty::UnsetValue;

DependencyProperty::DependencyProperty(QVariant dflt)
    : default_(dflt)
{
}

DependencyProperty::DependencyProperty(QVariant dflt, DependencyProperty::PropertyChangedCallback changed)
    : default_(dflt)
    , changed_(changed)
{

}

DependencyProperty::DependencyProperty(QVariant dflt, DependencyProperty::PropertyChangedCallback changed, DependencyProperty::ValidateValueCallback validate)
    : default_(dflt)
    , changed_(changed)
    , validate_(validate)
{

}

int DependencyProperty::GlobalIndex() const
{
    return 0;
}

QVariant DependencyProperty::DefaultValue() const
{
    if (default_.userType() == qMetaTypeId<DefaultValueFactory*>())
        return default_.value<DefaultValueFactory*>()->DefaultValue();
    else
        return default_;
}

void DependencyProperty::Changed(DependencyObject& d, QVariant old, QVariant new_) const
{
    if (changed_) {
        DependencyPropertyChangedEventArgs args(this, old, new_);
        changed_(d, args);
    }
}

