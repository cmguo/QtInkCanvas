#include "strokedescriptor.h"

StrokeDescriptor::StrokeDescriptor()
{

}

bool StrokeDescriptor::IsEqual(StrokeDescriptor const & strd)
{
    // If the no of templates in them are different return false
    if( Template.size() != strd.Template.size() )
        return false;

    // Compare each tag in the template. If any one of them is different, return false;
    for( int i = 0; i < Template.size(); i++ )
        if( Template[i] != strd.Template[i] )
            return false;
    return true;
}
