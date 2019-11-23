#ifndef DEPENDENCYPROPERTY_H
#define DEPENDENCYPROPERTY_H

#include <QVariant>

class DependencyProperty
{
public:
    DependencyProperty();

    static QVariant const UnsetValue;

    int GlobalIndex();
};

#endif // DEPENDENCYPROPERTY_H
