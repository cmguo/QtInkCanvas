#include "rawstylusactions.h"

const RawStylusActions RawStylusActionsHelper::MaxActions =
    RawStylusActions::None |
    RawStylusActions::Activate |
    RawStylusActions::Deactivate |
    RawStylusActions::Down |
    RawStylusActions::Up |
    RawStylusActions::Move |
    RawStylusActions::InAirMove |
    RawStylusActions::InRange |
    RawStylusActions::OutOfRange |
    RawStylusActions::SystemGesture;

bool RawStylusActionsHelper::IsValid(RawStylusActions action)
{
    if (action < RawStylusActions::None || action > MaxActions)
    {
        return false;
    }
    return true;
}
