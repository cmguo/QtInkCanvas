#ifndef STROKEDESCRIPTOR_H
#define STROKEDESCRIPTOR_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"
#include "Collections/Generic/list.h"

INKCANVAS_BEGIN_NAMESPACE

class StrokeDescriptor
{
public:
    List<KnownTagCache::KnownTagIndex> Template;
    uint Size;

public:
    StrokeDescriptor();

    bool IsEqual(StrokeDescriptor const & strd);
};

INKCANVAS_END_NAMESPACE

#endif // STROKEDESCRIPTOR_H
