#ifndef ELEMENTSCLIPBOARDDATA_H
#define ELEMENTSCLIPBOARDDATA_H

#include "Activities/Presentation/clipboarddata.h"

#include <QList>

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
    ElementsClipboardData(QList<UIElement*> elements)
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
    QList<UIElement*>& Elements()
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
    QList<UIElement*>& ElementList()
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
    QList<UIElement*> _elementList;

    //#endregion Private Fields

};

#endif // ELEMENTSCLIPBOARDDATA_H
