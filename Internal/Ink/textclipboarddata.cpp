#include "Internal/Ink/textclipboarddata.h"

#include <QMimeData>

TextClipboardData::TextClipboardData()
    : TextClipboardData(nullptr)
{

}

TextClipboardData::TextClipboardData(QString text)
    : _text(text)
{

}

// Checks if the data can be pasted.
bool TextClipboardData::CanPaste(DataObject const * dataObject)
{
    return ( dataObject->hasFormat("UnicodeText")
                || dataObject->hasFormat("Text"));
}

//-------------------------------------------------------------------------------
//
// Protected Methods
//
//-------------------------------------------------------------------------------

//#region Protected Methods

// Checks if the data can be copied.
bool TextClipboardData::CanCopy()
{
    return !_text.isEmpty();
}

// Copy the text to the IDataObject
void TextClipboardData::DoCopy(DataObject * dataObject)
{
    // Put the text to the clipboard
    dataObject->setData("UnicodeText", _text.toUtf8());
}

// Retrieves the text from the IDataObject instance.
// Then create a textbox with the text data.
void TextClipboardData::DoPaste(DataObject const * dataObject)
{
    ElementList() = QList<UIElement*>();

    // Get the string from the data object.
    QString text = dataObject->data("UnicodeText");

    if ( text.isEmpty() )
    {
        // OemText can be retrieved as CF_TEXT.
        text = dataObject->data("Text");
    }

    if ( !text.isEmpty() )
    {
        // Now, create a text box and set the text to it.
        //TextBox textBox = new TextBox();

        //textBox.Text = text;
        //textBox.TextWrapping = TextWrapping.Wrap;

        // Add the textbox to the array list.
        //ElementList().Add(textBox);
    }

}
