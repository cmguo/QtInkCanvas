#ifndef WINDOWS_INPUT_STYLUSEVENTARGS_H
#define WINDOWS_INPUT_STYLUSEVENTARGS_H

#include "Windows/Input/inputeventargs.h"
#include "Windows/point.h"
#include "sharedptr.h"

#include <qevent.h>

INKCANVAS_BEGIN_NAMESPACE

class StylusDevice;
class Visual;
class StylusPointCollection;
class StylusPointDescription;

/////////////////////////////////////////////////////////////////////////
/// <summary>
///     The StylusEventArgs class provides access to the logical
///     Stylus device for all derived event args.
/// </summary>
class StylusEventArgs : public InputEventArgs
{
public:
    StylusEventArgs(QTouchEvent& event);

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
    StylusEventArgs(StylusDevice* stylus, int timestamp);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Read-only access to the stylus device associated with this
    ///     event.
    /// </summary>
    StylusDevice* GetStylusDevice();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Calculates the position of the stylus relative to a particular element.
    /// </summary>
    Point GetPosition(Visual* relativeTo);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Indicates the stylus is not touching the surface.
    /// </summary>
    bool InAir();


    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Indicates stylusDevice is in the inverted state.
    /// </summary>
    bool Inverted();

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Returns a StylusPointCollection for processing the data from input.
    ///     This method creates a new StylusPointCollection and copies the data.
    /// </summary>
    SharedPointer<StylusPointCollection> GetStylusPoints(Visual* relativeTo);

    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     Returns a StylusPointCollection for processing the data from input.
    ///     This method creates a new StylusPointCollection and copies the data.
    /// </summary>
    SharedPointer<StylusPointCollection> GetStylusPoints(Visual* relativeTo, SharedPointer<StylusPointDescription> subsetToReformatTo);

    QTouchEvent * event() const { return event_; }

protected:
    /////////////////////////////////////////////////////////////////////
    /// <summary>
    ///     The mechanism used to call the type-specific handler on the
    ///     target.
    /// </summary>
    /// <param name="genericHandler">
    ///     The generic handler to call in a type-specific way.
    ///
    /// <param name="genericTarget">
    ///     The target to call the handler on.
    ///
    //virtual void InvokeEventHandler(Delegate genericHandler, object genericTarget)
    //{
    //    StylusEventHandler handler = (StylusEventHandler) genericHandler;
    //    handler(genericTarget, this);
    //}

    /////////////////////////////////////////////////////////////////////

    //RawStylusInputReport InputReport()
    //{
    //    return _inputReport;
    //}
    //void SetRawStylusInputReport(internal value)
    //{
    //    _inputReport = value;
    //}


    /////////////////////////////////////////////////////////////////////

    //RawStylusInputReport*    _inputReport;

private:
    QTouchEvent * event_;
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INPUT_STYLUSEVENTARGS_H
