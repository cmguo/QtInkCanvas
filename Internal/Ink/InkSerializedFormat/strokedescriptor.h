#ifndef STROKEDESCRIPTOR_H
#define STROKEDESCRIPTOR_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

#include <QList>

INKCANVAS_BEGIN_NAMESPACE

class StrokeDescriptor
{
public:
    QList<KnownTagCache::KnownTagIndex> Template;
    uint Size;

public:
    StrokeDescriptor();

    bool IsEqual(StrokeDescriptor const & strd);
};

INKCANVAS_END_NAMESPACE

#endif // STROKEDESCRIPTOR_H
