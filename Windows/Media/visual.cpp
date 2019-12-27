#include "Windows/Media/visual.h"
#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/hittestresult.h"

#include <QDebug>
#include <QResizeEvent>
#include <QGraphicsScene>

Visual::Visual()
    : QGraphicsItem(nullptr)
{
}

void Visual::SetOpacity(double opacity)
{
    setOpacity(opacity);
}

QTransform Visual::TransformToAncestor(Visual* visual)
{
    return QTransform();
}

QTransform Visual::TransformToDescendant(Visual* visual)
{
    return QTransform();
}


void Visual::AddVisualChild(Visual * visual)
{
    visual->setParentItem(this);
    if (isVisible())
        visual->show();
}

void Visual::RemoveVisualChild(Visual * visual)
{
    if (visual->parentItem() == this) {
        if (scene())
            scene()->removeItem(visual);
        else
            visual->setParentItem(nullptr);
    }
}

Visual *Visual::VisualParent()
{
    return static_cast<Visual*>(parentItem());
}

void Visual::OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved)
{

}

HitTestResult Visual::HitTestCore(PointHitTestParameters hitTestParams)
{
    return HitTestResult(nullptr);
}

QRectF Visual::boundingRect() const
{
    return QRectF();
}

void Visual::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}
