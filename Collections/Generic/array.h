#ifndef ARRAY_H
#define ARRAY_H

#include "InkCanvas_global.h"

#ifdef INKCANVAS_QT

#include <QVector>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class Array : private QVector<T>
{
public:
    Array() {}
    Array(int n) { resize(n); }
    Array(QVector<T> const & array) : QVector<T>(array) {}
    Array(QVector<T> && array) : QVector<T>(std::move(array)) {}
    Array(Array const & array) : QVector<T>(array) {}
    Array(Array && array) : QVector<T>(std::move(array)) {}
    Array(std::initializer_list<T> list) : QVector<T>(list) {}

    Array & operator=(Array const & array) { QVector<T>::operator=(array); return *this; }
    Array & operator=(Array && array) { QVector<T>::operator=(std::move(array)); return *this; }

    int Length() const { return size(); }
    //void Add(T const & t) { push_back(t); }
    //void Insert(int index, T const & t) { insert(begin() + index, t); }
    //void Clear() { clear(); }

    T & operator[](int index) { return QVector<T>::operator[](static_cast<size_t>(index)); }
    T const & operator[](int index) const { return QVector<T>::operator[](static_cast<size_t>(index)); }

    using QVector<T>::begin;
    using QVector<T>::end;

public:
    using QVector<T>::empty;

    void resize(int n) { QVector<T>::resize(static_cast<size_t>(n)); }

    QVector<T> const & toQVector() const { return *this; }
    QVector<T> && toQVector() { return std::move(*this); }
};

INKCANVAS_END_NAMESPACE

#else

#include <vector>

INKCANVAS_BEGIN_NAMESPACE

template<typename T>
class Array : private std::vector<T>
{
public:
    typedef typename std::vector<T>::value_type value_type;
    Array() {}
    Array(int n) { resize(n); }
    Array(std::vector<T> const & array) : std::vector<T>(array) {}
    Array(std::vector<T> && array) : std::vector<T>(std::move(array)) {}
    Array(Array const & array) : std::vector<T>(array) {}
    Array(Array && array) : std::vector<T>(std::move(array)) {}
    Array(std::initializer_list<T> list) : std::vector<T>(list) {}

    Array & operator=(Array const & array) { std::vector<T>::operator=(array); return *this; }
    Array & operator=(Array && array) { std::vector<T>::operator=(std::move(array)); return *this; }

    int Length() const { return static_cast<int>(size()); }
    void Add(T const & t) { push_back(t); }
    void Insert(int index, T const & t) { insert(begin() + index, t); }
    void Clear() { clear(); }

    T & operator[](int index) { return std::vector<T>::operator[](static_cast<size_t>(index)); }
    T const & operator[](int index) const { return std::vector<T>::operator[](static_cast<size_t>(index)); }

    using std::vector<T>::begin;
    using std::vector<T>::end;

public:
    using std::vector<T>::empty;

    void resize(int n) { std::vector<T>::resize(static_cast<size_t>(n)); }
};

INKCANVAS_END_NAMESPACE

#endif

#endif // ARRAY_H
