#ifndef STYLUSPLUGIN_H
#define STYLUSPLUGIN_H

#include <QTouchEvent>

typedef QTouchEvent RawStylusInput;

class StylusPlugIn
{
public:
    StylusPlugIn();

    virtual ~StylusPlugIn();

protected:
    virtual void OnStylusEnter(RawStylusInput& rawStylusInput, bool confirmed) = 0;

    virtual void OnStylusLeave(RawStylusInput& rawStylusInput, bool confirmed) = 0;

    virtual void OnStylusDown(RawStylusInput& rawStylusInput) = 0;

    virtual void OnStylusMove(RawStylusInput& rawStylusInput) = 0;

    virtual void OnStylusUp(RawStylusInput& rawStylusInput) = 0;
};

#endif // STYLUSPLUGIN_H
