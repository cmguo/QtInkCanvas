#ifndef TEXTCLIPBOARDDATA_H
#define TEXTCLIPBOARDDATA_H

#include "elementsclipboarddata.h"

#include <QString>

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class TextClipboardData : public ElementsClipboardData
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors
public:
    // The default constructor
    TextClipboardData();

    // The constructor with a string as argument
    TextClipboardData(QString text);

    //#endregion Constructors

protected:
    // Checks if the data can be pasted.
    virtual bool CanPaste(DataObject const * dataObject);

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

    // Checks if the data can be copied.
    virtual bool CanCopy();

    // Copy the text to the IDataObject
    virtual void DoCopy(DataObject * dataObject);

    // Retrieves the text from the IDataObject instance.
    // Then create a textbox with the text data.
    virtual void DoPaste(DataObject const * dataObject);

    //#endregion Protected Methods

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------

    //#region Private Fields

private:
    QString      _text;

    ////#endregion Private Fields
};

INKCANVAS_END_NAMESPACE

#endif // TEXTCLIPBOARDDATA_H
