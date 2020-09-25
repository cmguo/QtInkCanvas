#ifndef WINDOWS_MEDIA_CONTAINERVISUAL_H
#define WINDOWS_MEDIA_CONTAINERVISUAL_H

#include "visual.h"
#include "Collections/ObjectModel/collection.h"

#include <QObject>

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class Geometry;

class ContainerVisual;

class VisualCollection : public Collection<Visual*>
{
public:
    VisualCollection(ContainerVisual * parent);
    void Add(Visual* visual);
    void Insert(int index, Visual* visual);
    void Remove(Visual* visual);

private:
    ContainerVisual* parent_;
};

class ContainerVisual : public QObject, public Visual
{
    Q_OBJECT
public:
    static constexpr int ITEM_DATA = 6000;

    static ContainerVisual* fromItem(QGraphicsItem* item);

public:
    ContainerVisual();

    VisualCollection& Children();

    Geometry* Clip();

    void SetClip(Geometry*);

private:
    VisualCollection children_;
};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_MEDIA_CONTAINERVISUAL_H
