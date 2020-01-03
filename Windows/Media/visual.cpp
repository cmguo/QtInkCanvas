#include "Windows/Media/visual.h"
#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/hittestresult.h"

#include <QDebug>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QDebug>

static int count = 0;

Visual::Visual()
    : QGraphicsItem(nullptr)
{
    qDebug() << "Visual ++ " << ++count;
}

Visual::~Visual()
{
    qDebug() << "Visual -- " << --count;
}

void Visual::SetOpacity(double opacity)
{
    setOpacity(opacity);
}

QTransform Visual::TransformToAncestor(Visual* visual)
{
    return itemTransform(visual);
}

QTransform Visual::TransformToDescendant(Visual* visual)
{
    return itemTransform(visual);
}


void Visual::AddVisualChild(Visual * visual)
{
    visual->setParentItem(this);
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

void Visual::OnVisualChildrenChanged(DependencyObject*, DependencyObject*)
{

}

HitTestResult Visual::HitTestCore(PointHitTestParameters)
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
