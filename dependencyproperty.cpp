#include "dependencyproperty.h"

DependencyProperty::DependencyProperty()
{

}

QVariant const DependencyProperty::UnsetValue;

int DependencyProperty::GlobalIndex()
{
    return 0;
}
