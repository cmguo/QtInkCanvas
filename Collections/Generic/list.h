#ifndef LIST_H
#define LIST_H

#include "InkCanvas_global.h"
#include "Collections/Generic/array.h"

#ifdef INKCANVAS_QT

#include <QList>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class List : private QList<T>
{
public:
    List() {}
    List(std::initializer_list<T> list) : QList<T>(list) {}
    List(QList<T> const & list) : QList<T>(list) {}
    List(Array<T> const & list) { for (auto & t : list) append(t); }
    int Count() const { return size(); }
    void Add(T const & t) { push_back(t); }
    void AddRange(List<T> const & list) { append(list); }
    void Insert(int index, T const & t) { insert(begin() + index, t); }
    void InsertRange(int index, List<T> const & list) {
        for (T const & t : list)
            insert(index++, t);
    }
    void Clear() { clear(); }
    bool Remove(T const & t) { return removeOne(t); }
    void RemoveRange(int index, int count) { erase(begin() + index, begin() + index + count); }
    void RemoveAt(int index)  { removeAt(index); }
    int IndexOf(T const & t) const { return indexOf(t); }
    bool Contains(T const & t) const { return contains(t); }

    T & operator[](int index) { return QList<T>::operator[](static_cast<size_t>(index)); }
    T const & operator[](int index) const { return QList<T>::operator[](static_cast<size_t>(index)); }

    Array<T> ToArray() const
    {
        Array<T> a(Count());
        a = toVector();
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
    List() {}
    List(std::initializer_list<T> list) : std::vector<T>(list) {}
    List(Array<T> const & list) { std::copy(list.begin(), list.end(), std::back_inserter(static_cast<std::vector<T>&>(*this))); }
    int Count() const { return static_cast<int>(size()); }
    void Add(T const & t) { push_back(t); }
    void AddRange(List<T> const & list) { std::copy(list.begin(), list.end(), std::back_inserter(static_cast<std::vector<T>&>(*this))); }
    void Insert(int index, T const & t) { insert(begin() + index, t); }
    void InsertRange(int index, List<T> const & list) { std::copy(list.begin(), list.end(), std::inserter(static_cast<std::vector<T>&>(*this), begin() + index)); }
    void Clear() { clear(); }
    bool Remove(T const & t) { auto iter = std::find(begin(), end(), t); if (iter == end()) return false; erase(iter); return true; }
    void RemoveAt(int index)  { erase(begin() + index); }
    void RemoveRange(int index, int count) { erase(begin() + index, begin() + index + count); }
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

#endif // LIST_H
