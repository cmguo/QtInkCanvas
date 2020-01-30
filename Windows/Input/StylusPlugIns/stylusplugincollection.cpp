#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/Input/StylusPlugIns/stylusplugin.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/pencontexts.h"
#include "Windows/uielement.h"
#include "Windows/dependencypropertychangedeventargs.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

/// <summary>
/// Insert a StylusPlugIn in the collection at a specific index.
/// This method should be called from the application context only
/// </summary>
/// <param name="index">index at which to insert the StylusPlugIn object
/// <param name="plugIn">StylusPlugIn object to insert, downcast to an object
void StylusPlugInCollection::InsertItem(int index, StylusPlugIn* plugIn)
{
    // Verify it's called from the app dispatcher
    _element->VerifyAccess();

    // Validate the input parameter
    if (nullptr == plugIn)
    {
        throw std::runtime_error("plugIn");
    }

    if (indexOf(plugIn) != -1)
    {
        throw std::runtime_error("plugIn");
    }

    // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
    // which a call to Lock() can cause.
    //_element->Dispatcher().DisableProcessing();
    {
        if (IsActiveForInput())
        {
            // If we are currently active for input then we have a _penContexts that we must lock!
            {
                QMutexLocker l(&PenContextsSyncRoot());
                Debug::Assert(size() > 0); // If active must have more than one plugin already
                insert(index, plugIn);
                plugIn->Added(this);
            }
        }
        else
        {
            EnsureEventsHooked(); // Hook up events to track changes to the plugin's element
            insert(index, plugIn);
            //try
            {
                FinallyHelper final([this](){
                    UpdatePenContextsState();
                });
                plugIn->Added(this); // Notify plugin that it has been added to collection
            }
            //finally
            //{
            //    UpdatePenContextsState(); // Add to PenContexts if element is in proper state (can fire isactiveforinput).
            //}
        }
    }
}

/// <summary>
/// Remove all the StylusPlugIn objects from the collection.
/// This method should be called from the application context only.
/// </summary>
void StylusPlugInCollection::ClearItems()
{
    // Verify it's called from the app dispatcher
    _element->VerifyAccess();

    if (size() != 0)
    {
        // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
        // which a call to Lock() can cause.
        //using (_element->Dispatcher.DisableProcessing())
        {
            if (IsActiveForInput())
            {
                // If we are currently active for input then we have a _penContexts that we must lock!
                QMutexLocker l(&PenContextsSyncRoot());
                {
                    while (size() > 0)
                    {
                        RemoveItem(0);  // Does work to fire event and remove from collection and pencontexts
                    }
                }
            }
            else
            {
                while (size() > 0)
                {
                    RemoveItem(0);  // Does work to fire event and remove from collection.
                }
            }
        }
    }
}

/// <summary>
/// Remove the StylusPlugIn in the collection at the specified index.
/// This method should be called from the application context only.
/// </summary>
/// <param name="index">
void StylusPlugInCollection::RemoveItem(int index)
{
    // Verify it's called from the app dispatcher
    _element->VerifyAccess();

    // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
    // which a call to Lock() can cause.
    //using (_element->Dispatcher.DisableProcessing())
    {
        if (IsActiveForInput())
        {
            // If we are currently active for input then we have a _penContexts that we must lock!
            {
                QMutexLocker l(&PenContextsSyncRoot());
                StylusPlugIn* removedItem = (*this)[index];
                removeAt(index);
                //try
                {
                    FinallyHelper final([removedItem](){
                        removedItem->Removed();
                    });
                    EnsureEventsAndPenContextsUnhooked(); // Clean up events and remove from pencontexts
                }
                //finally
                //{
                //    removedItem.Removed(); // Notify plugin it has been removed
                //}
            }
        }
        else
        {
            StylusPlugIn* removedItem = (*this)[index];
            removeAt(index);
            //try
            {
                FinallyHelper final([removedItem](){
                    removedItem->Removed();
                });
                EnsureEventsAndPenContextsUnhooked(); // Clean up events and remove from pencontexts
            }
            //finally
            //{
            //    removedItem.Removed(); // Notify plugin it has been removed
            //}
        }
    }
}

/// <summary>
/// Indexer to retrieve/set a StylusPlugIn at a given index in the collection
/// Accessible from both the real time context and application context.
/// </summary>
void StylusPlugInCollection::SetItem(int index, StylusPlugIn* plugIn)
{
    // Verify it's called from the app dispatcher
    _element->VerifyAccess();

    if (nullptr == plugIn)
    {
        throw std::runtime_error("plugIn");
    }

    if (indexOf(plugIn) != -1)
    {
        throw std::runtime_error("plugIn");
    }

    // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
    // which a call to Lock() can cause.
    //using (_element->Dispatcher.DisableProcessing())
    {
        if (IsActiveForInput())
        {
            // If we are currently active for input then we have a _penContexts that we must lock!
            {
                QMutexLocker l(&PenContextsSyncRoot());
                StylusPlugIn* originalPlugIn = (*this)[index];
                at(index) = plugIn;
                //try
                {
                    FinallyHelper final([plugIn, this](){
                        plugIn->Added(this);
                    });
                    originalPlugIn->Removed();
                }
                //finally
                //{
                //    plugIn->Added(this);
                //}
            }
        }
        else
        {
            StylusPlugIn* originalPlugIn = (*this)[index];
            at(index) = plugIn;
            //try
            {
                FinallyHelper final([plugIn, this](){
                    plugIn->Added(this);
                });
                originalPlugIn->Removed();
            }
            //finally
            //{
            //    plugIn->Added(this);
            //}
        }
    }
}

//#endregion

//#region Internal APIs
/// <summary>
/// Constructor
/// </summary>
/// <param name="element">
StylusPlugInCollection::StylusPlugInCollection(UIElement* element)
{
    _element = element;

    //_isEnabledChangedEventHandler = new DependencyPropertyChangedEventHandler(OnIsEnabledChanged);
    //_isVisibleChangedEventHandler = new DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
    //_isHitTestVisibleChangedEventHandler = new DependencyPropertyChangedEventHandler(OnIsHitTestVisibleChanged);
    //_sourceChangedEventHandler = new SourceChangedEventHandler(OnSourceChanged);
    //_layoutChangedEventHandler = new EventHandler(OnLayoutUpdated);
}

/// <summary>
/// Get the UIElement
/// This method is called from the real-time context.
/// </summary>
UIElement* StylusPlugInCollection::Element()
{
    return _element;
}

/// <summary>
/// Update the rectangular bound of the element
/// This method is called from the application context.
/// </summary>
void StylusPlugInCollection::UpdateRect()
{
    // The RenderSize is only valid if IsArrangeValid is true.
    if (_element->IsArrangeValid() && _element->IsEnabled() && _element->IsVisible() && _element->IsHitTestVisible())
    {
        _rc = QRectF(QPointF(), _element->RenderSize());// _element->GetContentBoundingBox();
        Visual* root = nullptr; //VisualTreeHelper.GetContainingVisual2D(InputElement.GetRootVisual(_element));

        try
        {
            _viewToElement = root->TransformToDescendant(_element);
        }
        catch(...)
        {
            // This gets hit if the transform is not invertable.  In that case
            // we will just not allow this plugin to be hit.
            _rc = QRectF(); // empty rect so we don't hittest it.
            _viewToElement = QTransform();
        }
    }
    else
    {
        _rc = QRectF(); // empty rect so we don't hittest it.
    }
    //if (_viewToElement == nullptr)
    //{
    //    _viewToElement = QTransform();
    //}
}

/// <summary>
/// Check whether a point hits the element
/// This method is called from the real-time context.
/// </summary>
/// <param name="pt">a point to check
/// <returns>true if the point is within the bound of the element; false otherwise</returns>
bool StylusPlugInCollection::IsHit(QPointF pt)
{
    QPointF ptElement = pt;
    ptElement = _viewToElement.map(ptElement);
    return _rc.contains(ptElement);
}

/// <summary>
/// Get the transform matrix from the root visual to the current UIElement
/// This method is called from the real-time context.
/// </summary>
QTransform StylusPlugInCollection::ViewToElement()
{
    return _viewToElement;
}

/// <summary>
/// Get the current rect for the Element that the StylusPlugInCollection is attached to.
/// May be empty rect if plug in is not in tree.
/// </summary>
QRectF StylusPlugInCollection::Rect()
{
   return _rc;
}

/// <summary>
/// Get the current rect for the Element that the StylusPlugInCollection is attached to.
/// May be empty rect if plug in is not in tree.
/// </summary>
/// <securitynote>
/// Critical - Accesses SecurityCritical data _penContexts->
/// TreatAsSafe - Just returns if _pencontexts is nullptr.  No data goes in or out.  Knowing
///               the fact that you can recieve real time input is something that is safe
///               to know and we want to expose.
/// </securitynote>
bool StylusPlugInCollection::IsActiveForInput()
{
    return _penContexts != nullptr;
}


/// <securitynote>
/// Critical - Accesses SecurityCritical data _penContexts->
/// TreatAsSafe - The [....] object on the _penContexts object is not considered security
///                 critical data.  It is already internally exposed directly on the
///                 PenContexts object.
/// </securitynote>
QMutex& StylusPlugInCollection::PenContextsSyncRoot()
{
    static QMutex unused;
    return _penContexts != nullptr ? _penContexts->SyncRoot() : unused;
}


/// <summary>
/// Fire the Enter notification.
/// This method is called from pen threads and app thread.
/// </summary>
void StylusPlugInCollection::FireEnterLeave(bool isEnter, RawStylusInput& rawStylusInput, bool confirmed)
{
    if (IsActiveForInput())
    {
        // If we are currently active for input then we have a _penContexts that we must lock!
        {
            QMutexLocker l(&PenContextsSyncRoot());
            for (int i = 0; i < size(); i++)
            {
                (*this)[i]->StylusEnterLeave(isEnter, rawStylusInput, confirmed);
            }
        }
    }
    else
    {
        for (int i = 0; i < size(); i++)
        {
            (*this)[i]->StylusEnterLeave(isEnter, rawStylusInput, confirmed);
        }
    }
}

/// <summary>
/// Fire RawStylusInputEvent for all the StylusPlugIns
/// This method is called from the real-time context (pen thread) only
/// </summary>
/// <param name="args">
void StylusPlugInCollection::FireRawStylusInput(RawStylusInput& args)
{
    //try
    {
        FinallyHelper final([&args](){
            args.SetCurrentNotifyPlugIn(nullptr);
        });
        if (IsActiveForInput())
        {
            // If we are currently active for input then we have a _penContexts that we must lock!
            {
                QMutexLocker l(&PenContextsSyncRoot());
                for (int i = 0; i < size(); i++)
                {
                    StylusPlugIn* plugIn = (*this)[i];
                    // set current plugin so any callback data gets an owner.
                    args.SetCurrentNotifyPlugIn(plugIn);
                    plugIn->RawStylusInput2(args);
                }
            }
        }
        else
        {
            for (int i = 0; i < size(); i++)
            {
                StylusPlugIn* plugIn = (*this)[i];
                // set current plugin so any callback data gets an owner.
                args.SetCurrentNotifyPlugIn(plugIn);
                plugIn->RawStylusInput2(args);
            }
        }
    }
    //finally
    //{
    //    args.SetCurrentNotifyPlugIn(nullptr);
    //}
}


/// <securitynote>
///     Critical:  Accesses critical member _penContexts->
/// </securitynote>
PenContexts* StylusPlugInCollection::GetPenContexts()
{
    return _penContexts;
}

//#endregion

//#region APIs

/// <summary>
/// Add this StylusPlugInCollection to the StylusPlugInCollectionList when it the first
/// element is added.
/// </summary>
/// <securitynote>
/// Critical - Presentation source access
/// TreatAsSafe: - PresentationSource makes a SecurityDemand
///                    - no data handed out or accepted
///                    - called by Add and Insert
/// </securitynote>
void StylusPlugInCollection::EnsureEventsHooked()
{
    if (size() == 0)
    {
        // Grab current element info
        UpdateRect();
        // Now hook up events to track on this element.
        //_element->IsEnabledChanged += _isEnabledChangedEventHandler;
        //_element->IsVisibleChanged += _isVisibleChangedEventHandler;
        //_element->IsHitTestVisibleChanged += _isHitTestVisibleChangedEventHandler;
        //PresentationSource.AddSourceChangedHandler(_element, _sourceChangedEventHandler);  // has a security linkdemand
        //_element->LayoutUpdated += _layoutChangedEventHandler;

        QObject::connect(_element, &UIElement::IsVisibleChanged, this, &StylusPlugInCollection::OnIsVisibleChanged);
        QObject::connect(_element, &UIElement::IsEnabledChanged, this, &StylusPlugInCollection::OnIsEnabledChanged);
        QObject::connect(_element, &UIElement::IsHitTestVisibleChanged, this, &StylusPlugInCollection::OnIsHitTestVisibleChanged);
        //if (_element->RenderTransform != nullptr &&
        //    !_element->RenderTransform.IsFrozen)
        //{
        //    if (_renderTransformChangedEventHandler == nullptr)
        //    {
        //        _renderTransformChangedEventHandler = new EventHandler(OnRenderTransformChanged);
        //        _element->RenderTransform.Changed  += _renderTransformChangedEventHandler;
        //    }
        //}
    }
}

/// <summary>
/// Remove this StylusPlugInCollection from the StylusPlugInCollectionList when it the last
/// element is removed for this collection.
/// </summary>
void StylusPlugInCollection::EnsureEventsAndPenContextsUnhooked()
{
    if (size() == 0)
    {
        // Unhook events.
        //_element->IsEnabledChanged -= _isEnabledChangedEventHandler;
        //_element->IsVisibleChanged -= _isVisibleChangedEventHandler;
        //_element->IsHitTestVisibleChanged -= _isHitTestVisibleChangedEventHandler;
        //if (_renderTransformChangedEventHandler != nullptr)
        //{
        //    _element->RenderTransform.Changed  -= _renderTransformChangedEventHandler;
        //}
        //PresentationSource.RemoveSourceChangedHandler(_element, _sourceChangedEventHandler);
        //_element->LayoutUpdated -= _layoutChangedEventHandler;
        QObject::disconnect(_element, &UIElement::IsVisibleChanged, this, &StylusPlugInCollection::OnIsVisibleChanged);
        QObject::disconnect(_element, &UIElement::IsEnabledChanged, this, &StylusPlugInCollection::OnIsEnabledChanged);
        QObject::disconnect(_element, &UIElement::IsHitTestVisibleChanged, this, &StylusPlugInCollection::OnIsHitTestVisibleChanged);

        // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
        // which a call to Lock() can cause.
        //using (_element->Dispatcher.DisableProcessing())
        {
            // Make sure we are unhooked from PenContexts if we don't have any plugins.
            UnhookPenContexts();
        }
    }
}

void StylusPlugInCollection::OnIsEnabledChanged()
{
    //Debug::Assert(_element->IsEnabled() == e.NewValueT<bool>());
    UpdatePenContextsState();
}

void StylusPlugInCollection::OnIsVisibleChanged()
{
    //Debug::Assert(_element->IsVisible() == e.NewValueT<bool>());
    UpdatePenContextsState();
}

void StylusPlugInCollection::OnIsHitTestVisibleChanged()
{
    //Debug::Assert(_element->IsHitTestVisible() == e.NewValueT<bool>());
    UpdatePenContextsState();
}

void StylusPlugInCollection::OnRenderTransformChanged(EventArgs& e)
{
    OnLayoutUpdated(e);
}

void StylusPlugInCollection::OnSourceChanged(SourceChangedEventArgs& e)
{
    // This means that the element has been added or remvoed from its source.
    UpdatePenContextsState();
}

void StylusPlugInCollection::OnLayoutUpdated(EventArgs& e)
{
    // Make sure our rect and transform is up to date on layout changes.

    // NOTE: We need to make sure we do this under a lock if we are active for input since we don't
    // want the PenContexts code to get a mismatched set of state for this element.
    if (IsActiveForInput())
    {
        // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
        // which a call to QMutexLocker l(&) can cause.
        //using (_element->Dispatcher.DisableProcessing())
        {
            // If we are currently active for input then we have a _penContexts that we must lock!
            {
                QMutexLocker l(&PenContextsSyncRoot());
                UpdateRect();
            }
        }
    }
    else
    {
        UpdateRect();
    }

    if (_lastRenderTransform != _element->RenderTransform())
    {
        //if (_renderTransformChangedEventHandler != nullptr)
        //{
        //    _lastRenderTransform.Changed -= _renderTransformChangedEventHandler;
        //    _renderTransformChangedEventHandler = nullptr;
        //}

        _lastRenderTransform = _element->RenderTransform();
    }
/*
    if (_lastRenderTransform != nullptr)
    {
        if (_lastRenderTransform.IsFrozen)
        {
            if (_renderTransformChangedEventHandler != nullptr)
            {
                _renderTransformChangedEventHandler = nullptr;
            }
        }
        else
        {
            if (_renderTransformChangedEventHandler == nullptr)
            {
                _renderTransformChangedEventHandler = new EventHandler(OnRenderTransformChanged);
                _lastRenderTransform.Changed += _renderTransformChangedEventHandler;
            }
        }
    }*/
}

// On app ui dispatcher
/// <securitynote>
/// Critical - Presentation source access
///            Calls SecurityCritical routines PresentationSource.CriticalFromVisual and
///            HwndSource.CriticalHandle.
/// TreatAsSafe:
///          - no data handed out or accepted
/// </securitynote>
void StylusPlugInCollection::UpdatePenContextsState()
{
    bool unhookPenContexts = true;

    // Disable processing of the queue during blocking operations to prevent unrelated reentrancy
    // which a call to QMutexLocker l(&) can cause.
    //using (_element->Dispatcher.DisableProcessing())
    {
        // See if we should be enabled
        if (_element->IsVisible() && _element->IsEnabled() && _element->IsHitTestVisible())
        {
            //PresentationSource presentationSource = PresentationSource.CriticalFromVisual(_element as Visual);

            //if (presentationSource != nullptr)
            {
                unhookPenContexts = false;

                // Are we currently hooked up?  If not then hook up.
                if (_penContexts == nullptr)
                {
                    //InputManager inputManager = (InputManager)_element->Dispatcher.InputManager;
                    PenContexts* penContexts = _element->GetPenContexts(); //inputManager.StylusLogic.GetPenContextsFromHwnd(presentationSource);

                    // _penContexts must be non nullptr or don't do anything.
                    if (penContexts != nullptr)
                    {
                        _penContexts = penContexts;

                        {
                            QMutexLocker l(&penContexts->SyncRoot());
                            penContexts->AddStylusPlugInCollection(this);

                            for (StylusPlugIn* spi : *this)
                            {
                                spi->InvalidateIsActiveForInput(); // Uses _penContexts being set to determine active state.

                            }
                            // NTRAID:WINDOWSOS#1677277-2006/06/05-WAYNEZEN,
                            // Normally the Rect will be updated when we receive the LayoutUpdate.
                            // However there could be a race condition which the LayoutUpdate gets received
                            // before the properties like IsVisible being set.
                            // So we should always force to call OnLayoutUpdated whenever the input is active.
                            OnLayoutUpdated(EventArgs::Empty);
                        }

                    }
                }
            }
        }

        if (unhookPenContexts)
        {
            UnhookPenContexts();
        }
    }
}

/// <securitynote>
/// Critical - _penContexts access
/// TreatAsSafe:
///          - no data handed out or accepted
/// </securitynote>
void StylusPlugInCollection::UnhookPenContexts()
{
    // Are we currently unhooked?  If not then unhook.
    if (_penContexts != nullptr)
    {
        {
            QMutexLocker l(&_penContexts->SyncRoot());
            _penContexts->RemoveStylusPlugInCollection(this);
            // Can't recieve any more input now!
            _penContexts = nullptr;

            // Notify after input is disabled to the PlugIns.
            for (StylusPlugIn* spi : *this)
            {
                spi->InvalidateIsActiveForInput();
            }
        }
    }
}
