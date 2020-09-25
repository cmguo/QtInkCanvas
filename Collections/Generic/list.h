#ifndef COLLECTIONS_GENERIC_LIST_H
#define COLLECTIONS_GENERIC_LIST_H

#include "InkCanvas_global.h"
#include "Collections/Generic/array.h"

#ifdef INKCANVAS_QT

#include <QList>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class List : private QList<T>
{
public:
    typedef typename QList<T>::const_iterator const_iterator;
    List() {}
    List(std::initializer_list<T> list) : QList<T>(list) {}
    List(QList<T> const & list) : QList<T>(list) {}
    List(Array<T> const & list) { for (auto & t : list) QList<T>::append(t); }
    int Count() const { return QList<T>::size(); }
    void Add(T const & t) { QList<T>::push_back(t); }
    void AddRange(List<T> const & list) { QList<T>::append(list); }
    void Insert(int index, T const & t) { QList<T>::insert(begin() + index, t); }
    void InsertRange(int index, List<T> const & list) {
        for (T const & t : list)
            QList<T>::insert(index++, t);
    }
    void Clear() { QList<T>::clear(); }
    bool Remove(T const & t) { return QList<T>::removeOne(t); }
    void RemoveRange(int index, int count) { QList<T>::erase(begin() + index, begin() + index + count); }
    void RemoveAt(int index)  { QList<T>::removeAt(index); }
    int IndexOf(T const & t) const { return QList<T>::indexOf(t); }
    bool Contains(T const & t) const { return QList<T>::contains(t); }

    T & operator[](int index) { return QList<T>::operator[](static_cast<size_t>(index)); }
    T const & operator[](int index) const { return QList<T>::operator[](static_cast<size_t>(index)); }

    Array<T> ToArray() const
    {
        Array<T> a(Count());
        a = QList<T>::toVector();
        return a;
    }

    void swap(List<T> & o)
    {
        QList<T>::swap(o);
    }

    using QList<T>::begin;
    using QList<T>::end;
    using QList<T>::back;
    using QList<T>::removeAll;

public:
    void reserve(int count) { QList<T>::reserve(count); }
};

INKCANVAS_END_NAMESPACE

#else

#include <vector>
#include <iterator>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class List : private std::vector<T>
{
public:
    typedef typename std::vector<T>::value_type value_type;
    typedef typename std::vector<T>::const_iterator const_iterator;
    List() {}
    List(std::initializer_list<T> list) : std::vector<T>(list) {}
    List(Array<T> const & list) { std::copy(list.begin(), list.end(), std::back_inserter(static_cast<std::vector<T>&>(*this))); }
    int Count() const { return static_cast<int>(std::vector<T>::size()); }
    void Add(T const & t) { std::vector<T>::push_back(t); }
    void AddRange(List<T> const & list) { std::copy(list.begin(), list.end(), std::back_inserter(static_cast<std::vector<T>&>(*this))); }
    void Insert(int index, T const & t) { std::vector<T>::insert(begin() + index, t); }
    void InsertRange(int index, List<T> const & list) { std::copy(list.begin(), list.end(), std::inserter(static_cast<std::vector<T>&>(*this), begin() + index)); }
    void Clear() {std::vector<T>:: clear(); }
    bool Remove(T const & t) { auto iter = std::find(begin(), end(), t); if (iter == end()) return false; std::vector<T>::erase(iter); return true; }
    void RemoveAt(int index)  { std::vector<T>::erase(begin() + index); }
    void RemoveRange(int index, int count) { std::vector<T>::erase(begin() + index, begin() + index + count); }
    bool Contains(T const & t) const { return std::find(begin(), end(), t) != end(); }
    int IndexOf(T const & t) const { auto iter = std::find(begin(), end(), t); return iter == end() ? -1 : iter - begin(); }

    Array<T> ToArray() const
    {
        Array<T> a(Count());
        std::copy(begin(), end(), a.begin());
        return a;
    }

    T & operator[](int index) { return std::vector<T>::operator[](static_cast<size_t>(index)); }
    T const & operator[](int index) const { return std::vector<T>::operator[](static_cast<size_t>(index)); }

    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::back;

public:
    void reserve(int count) { std::vector<T>::reserve(count); }
};

INKCANVAS_END_NAMESPACE

#endif

#endif // COLLECTIONS_GENERIC_LIST_H
