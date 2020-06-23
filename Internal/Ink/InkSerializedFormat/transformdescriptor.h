#ifndef TRANSFORMDESCRIPTOR_H
#define TRANSFORMDESCRIPTOR_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

INKCANVAS_BEGIN_NAMESPACE

class TransformDescriptor
{
public:
    double   Transform[6];
    uint      Size = 0;
    KnownTagCache::KnownTagIndex Tag;

public:
    bool Compare(TransformDescriptor const & that);
};

INKCANVAS_END_NAMESPACE

#endif // TRANSFORMDESCRIPTOR_H
