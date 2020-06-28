#ifndef ELEMENTSCLIPBOARDDATA_H
#define ELEMENTSCLIPBOARDDATA_H

#include "Activities/Presentation/clipboarddata.h"
#include "Collections/Generic/list.h"

INKCANVAS_BEGIN_NAMESPACE

class UIElement;

// namespace MS.Internal.Ink

class ElementsClipboardData : public ClipboardData
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors
public:
    // The default constructor
    ElementsClipboardData() { }

    // The constructor which takes a FrameworkElement array.
    ElementsClipboardData(List<UIElement*> elements)
    {
        //if ( elements != null )
        //{
            ElementList() = (elements);
        //}
    }

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

    // Gets the element array.
    List<UIElement*>& Elements()
    {
        //if ( ElementList() != null )
        {
            return _elementList;
        }
        //else
        //{
        //    return new List<UIElement>();
        //}
    }

    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Protected Properties
    //
    //-------------------------------------------------------------------------------

    //#region Protected Properties

    // Sets/Gets the array list
protected:
    List<UIElement*>& ElementList()
    {
        return _elementList;
    }

    //#endregion Protected Properties

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------

    //#region Private Fields

private:
    List<UIElement*> _elementList;

    //#endregion Private Fields

};

INKCANVAS_END_NAMESPACE

#endif // ELEMENTSCLIPBOARDDATA_H
