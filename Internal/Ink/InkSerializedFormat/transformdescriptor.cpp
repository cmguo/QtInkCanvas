#include "Internal/Ink/InkSerializedFormat/transformdescriptor.h"
#include "Internal/doubleutil.h"

INKCANVAS_BEGIN_NAMESPACE

bool TransformDescriptor::Compare(TransformDescriptor const & that)
{
    if( that.Tag == Tag )
    {
        if( that.Size == Size )
        {
            for( int i = 0; i < Size; i++ )
            {
                if( !DoubleUtil::AreClose(that.Transform[i], Transform[i] ))
                    return false;
            }
            return true;
        }
        else
            return false;
    }
    return false;
}

INKCANVAS_END_NAMESPACE
