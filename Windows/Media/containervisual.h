#ifndef CONTAINERVISUAL_H
#define CONTAINERVISUAL_H

#include "visual.h"

// namespace System.Windows.Media

class ContainerVisual : public Visual
{
    Q_OBJECT
public:
    QList<Visual*>& Children();

private:
    QList<Visual*> children_;
};

#endif // CONTAINERVISUAL_H
