#ifndef WINDOWS_MEDIA_VISUAL_H
#define WINDOWS_MEDIA_VISUAL_H

#include "InkCanvas_global.h"

#include "Windows/dependencyobject.h"
#include "pointhittestparameters.h"
#include "Windows/Media/matrix.h"

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

    GeneralTransform TransformToAncestor(Visual* visual);

    GeneralTransform TransformToDescendant(Visual* visual);

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

#endif // WINDOWS_MEDIA_VISUAL_H
