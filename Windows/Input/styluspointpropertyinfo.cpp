#include <stdexcept>
#include "Windows/Input/styluspointpropertyinfo.h"
#include "Windows/Input/styluspointpropertyinfodefaults.h"
#include "Windows/Input/styluspointproperty.h"

INKCANVAS_BEGIN_NAMESPACE

StylusPointPropertyInfo::StylusPointPropertyInfo(StylusPointProperty const & stylusPointProperty)
    : prop_(&stylusPointProperty) //base checks for null
{
    StylusPointPropertyInfo info =
        StylusPointPropertyInfoDefaults::GetStylusPointPropertyInfoDefault(stylusPointProperty);
    _min = info._min;
    _max = info._max;
    _resolution = info._resolution;
    _unit = info._unit;

}

StylusPointPropertyInfo::StylusPointPropertyInfo(StylusPointProperty const & stylusPointProperty, int minimum, int maximum,
                                                 StylusPointPropertyUnit unit, float resolution)
    : prop_(&stylusPointProperty) //base checks for null
{
    // validate unit
    //if (!StylusPointPropertyUnitHelper::IsDefined(unit))
    //{
    //    throw std::runtime_error("unit");
    //}

    // validate min/max
    if (maximum < minimum)
    {
        throw std::runtime_error("maximum");
    }

    // validate resolution
    if (resolution < 0.0f)
    {
        throw std::runtime_error("resolution");
    }

    _min = minimum;
    _max = maximum;
    _resolution = resolution;
    _unit = unit;
}

bool StylusPointPropertyInfo::AreCompatible(StylusPointPropertyInfo const & stylusPointPropertyInfo1, StylusPointPropertyInfo const & stylusPointPropertyInfo2)
{
    //Debug.Assert((  stylusPointPropertyInfo1.Id != StylusPointPropertyIds.X &&
    //                stylusPointPropertyInfo1.Id != StylusPointPropertyIds.Y &&
    //                stylusPointPropertyInfo2.Id != StylusPointPropertyIds.X &&
    //                stylusPointPropertyInfo2.Id != StylusPointPropertyIds.Y),
    //                "Why are you checking X, Y for compatibility?  They're always compatible");
    //
    // we only take ID and IsButton into account, we don't take metrics into account
    //
    return (stylusPointPropertyInfo1.Id() == stylusPointPropertyInfo2.Id() &&
            stylusPointPropertyInfo1.IsButton() == stylusPointPropertyInfo2.IsButton());
}

INKCANVAS_END_NAMESPACE
