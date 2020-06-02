#ifndef LIST_H
#define LIST_H

#include "InkCanvas_global.h"
#include "Collections/Generic/array.h"

#ifdef INKCANVAS_CORE

#include <vector>
#include <iterator>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class List : private std::vector<T>
{
public:
    typedef typename std::vector<T>::value_type value_type;
    List() {}
    List(std::initializer_list<T> list) : std::vector<T>(list) {}
    List(Array<T> const & list) { std::copy(list.begin(), list.end(), std::back_inserter(static_cast<std::vector<T>&>(*this))); }
    int Count() const { return static_cast<int>(size()); }
    void Add(T const & t) { push_back(t); }
    void AddRange(List<T> const & list) { std::copy(list.begin(), list.end(), std::back_inserter(static_cast<std::vector<T>&>(*this))); }
    void Insert(int index, T const & t) { insert(begin() + index, t); }
    void InsertRange(int index, List<T> const & list) { std::copy(list.begin(), list.end(), std::inserter(static_cast<std::vector<T>&>(*this), begin() + index)); }
    void Clear() { clear(); }
    void Remove(T const t) { auto iter = std::find(begin(), end(), t); if (iter != end()) erase(iter); }
    void RemoveAt(int index)  { erase(begin() + index); }
    void RemoveRange(int index, int count) { erase(begin() + index, begin() + index + count); }
    bool Contains(T const & t) const { return std::find(begin(), end(), t) != end(); }
    int IndexOf() { auto iter = std::find(begin(), end(), t); return iter == end() ? -1 : iter - begin(); }

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

#else

#include <QList>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class List : private QList<T>
{
public:
    int Count() const { return size(); }
    void Add(T const & t) { push_back(t); }
    void Insert(int index, T const & t) { insert(begin() + index, t); }
    void Clear() { clear(); }
    void RemoveRange(int index, int count) { erase(begin() + index, begin() + index + count); }

    T & operator[](int index) { return QList<T>::operator[](static_cast<size_t>(index)); }
    T const & operator[](int index) const { return QList<T>::operator[](static_cast<size_t>(index)); }

    using QList<T>::begin;
    using QList<T>::end;

public:
    void reserve(int count) { QList<T>::reserve(count); }
};

INKCANVAS_END_NAMESPACE

#endif

#endif // LIST_H
