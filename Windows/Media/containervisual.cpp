#include "Windows/Media/containervisual.h"


ContainerVisual *ContainerVisual::fromItem(QGraphicsItem *item)
{
    if (item == nullptr)
        return nullptr;
    QVariant elem = item->data(ITEM_DATA);
    if (elem.userType() == qMetaTypeId<ContainerVisual*>()) {
        ContainerVisual * ue = elem.value<ContainerVisual*>();
        if (ue == item)
            return ue;
    }
    return nullptr;
}

ContainerVisual::ContainerVisual()
    : children_(this)
{
    setData(ITEM_DATA, QVariant::fromValue(this));
}

VisualCollection& ContainerVisual::Children()
{
    return children_;
}

Geometry* ContainerVisual::Clip()
{
    return nullptr;
}

void ContainerVisual::SetClip(Geometry *)
{
}

VisualCollection::VisualCollection(ContainerVisual *parent)
    : parent_(parent)
{
}

void VisualCollection::Add(Visual *visual)
{
    parent_->AddVisualChild(visual);
    append(visual);
}

void VisualCollection::Insert(int index, Visual *visual)
{
    parent_->AddVisualChild(visual);
    insert(index, visual);
}

void VisualCollection::Remove(Visual *visual)
{
    if (contains(visual)) {
        removeOne(visual);
        parent_->RemoveVisualChild(visual);
    }
}
