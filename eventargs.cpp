#include "eventargs.h"

EventArgs EventArgs::Empty;

EventArgs::EventArgs()
    : QEvent(QEvent::User)
{

}
