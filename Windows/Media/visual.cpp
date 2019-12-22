#include "Windows/Media/visual.h"
#include "Windows/Media/drawingvisual.h"
#include "Windows/Media/hittestresult.h"

#include <QDebug>
#include <QResizeEvent>

Visual::Visual()
    : QWidget(nullptr, Qt::SubWindow | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
}


void Visual::SetOpacity(double opacity)
{
    setProperty("Opacity", opacity);
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
    if (!visual->metaObject()->inherits(&DrawingVisual::staticMetaObject)
            || qobject_cast<DrawingVisual*>(visual)->GetDrawing() == nullptr) {
        visual->resize(size());
        visual->move(0, 0);
    }
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
