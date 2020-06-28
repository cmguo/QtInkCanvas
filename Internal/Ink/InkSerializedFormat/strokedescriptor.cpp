#include "strokedescriptor.h"

INKCANVAS_BEGIN_NAMESPACE

StrokeDescriptor::StrokeDescriptor()
{

}

bool StrokeDescriptor::IsEqual(StrokeDescriptor const & strd)
{
    // If the no of templates in them are different return false
    if( Template.Count() != strd.Template.Count() )
        return false;

    // Compare each tag in the template. If any one of them is different, return false;
    for( int i = 0; i < Template.Count(); i++ )
        if( Template[i] != strd.Template[i] )
            return false;
    return true;
}

INKCANVAS_END_NAMESPACE
