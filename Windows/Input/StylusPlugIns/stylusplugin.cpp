#include "Windows/Input/StylusPlugIns/stylusplugin.h"
#include "Windows/Input/StylusPlugIns/rawstylusinput.h"
#include "Windows/Input/StylusPlugIns/stylusplugincollection.h"
#include "Windows/Input/inputeventargs.h"
#include "Windows/uielement.h"

INKCANVAS_BEGIN_NAMESPACE

StylusPlugIn::StylusPlugIn()
{

}

StylusPlugIn::~StylusPlugIn()
{

}

/////////////////////////////////////////////////////////////////////
// (in Dispatcher)
void StylusPlugIn::Added(StylusPlugInCollection* plugInCollection)
{
    _pic = plugInCollection;
    OnAdded();
    InvalidateIsActiveForInput(); // Make sure we fire OnIsActivateForInputChanged.
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on Dispatcher
/// </summary>
void StylusPlugIn::OnAdded()
{
}

/////////////////////////////////////////////////////////////////////
// (in Dispatcher)
void StylusPlugIn::Removed()
{
    // Make sure we fire OnIsActivateForInputChanged if we need to.
    if (_activeForInput)
    {
        InvalidateIsActiveForInput();
    }
    OnRemoved();
    _pic = nullptr;
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on Dispatcher
/// </summary>
void StylusPlugIn::OnRemoved()
{
}

/////////////////////////////////////////////////////////////////////
// (in RTI Dispatcher)
void StylusPlugIn::StylusEnterLeave(bool isEnter, RawStylusInput& rawStylusInput, bool confirmed)
{
    // Only fire if plugin is enabled and hooked up to plugincollection.
    if (__enabled && _pic != nullptr)
    {
        if (isEnter)
            OnStylusEnter(rawStylusInput, confirmed);
        else
            OnStylusLeave(rawStylusInput, confirmed);
    }
}

/////////////////////////////////////////////////////////////////////
// (in RTI Dispatcher)
void StylusPlugIn::RawStylusInput2(RawStylusInput& rawStylusInput)
{
    // Only fire if plugin is enabled and hooked up to plugincollection.
    if (__enabled && _pic != nullptr)
    {
        switch( rawStylusInput.Actions() )
        {
            case RawStylusActions::Down:
                OnStylusDown(rawStylusInput);
                break;
            case RawStylusActions::Move:
                OnStylusMove(rawStylusInput);
                break;
            case RawStylusActions::Up:
                OnStylusUp(rawStylusInput);
                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on RTI Dispatcher
/// </summary>
void StylusPlugIn::OnStylusEnter(RawStylusInput& rawStylusInput, bool confirmed)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on RTI Dispatcher
/// </summary>
void StylusPlugIn::OnStylusLeave(RawStylusInput& rawStylusInput, bool confirmed)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on RTI Dispatcher
/// </summary>
void StylusPlugIn::OnStylusDown(RawStylusInput& rawStylusInput)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on RTI Dispatcher
/// </summary>
void StylusPlugIn::OnStylusMove(RawStylusInput& rawStylusInput)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on RTI Dispatcher
/// </summary>
void StylusPlugIn::OnStylusUp(RawStylusInput& rawStylusInput)
{
}

/////////////////////////////////////////////////////////////////////
// (on app Dispatcher)
void StylusPlugIn::FireCustomData(void* callbackData, RawStylusActions& action, bool targetVerified)
{
    // Only fire if plugin is enabled and hooked up to plugincollection.
    if (__enabled && _pic != nullptr)
    {
        switch (action)
        {
            case RawStylusActions::Down:
                OnStylusDownProcessed(callbackData, targetVerified);
                break;
            case RawStylusActions::Move:
                OnStylusMoveProcessed(callbackData, targetVerified);
                break;
            case RawStylusActions::Up:
                OnStylusUpProcessed(callbackData, targetVerified);
                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on app Dispatcher
/// </summary>
void StylusPlugIn::OnStylusDownProcessed(void* callbackData, bool targetVerified)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on app Dispatcher
/// </summary>
void StylusPlugIn::OnStylusMoveProcessed(void* callbackData, bool targetVerified)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on app Dispatcher
/// </summary>
void StylusPlugIn::OnStylusUpProcessed(void* callbackData, bool targetVerified)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - both Dispatchers
/// </summary>
UIElement* StylusPlugIn::GetElement()
{
     return (_pic != nullptr) ? _pic->Element() : nullptr;
}
/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - both Dispatchers
/// </summary>
QRectF StylusPlugIn::ElementBounds()
{
     return (_pic != nullptr) ? _pic->Rect() : QRectF();
}
/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - get - both Dispatchers, set Dispatcher
/// </summary>
bool StylusPlugIn::Enabled()
{
     return __enabled;
}
void StylusPlugIn::SetEnabled(bool value)
{
        // Verify we are on the proper thread.
     // Verify we are on the proper thread.
     if (_pic != nullptr)
     {
         _pic->Element()->VerifyAccess();
     }
     if (value != __enabled)
     {
            // If we are currently active for input we need to lock before input before
         // If we are currently active for input we need to lock before input before
            // changing so we don't get input coming in before event is fired.
         // changing so we don't get input coming in before event is fired.
         if (_pic != nullptr && _pic->IsActiveForInput())
         {
                // Make sure lock() doesn't cause reentrancy.
             // Make sure lock() doesn't cause reentrancy.
             //using(_pic->Element()->Dispatcher().DisableProcessing())
             {
                 {
                     QMutexLocker l(&_pic->PenContextsSyncRoot());
                        // Make sure we fire the OnEnabledChanged event in the proper order
                     // Make sure we fire the OnEnabledChanged event in the proper order
                        // depending on whether we are going active or inactive so you don't
                     // depending on whether we are going active or inactive so you don't
                        // get input events after going inactive or before going active.
                     // get input events after going inactive or before going active.
                     __enabled = value;
                     if (value == false)
                     {
                            // Make sure we fire OnIsActivateForInputChanged if we need to.
                         // Make sure we fire OnIsActivateForInputChanged if we need to.
                         InvalidateIsActiveForInput();
                         OnEnabledChanged();
                     }
                     else
                     {
                         OnEnabledChanged();
                         InvalidateIsActiveForInput();
                     }
                 }
             }
         }
         else
         {
             __enabled = value;
             if (value == false)
             {
                    // Make sure we fire OnIsActivateForInputChanged if we need to.
                 // Make sure we fire OnIsActivateForInputChanged if we need to.
                 InvalidateIsActiveForInput();
                 OnEnabledChanged();
             }
             else
             {
                 OnEnabledChanged();
                 InvalidateIsActiveForInput();
             }
         }
     }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on app Dispatcher
/// </summary>
void StylusPlugIn::OnEnabledChanged()
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - app Dispatcher
/// </summary>
void StylusPlugIn::InvalidateIsActiveForInput()
{

    bool newIsActive = (_pic != nullptr) ? (Enabled() &&  _pic->contains(this) &&
        _pic->IsActiveForInput()) : false;

    if (newIsActive != _activeForInput)
    {
        _activeForInput = newIsActive;
        OnIsActiveForInputChanged();
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - get - both Dispatchers
/// </summary>
bool StylusPlugIn::IsActiveForInput()
{
     return _activeForInput;
}


/////////////////////////////////////////////////////////////////////
/// <summary>
/// [TBS] - on Dispatcher
/// </summary>
void StylusPlugIn::OnIsActiveForInputChanged()
{
}

INKCANVAS_END_NAMESPACE
