#include "Windows/Input/styluseventargs.h"
#include "Windows/Input/stylusdevice.h"
#include <stdexcept>
INKCANVAS_BEGIN_NAMESPACE

StylusEventArgs::StylusEventArgs(QTouchEvent& event)
    : InputEventArgs(Stylus::GetDevice(event.device()), static_cast<int>(event.timestamp()))
    , event_(&event)
{
}

/////////////////////////////////////////////////////////////////////
/// <summary>
///     Initializes a new instance of the StylusEventArgs class.
/// </summary>
/// <param name="stylus">
///     The logical Stylus device associated with this event.
///
/// <param name="timestamp">
///     The time when the input occured.
///
StylusEventArgs::StylusEventArgs(StylusDevice* stylus, int timestamp)
    : InputEventArgs(stylus, timestamp)
    , event_(nullptr)
{
    if( stylus == nullptr )
    {
        throw std::runtime_error("stylus");
    }
}

/////////////////////////////////////////////////////////////////////
/// <summary>
///     Read-only access to the stylus device associated with this
///     event.
/// </summary>
StylusDevice* StylusEventArgs::GetStylusDevice()
{
     return (StylusDevice*)Device();
}

/////////////////////////////////////////////////////////////////////
/// <summary>
///     Calculates the position of the stylus relative to a particular element.
/// </summary>
Point StylusEventArgs::GetPosition(Visual* relativeTo)
{
    return GetStylusDevice()->GetPosition(relativeTo);
}

/////////////////////////////////////////////////////////////////////
/// <summary>
///     Indicates the stylus is not touching the surface.
/// </summary>
bool StylusEventArgs::InAir()
{
     return GetStylusDevice()->InAir();
}


/////////////////////////////////////////////////////////////////////
/// <summary>
///     Indicates stylusDevice is in the inverted state.
/// </summary>
bool StylusEventArgs::Inverted()
{
     return GetStylusDevice()->Inverted();
}

/////////////////////////////////////////////////////////////////////
/// <summary>
///     Returns a StylusPointCollection for processing the data from input.
///     This method creates a new StylusPointCollection and copies the data.
/// </summary>
SharedPointer<StylusPointCollection> StylusEventArgs::GetStylusPoints(Visual* relativeTo)
{
    return GetStylusDevice()->GetStylusPoints(relativeTo);
}

/////////////////////////////////////////////////////////////////////
/// <summary>
///     Returns a StylusPointCollection for processing the data from input.
///     This method creates a new StylusPointCollection and copies the data.
/// </summary>
SharedPointer<StylusPointCollection> StylusEventArgs::GetStylusPoints(Visual* relativeTo, SharedPointer<StylusPointDescription> subsetToReformatTo)
{
    return GetStylusDevice()->GetStylusPoints(relativeTo, subsetToReformatTo);
}

INKCANVAS_END_NAMESPACE
