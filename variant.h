#ifndef VARIANT_H
#define VARIANT_H

#include "InkCanvas_global.h"

#ifdef INKCANVAS_QT

#include <QVariant>

INKCANVAS_BEGIN_NAMESPACE

typedef QVariant Variant;

inline bool operator==(Variant const & l, nullptr_t)
{
    return l.isNull();
}

inline bool operator!=(Variant const & l, nullptr_t)
{
    return !l.isNull();
}

INKCANVAS_END_NAMESPACE

#else

#include <algorithm>
#include <assert.h>

INKCANVAS_BEGIN_NAMESPACE

class Variant
{
public:
    template<typename T>
    static Variant fromValue(T const & t)
    {
        return Variant(t);
    }

    Variant()
    {
    }

    Variant(nullptr_t)
    {
    }

    template<typename T>
    Variant(T const & t)
    {
        value_ = new T(t);
        copy_ = &copy<T>;
        equal_ = &equal<T>;
        destroy_ = &destroy<T>;
    }

    template<typename T>
    Variant(T const * t)
    {
        // no need copy & destroy
        value_ = t;
        copy_ = nullptr;
        equal_ = nullptr;
        destroy_ = &destroy2<T>;
    }

    Variant(Variant const & o)
        : value_(o.value_)
        , copy_(o.copy_)
        , destroy_(o.destroy_)
    {
        if (value_ && copy_)
            value_ = copy_(value_);
    }

    ~Variant()
    {
        if (value_ && destroy_)
            destroy_(value_);
        value_ = nullptr;
        copy_ = nullptr;
        destroy_ = nullptr;
    }

    friend bool operator==(Variant const & l, Variant const & r)
    {
        return l.destroy_ == r.destroy_
                && (l.equal_ ? l.equal_(l.value_, r.value_) : l.value_ == r.value_);
    }

    friend bool operator!=(Variant const & l, Variant const & r)
    {
        return !(l == r);
    }

    void swap(Variant & o)
    {
        std::swap(value_, o.value_);
        std::swap(copy_, o.copy_);
        std::swap(destroy_, o.destroy_);
    }

    template<typename T>
    T & value() const
    {
        assert(destroy_ == &destroy<T> || destroy_ == &destroy2<T>);
        return *reinterpret_cast<T*>(value_);
    }

private:
    template<typename T>
    static void* copy(void * t)
    {
        return new T(*reinterpret_cast<T*>(t));
    }

    template<typename T>
    static bool equal(void * l, void * r)
    {
        return *reinterpret_cast<T*>(l) == *reinterpret_cast<T*>(r);
    }

    template<typename T>
    static void destroy(void * t)
    {
        delete reinterpret_cast<T*>(t);
    }

    template<typename T>
    static void destroy2(void *)
    {
    }

private:
    void * value_ = nullptr;
    void* (*copy_)(void*) = nullptr;
    bool (*equal_)(void*, void*) = nullptr;
    void (*destroy_)(void*) = nullptr;
};

INKCANVAS_END_NAMESPACE

#endif

#endif // VARIANT_H
