#ifndef EVENTARGS_H
#define EVENTARGS_H

#include <QEvent>

class EventArgs;

class EventHandler
{
public:
    EventHandler() : data_(nullptr), handle_(nullptr) {}

    EventHandler(nullptr_t) : data_(nullptr), handle_(nullptr) {}

    void operator()(EventArgs & e) const
    {
        handle_(data_, e);
    }

    friend bool operator==(EventHandler const & l, EventHandler const & r)
    {
        return l.data_ == r.data_ && l.handle_ == r.handle_;
    }

    friend bool operator!=(EventHandler const & l, EventHandler const & r)
    {
        return !(l == r);
    }

protected:
    EventHandler(void (*handle)(void*, EventArgs & e), void * data) : data_(data), handle_(handle) {}

private:
    void * data_ = nullptr;
    void (*handle_)(void*, EventArgs & e) = nullptr;
};

template <typename T, typename E, void (T::*H)(E & e)>
class EventHandlerT : public EventHandler
{
public:
    EventHandlerT(T * obj) : EventHandler(obj, handle) {}
private:
    static void handle(void* obj, EventArgs & e)
    {
        reinterpret_cast<T*>(obj)->H(static_cast<E&>(e));
    }
};

class EventArgs : public QEvent
{
public:
    static EventArgs Empty;

    EventArgs();
};

#endif // EVENTARGS_H
