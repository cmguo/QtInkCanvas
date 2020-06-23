#ifndef VISUAL_H
#define VISUAL_H

#include "InkCanvas_global.h"

#include "Windows/dependencyobject.h"
#include "pointhittestparameters.h"

#include <QGraphicsItem>

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class HitTestResult;

class INKCANVAS_EXPORT Visual : public QGraphicsItem, public DependencyObject
{
public:
    Visual();

    virtual ~Visual() override;

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

public:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

INKCANVAS_END_NAMESPACE

#endif // VISUAL_H
