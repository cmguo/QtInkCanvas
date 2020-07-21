#ifndef COLLECTION_H
#define COLLECTION_H

#include "InkCanvas_global.h"
#include "Collections/Generic/list.h"

INKCANVAS_BEGIN_NAMESPACE

template <typename T>
class Collection
{
public:
    typedef typename List<T>::const_iterator const_iterator;

    T const & operator[](int index) const
    {
        return items[index];
    }

    int Count() const
    {
        return items.Count();
    }

    void Add(T const & item) {
        int index = items.Count();
        InsertItem(index, item);
    }

    void Clear() {
        ClearItems();
    }

    bool Contains(T const & item) {
        return items.Contains(item);
    }

    int IndexOf(T const & item) {
        return items.IndexOf(item);
    }

    void Insert(int index, T const & item) {
        InsertItem(index, item);
    }

    bool Remove(T const & item) {
        int index = items.IndexOf(item);
        if (index < 0) return false;
        RemoveItem(index);
        return true;
    }

    void RemoveAt(int index) {
        RemoveItem(index);
    }

    virtual void ClearItems() {
        items.Clear();
    }

    virtual void InsertItem(int index, T const & item) {
        items.Insert(index, item);
    }

    virtual void RemoveItem(int index) {
        items.RemoveAt(index);
    }

    virtual void SetItem(int index, T const & item) {
        items[index] = item;
    }

    void SetCapacity(int n) { items.reserve(n); }

    const_iterator begin() const
    {
        return items.begin();
    }

    const_iterator end() const
    {
        return items.end();
    }

    int IndexOf(T const & t) const
    {
        return items.IndexOf(t);
    }

    bool Contains(T const & t) const
    {
        return items.Contains(t);
    }

protected:
    List<T> & Items() { return items; }

    T & at(int index)
    {
        return items[index];
    }

private:
    List<T> items;
};

INKCANVAS_END_NAMESPACE

#endif // COLLECTION_H
