#include "Internal/Ink/isfclipboarddata.h"
#include "Windows/Ink/strokecollection.h"

#include <QBuffer>
#include <QMimeData>

// The default constructor
ISFClipboardData::ISFClipboardData() { }

// The constructor which takes StrokeCollection argument
ISFClipboardData::ISFClipboardData(QSharedPointer<StrokeCollection> strokes)
{
    _strokes = strokes;
}

// Checks if the data can be pasted.
bool ISFClipboardData::CanPaste(DataObject const * dataObject)
{
    return dataObject->hasFormat(StrokeCollection::InkSerializedFormat);
}

//#endregion Constructors

//-------------------------------------------------------------------------------
//
// Methods
//
//-------------------------------------------------------------------------------

//#region Methods
// Checks if there is stroke data in this instance
bool ISFClipboardData::CanCopy()
{
    return (Strokes() != nullptr && Strokes()->size() != 0);
}

// Copies the strokes to the IDataObject
/// <SecurityNote>
///    Critical: This code asserts to set data on a data object
/// </SecurityNote>
//[SecurityCritical]
void ISFClipboardData::DoCopy(DataObject* dataObject)
{
    // samgeo - Presharp issue
    // Presharp gives a warning when local IDisposable variables are not closed
    // in this case, we can't call Dispose since it will also close the underlying stream
    // which needs to be open for consumers to read
#pragma warning disable 1634, 1691
#pragma warning disable 6518

    // Save the data in the data object.
    QBuffer stream;
    stream.open(QIODevice::ReadWrite);
    Strokes()->Save(&stream);
    stream.seek(0);
    //(new UIPermission(UIPermissionClipboard.AllClipboard)).Assert();//BlessedAssert
    //try
    {
        dataObject->setData(StrokeCollection::InkSerializedFormat, stream.data());
    }
    //finally
    //{
    //    UIPermission.RevertAssert();
    //}
#pragma warning restore 6518
#pragma warning restore 1634, 1691
}

// Retrieves the stroks from the IDataObject
void ISFClipboardData::DoPaste(DataObject  const * dataObject)
{
    // Check if we have ink data
    QByteArray data = dataObject->data(StrokeCollection::InkSerializedFormat);
    QBuffer stream; stream.setData(data);
    stream.open(QIODevice::ReadOnly);

    QSharedPointer<StrokeCollection> newStrokes;
    bool fSucceeded = false;
    //if ( stream != null && stream != Stream.Null )
    {
        try
        {
            // Now add these ink strokes to the InkCanvas ink collection.
            newStrokes.reset(new StrokeCollection(&stream));
            fSucceeded = true;
        }
        catch ( ... )
        {
            // If an invalid stream was passed in, we should get ArgumentException here.
            // We catch this specific exception and eat it.
            fSucceeded = false;
        }
    }

    // Depending on whether we are succeeded or not, we set the correct stroke collection here.
    _strokes = fSucceeded ? newStrokes : QSharedPointer<StrokeCollection>(new StrokeCollection());
}
