#ifndef VISUAL_H
#define VISUAL_H

#include "InkCanvas_global.h"

#include "Windows/dependencyobject.h"
#include "pointhittestparameters.h"

#include <QWidget>

class HitTestResult;

// namespace System.Windows.Media

class INKCANVAS_EXPORT Visual : public QWidget, public DependencyObject
{
    Q_OBJECT
public:
    Visual();

public:
    void SetOpacity(double opacity);

    QTransform TransformToAncestor(Visual* visual);

    QTransform TransformToDescendant(Visual* visual);

    void AddVisualChild(Visual *);

    void RemoveVisualChild(Visual *);

    Visual* VisualParent();

protected:
    virtual void OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved);

    virtual HitTestResult HitTestCore(PointHitTestParameters hitTestParams);

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
};

#endif // VISUAL_H
