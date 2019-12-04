#ifndef COLLECTION_H
#define COLLECTION_H

#include <QList>

template <typename T>
class Collection : protected QList<T>
{
public:
    T const & operator[](int index) const
    {
        return QList<T>::at(index);
    }

    int size() const
    {
        return QList<T>::size();
    }

    int count() const
    {
        return size();
    }

    typename QList<T>::const_iterator begin() const
    {
        return QList<T>::begin();
    }

    typename QList<T>::const_iterator end() const
    {
        return QList<T>::end();
    }

    using QList<T>::at;

    using QList<T>::indexOf;

    using QList<T>::contains;

protected:
    T & at(int index)
    {
        return QList<T>::operator[](index);
    }
};

#endif // COLLECTION_H
