#ifndef ROUTEDEVENTARGS_H
#define ROUTEDEVENTARGS_H

#include "eventargs.h"

#include <QBitArray>
#include <QEvent>
#include <QList>

INKCANVAS_BEGIN_NAMESPACE

class DependencyObject;
class RoutedEventArgs;

class RoutedEventHandler;

class RoutedEvent
{
public:
    RoutedEvent(int type, int type2 = 0) : type_(type), type2_(type2) {}
    virtual ~RoutedEvent() {}
    int type() {return type_; }
    int type2() {return type2_; }
    virtual void handle(QEvent& event, QList<RoutedEventHandler> handlers);
    virtual void handle2(QEvent& event, QList<RoutedEventHandler> handlers);
public:
    void handle(QEvent &event, RoutedEventArgs& args, QList<RoutedEventHandler> handlers);
private:
    int type_;
    int type2_;
};

class RoutedEventHandler
{
public:
    void operator()(RoutedEventArgs & e) const
    {
        handle_(data_, e);
    }

    friend bool operator==(RoutedEventHandler const & l, RoutedEventHandler const & r)
    {
        return l.data_ == r.data_ && l.handle_ == r.handle_;
    }

protected:
    RoutedEventHandler(void * data, void (*handle)(void*, RoutedEventArgs & e)) : data_(data), handle_(handle) {}

private:
    void * data_ = nullptr;
    void (*handle_)(void*, RoutedEventArgs & e) = nullptr;
};

template <typename T, typename E, void (T::*H)(E & e)>
class RoutedEventHandlerT : public RoutedEventHandler
{
public:
    RoutedEventHandlerT(T * obj) : RoutedEventHandler(obj, handle) {}
private:
    static void handle(void* obj, RoutedEventArgs & e)
    {
        (reinterpret_cast<T*>(obj)->*H)(static_cast<E&>(e));
    }
};

// namespace System.Windows

class RoutedEventArgs : public EventArgs
{
public:
    /// <summary>
    ///     Constructor for <see cref="RoutedEventArgs"/>
    /// </summary>
    /// <remarks>
    ///     All members take default values <para/>
    ///     <para/>
    ///
    ///     <see cref="RoutedEventArgs.RoutedEvent"/>
    ///     defaults to nullptr <para/>
    ///     <see cref="RoutedEventArgs.Handled"/> defaults to
    ///     false <para/>
    ///     <see cref="Source"/> defaults to nullptr <para/>
    ///     <see cref="OriginalSource"/> also defaults to nullptr
    ///     <para/>
    /// </remarks>
    RoutedEventArgs();

    /// <summary>
    ///     Constructor for <see cref="RoutedEventArgs"/>
    /// </summary>
    /// <param name="routedEvent">The new value that the RoutedEvent Property is being set to </param>
    RoutedEventArgs(RoutedEvent& routedEvent);

    /// <summary>
    ///     Constructor for <see cref="RoutedEventArgs"/>
    /// </summary>
    /// <param name="source">The new value that the SourceProperty is being set to </param>
    /// <param name="routedEvent">The new value that the RoutedEvent Property is being set to </param>
    RoutedEventArgs(RoutedEvent& routedEvent, DependencyObject* source);

    virtual ~RoutedEventArgs() {}

    //#endregion Construction

    //#region External API
    /// <summary>
    ///     Returns the <see cref="RoutedEvent"/> associated
    ///     with this <see cref="RoutedEventArgs"/>
    /// </summary>
    /// <remarks>
    ///     The <see cref="RoutedEvent"/> cannot be nullptr
    ///     at any time
    /// </remarks>
    RoutedEvent& GetRoutedEvent();
    void SetRoutedEvent(RoutedEvent& value);

    /// <summary>
    ///     Changes the RoutedEvent assocatied with these RoutedEventArgs
    /// </summary>
    /// <remarks>
    ///     Only used internally.  Added to support cracking generic MouseButtonDown/Up events
    ///     into MouseLeft/RightButtonDown/Up events.
    /// </remarks>
    /// <param name="newRoutedEvent">
    ///     The new RoutedEvent to associate with these RoutedEventArgs
    /// </param>
    void OverrideRoutedEvent( RoutedEvent& newRoutedEvent );

    /// <summary>
    ///     Returns a boolean flag indicating if or not this
    ///     RoutedEvent has been handled this far in the route
    /// </summary>
    /// <remarks>
    ///     Initially starts with a false value before routing
    ///     has begun
    /// </remarks>
    ///<SecurityNote>
    /// Critical - _flags is critical due to UserInitiated value.
    /// PublicOK - in this function we're not setting UserInitiated - we're setting Handled.
    ///</SecurityNote>
    bool Handled();
    void SetHandled(bool value);

    /// <summary>
    ///     Returns Source object that raised the RoutedEvent
    /// </summary>
    DependencyObject* Source();
    void SetSource(DependencyObject* value);


    /// <summary>
    ///     Changes the Source assocatied with these RoutedEventArgs
    /// </summary>
    /// <remarks>
    ///     Only used internally.  Added to support cracking generic MouseButtonDown/Up events
    ///     into MouseLeft/RightButtonDown/Up events.
    /// </remarks>
    /// <param name="source">
    ///     The new object to associate as the source of these RoutedEventArgs
    /// </param>
    void OverrideSource( DependencyObject* source );

    /// <summary>
    ///     Returns OriginalSource object that raised the RoutedEvent
    /// </summary>
    /// <remarks>
    ///     Always returns the OriginalSource object that raised the
    ///     RoutedEvent unlike <see cref="RoutedEventArgs.Source"/>
    ///     that may vary under specific scenarios <para/>
    ///     This property acquires its value once before the event
    ///     handlers are invoked and never changes then on
    /// </remarks>
    DependencyObject* OriginalSource();

protected:
    /// <summary>
    ///     Invoked when the source of the event is set
    /// </summary>
    /// <remarks>
    ///     Changing the source of an event can often
    ///     require updating the data within the event.
    ///     For this reason, the OnSource=  method is
    ///     protected virtual and is meant to be
    ///     overridden by sub-classes of
    ///     <see cref="RoutedEventArgs"/> <para/>
    ///     Also see <see cref="RoutedEventArgs.Source"/>
    /// </remarks>
    /// <param name="source">
    ///     The new value that the SourceProperty is being set to
    /// </param>
    virtual void OnSetSource(DependencyObject* source);

    /// <summary>
    ///     Invokes the generic handler with the
    ///     appropriate arguments
    /// </summary>
    /// <remarks>
    ///     Is meant to be overridden by sub-classes of
    ///     <see cref="RoutedEventArgs"/> to provide
    ///     more efficient invocation of their delegate
    /// </remarks>
    /// <param name="genericHandler">
    ///     Generic Handler to be invoked
    /// </param>
    /// <param name="genericTarget">
    ///     Target on whom the Handler will be invoked
    /// </param>
    virtual void InvokeEventHandler(RoutedEventHandler const & handler, DependencyObject* genericTarget);

    //#endregion External API

    //#region Operations

    // Calls the InvokeEventHandler protected
    // virtual method
    //
    // This method is needed because
    // delegates are invoked from
    // RoutedEventHandler which is not a
    // sub-class of RoutedEventArgs
    // and hence cannot invoke protected
    // method RoutedEventArgs.FireEventHandler
    void InvokeHandler(RoutedEventHandler const & handler, DependencyObject* target);

public:
    ///<SecurityNote>
    ///     Critical - access critical information, if this is a user initiated command
    ///     TreatAsSafe - checking user initiated bit considered safe.
    ///</SecurityNote>
    bool UserInitiated();

    /// <SecurityNote>
    ///     Critical - access critical information, if this is a user initiated command
    /// </SecurityNote>
    //[SecurityCritical]
    void MarkAsUserInitiated();

    /// <SecurityNote>
    ///     Critical - access critical information, if this is a user initiated command
    ///     TreatAsSafe - clearing user initiated bit considered safe.
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe ]
    void ClearUserInitiated();

private:
    ///<SecurityNote>
    /// Critical - _flags is critical due to UserInitiated value.
    /// TreatAsSafe - in this function we're not setting UserInitiated - we're setting InvokingHandler.
    ///</SecurityNote>
    bool InvokingHandler();
    void SetInvokingHandler(bool value);

    //#endregion Operations



    //#region Data

private:
    RoutedEvent* _routedEvent;
    DependencyObject* _source;
    DependencyObject* _originalSource;

    ///<SecurityNote>
    /// Critical - the UserInitiated flag value is critical.
    ///</SecurityNote>
    //[SecurityCritical]
    QBitArray          _flags;

    static constexpr int HandledIndex                          = 1;
    static constexpr int UserInitiatedIndex                    = 2;
    static constexpr int InvokingHandlerIndex                  = 4;


    //#endregion Data
};

INKCANVAS_END_NAMESPACE

#endif // ROUTEDEVENTARGS_H
