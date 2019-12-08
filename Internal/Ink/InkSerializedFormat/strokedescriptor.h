#ifndef STROKEDESCRIPTOR_H
#define STROKEDESCRIPTOR_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

#include <QList>

class StrokeDescriptor
{
public:
    QList<KnownTagCache::KnownTagIndex> Template;
    uint Size;

public:
    StrokeDescriptor();

    bool IsEqual(StrokeDescriptor const & strd);
};

#endif // STROKEDESCRIPTOR_H
