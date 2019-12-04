#ifndef CONTAINERVISUAL_H
#define CONTAINERVISUAL_H

#include "visual.h"
#include "collection.h"

// namespace System.Windows.Media

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

class ContainerVisual : public Visual
{
    Q_OBJECT
public:
    ContainerVisual();

    VisualCollection& Children();

    Geometry* Clip();

    void SetClip(Geometry*);

private:
    VisualCollection children_;
};

#endif // CONTAINERVISUAL_H
