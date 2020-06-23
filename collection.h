#ifndef COLLECTION_H
#define COLLECTION_H

#include "InkCanvas_global.h"

#include <QVector>

INKCANVAS_BEGIN_NAMESPACE

template <typename T>
class Collection : protected QVector<T>
{
public:
    T const & operator[](int index) const
    {
        return QVector<T>::at(index);
    }

    int size() const
    {
        return QVector<T>::size();
    }

    int count() const
    {
        return size();
    }

    typename QVector<T>::const_iterator begin() const
    {
        return QVector<T>::begin();
    }

    typename QVector<T>::const_iterator end() const
    {
        return QVector<T>::end();
    }

    using QVector<T>::at;

    using QVector<T>::indexOf;

    using QVector<T>::contains;

    using QVector<T>::empty;

    using QVector<T>::startsWith;

    using QVector<T>::endsWith;

    using QVector<T>::constFirst;

    using QVector<T>::constLast;

protected:
    T & at(int index)
    {
        return QVector<T>::operator[](index);
    }
};

INKCANVAS_END_NAMESPACE

#endif // COLLECTION_H
