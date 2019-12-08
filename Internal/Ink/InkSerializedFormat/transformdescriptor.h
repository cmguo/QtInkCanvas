#ifndef TRANSFORMDESCRIPTOR_H
#define TRANSFORMDESCRIPTOR_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

class TransformDescriptor
{
public:
    double   Transform[6];
    uint      Size = 0;
    KnownTagCache::KnownTagIndex Tag;

public:
    bool Compare(TransformDescriptor const & that);
};

#endif // TRANSFORMDESCRIPTOR_H
