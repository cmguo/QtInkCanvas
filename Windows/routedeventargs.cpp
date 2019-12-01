#include "Windows/routedeventargs.h"
#include "finallyhelper.h"

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
RoutedEventArgs::RoutedEventArgs()
{
}

/// <summary>
///     Constructor for <see cref="RoutedEventArgs"/>
/// </summary>
/// <param name="routedEvent">The new value that the RoutedEvent Property is being set to </param>
RoutedEventArgs::RoutedEventArgs(RoutedEvent& routedEvent)
    : RoutedEventArgs( routedEvent, nullptr)
{
}

/// <summary>
///     Constructor for <see cref="RoutedEventArgs"/>
/// </summary>
/// <param name="source">The new value that the SourceProperty is being set to </param>
/// <param name="routedEvent">The new value that the RoutedEvent Property is being set to </param>
RoutedEventArgs::RoutedEventArgs(RoutedEvent& routedEvent, DependencyObject* source)
{
    _routedEvent = &routedEvent;
    _source = _originalSource = source;
}

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
RoutedEvent& RoutedEventArgs::GetRoutedEvent()
{
    return *_routedEvent;
}
void RoutedEventArgs::SetGetRoutedEvent(RoutedEvent& value)
{
     if (UserInitiated() && InvokingHandler())
         throw std::exception("SR.Get(SRID.RoutedEventCannotChangeWhileRouting)");
     _routedEvent = &value;
}

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
void RoutedEventArgs::OverrideRoutedEvent( RoutedEvent& newRoutedEvent )
{
    _routedEvent = &newRoutedEvent;
}

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
bool RoutedEventArgs::Handled()
{
     return _flags[ HandledIndex ] ;
}
void RoutedEventArgs::SetHandled(bool value)
{
     //if (_routedEvent == nullptr)
     //{
     //    throw std::exception(SR.Get(SRID.RoutedEventArgsMustHaveRoutedEvent));
     //}
     //if( TraceRoutedEvent.IsEnabled )
     //{
     //    TraceRoutedEvent.TraceActivityItem(
     //                            TraceRoutedEvent.HandleEvent,
     //                            value,
     //                            RoutedEvent.OwnerType.Name,
     //                            RoutedEvent.Name,
     //                            this );
     //}
     // Note: We need to allow the caller to change the handled value
     // from true to false.
     //
     // We are concerned about scenarios where a child element
     // listens to a high-level event (such as TextInput) while a
     // parent element listens tp a low-level event such as KeyDown.
     // In these scenarios, we want the parent to not respond to the
     // KeyDown event, in deference to the child.
     //
     // Internally we implement this by asking the parent to only
     // respond to KeyDown events if they have focus.  This works
     // around the problem and is an example of an unofficial
     // protocol coordinating the two elements.
     //
     // But we imagine that there will be some cases we miss or
     // that third parties introduce.  For these cases, we expect
     // that the application author may need to mark the KeyDown
     // as handled in the child, and then reset the event to
     // being unhandled after the parent, so that default processing
     // and promotion still occur.
     //
     // For more information see the following task:
     // 20284: Input promotion breaks down when lower level input is intercepted
     _flags[ HandledIndex ] = value;
}

/// <summary>
///     Returns Source object that raised the RoutedEvent
/// </summary>
DependencyObject* RoutedEventArgs::Source()
{
    return _source;
}
void RoutedEventArgs::SetSource(DependencyObject* value)
{
     if (InvokingHandler() && UserInitiated())
         throw std::exception("SR.Get(SRID.RoutedEventCannotChangeWhileRouting)");
     //if (_routedEvent == nullptr)
     //{
     //    throw std::exception(SR.Get(SRID.RoutedEventArgsMustHaveRoutedEvent));
     //}
     DependencyObject* source = value ;
     if (_source == nullptr && _originalSource == nullptr)
     {
         // Gets here when it is the first time that the source is set.
         // This implies that this is also the original source of the event
         _source = _originalSource = source;
         OnSetSource(source);
     }
     else if (_source != source)
     {
         // This is the actiaon taken at all other times when the
         // source is being set to a different value from what it was
         _source = source;
         OnSetSource(source);
     }
}


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
void RoutedEventArgs::OverrideSource( DependencyObject* source )
{
    _source = source;
}

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
DependencyObject* RoutedEventArgs::OriginalSource()
{
    return _originalSource;
}

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
void RoutedEventArgs::OnSetSource(DependencyObject* source)
{
    (void) source;
}

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
void RoutedEventArgs::InvokeEventHandler(RoutedEventHandler const & handler, DependencyObject* genericTarget)
{
    //if (genericHandler == nullptr)
    //{
    //    throw std::exception("genericHandler");
    //}

    if (genericTarget == nullptr)
    {
        throw std::exception("genericTarget");
    }

    //if (_routedEvent == nullptr)
    //{
    //    throw std::exception("SR.Get(SRID.RoutedEventArgsMustHaveRoutedEvent)");
    //}

    SetInvokingHandler(true);
    FinallyHelper final([this](){
        SetInvokingHandler(false);
    });
    //try
    {
        handler(*this);
        //if (genericHandler is RoutedEventHandler)
        //{
        //    ((RoutedEventHandler)genericHandler)(genericTarget, this);
        //}
        //else
        //{
            // Restricted Action - reflection permission required
            //genericHandler.DynamicInvoke(new object[] {genericTarget, this});
        //}
    }
    //finally
    //{
    //    SetInvokingHandler(false);
    //}

}

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
void RoutedEventArgs::InvokeHandler(RoutedEventHandler const & handler, DependencyObject* target)
{
    SetInvokingHandler(true);

    FinallyHelper final([this](){
        SetInvokingHandler(false);
    });
    //try
    {
        InvokeEventHandler(handler, target);
    }
    //finally
    //{
    //    SetInvokingHandler(false);
    //}
}

///<SecurityNote>
///     Critical - access critical information, if this is a user initiated command
///     TreatAsSafe - checking user initiated bit considered safe.
///</SecurityNote>
bool RoutedEventArgs::UserInitiated()
{
     if (_flags [UserInitiatedIndex])
     {
         return true;// SecurityHelper.CallerHasUserInitiatedRoutedEventPermission();
     }
     return false;
}

/// <SecurityNote>
///     Critical - access critical information, if this is a user initiated command
/// </SecurityNote>
//[SecurityCritical]
void RoutedEventArgs::MarkAsUserInitiated()
{
    _flags [ UserInitiatedIndex ] = true;
}

/// <SecurityNote>
///     Critical - access critical information, if this is a user initiated command
///     TreatAsSafe - clearing user initiated bit considered safe.
/// </SecurityNote>
//[SecurityCritical, SecurityTreatAsSafe ]
void RoutedEventArgs::ClearUserInitiated()
{
    _flags [ UserInitiatedIndex ] = false ;
}

///<SecurityNote>
/// Critical - _flags is critical due to UserInitiated value.
/// TreatAsSafe - in this function we're not setting UserInitiated - we're setting InvokingHandler.
///</SecurityNote>
bool RoutedEventArgs::InvokingHandler()
{
     return _flags[InvokingHandlerIndex];
}
void RoutedEventArgs::SetInvokingHandler(bool value)
{
     _flags[InvokingHandlerIndex] = value;
}

//#endregion Operations



//#region Data
