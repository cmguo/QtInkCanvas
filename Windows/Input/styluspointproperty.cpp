#include "Windows/Input/styluspointproperty.h"

void StylusPointProperty::Initialize(QUuid const & identifier, bool isButton)
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

    _id = identifier;
    _isButton = isButton;
}
