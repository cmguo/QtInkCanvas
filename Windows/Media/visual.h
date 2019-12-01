#ifndef VISUAL_H
#define VISUAL_H

#include "Windows/dependencyobject.h"
#include "pointhittestparameters.h"

#include <QWidget>

class HitTestResult;

// namespace System.Windows.Media

class Visual : public QWidget, public DependencyObject
{
public:
    Visual();

public:
    void SetOpacity(double opacity);

protected:
    void AddVisualChild(Visual *);

    virtual void OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved);

    virtual HitTestResult HitTestCore(PointHitTestParameters hitTestParams);
};

#endif // VISUAL_H
