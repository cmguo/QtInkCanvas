#include "Windows/Media/visual.h"
#include "Windows/Media/hittestresult.h"

#include <QDebug>
#include <QResizeEvent>

Visual::Visual()
    : QWidget(nullptr, Qt::SubWindow | Qt::FramelessWindowHint)
{
}


void Visual::SetOpacity(double opacity)
{

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
    visual->resize(size());
    visual->move(0, 0);
    visual->setParent(this);
    if (isVisible())
        visual->show();
}

void Visual::RemoveVisualChild(Visual * visual)
{
    if (visual->parent() == this)
        visual->setParent(nullptr);
}

Visual *Visual::VisualParent()
{
    return qobject_cast<Visual*>(parent());
}

void Visual::OnVisualChildrenChanged(DependencyObject* visualAdded, DependencyObject* visualRemoved)
{

}

HitTestResult Visual::HitTestCore(PointHitTestParameters hitTestParams)
{
    return HitTestResult(nullptr);
}

void Visual::resizeEvent(QResizeEvent *event)
{
    for (QObject * c : children()) {
        Visual * v = qobject_cast<Visual*>(c);
        if (v)
            v->resize(event->size());
    }
}
