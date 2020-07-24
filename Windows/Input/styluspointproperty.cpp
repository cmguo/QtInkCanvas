#include "Windows/Input/styluspointproperty.h"

#include <stdexcept>

INKCANVAS_BEGIN_NAMESPACE

StylusPointProperty const StylusPointProperty::Empty;

void StylusPointProperty::Initialize(Guid const & identifier, bool isButton)
{
    //
    // validate isButton for known guids
    //
    if (StylusPointPropertyIds::IsKnownButton(identifier))
    {
        if (!isButton)
        {
            //error, this is a known button
            throw std::runtime_error("isButton");
        }
    }
    else
    {
        if (StylusPointPropertyIds::IsKnownId(identifier) && isButton)
        {
            //error, this is a known guid that is NOT a button
            throw std::runtime_error("isButton");
        }
    }

    //_id = identifier;
    //_isButton = isButton;
}

INKCANVAS_END_NAMESPACE
