#ifndef DEPENDENCYPROPERTY_H
#define DEPENDENCYPROPERTY_H

#include <QVariant>

// namespace System.Windows

class DependencyProperty
{
public:
    DependencyProperty();

    static QVariant const UnsetValue;

    int GlobalIndex();
};

#endif // DEPENDENCYPROPERTY_H
