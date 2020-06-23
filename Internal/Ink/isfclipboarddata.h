#ifndef ISFCLIPBOARDDATA_H
#define ISFCLIPBOARDDATA_H

#include "Activities/Presentation/clipboarddata.h"

#include <QSharedPointer>

INKCANVAS_BEGIN_NAMESPACE

class StrokeCollection;

// namespace MS.Internal.Ink

class ISFClipboardData : public ClipboardData
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

    //#region Constructors
public:
    // The default constructor
    ISFClipboardData();

    // The constructor which takes StrokeCollection argument
    ISFClipboardData(QSharedPointer<StrokeCollection> strokes);

    // Checks if the data can be pasted.
    virtual bool CanPaste(DataObject  const * dataObject) override;

    //#endregion Constructors

    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods
protected:
    // Checks if there is stroke data in this instance
    virtual bool CanCopy() override;

    // Copies the strokes to the IDataObject
    /// <SecurityNote>
    ///    Critical: This code asserts to set data on a data object
    /// </SecurityNote>
    //[SecurityCritical]
    virtual void DoCopy(DataObject* dataObject) override;

    // Retrieves the stroks from the IDataObject
    virtual void DoPaste(DataObject  const * dataObject) override;

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------------------------------------------

    //#region Properties

public:
    // Gets the strokes
    QSharedPointer<StrokeCollection> Strokes()
    {
       return _strokes;
    }

    //#endregion Properties

    //-------------------------------------------------------------------------------
    //
    // Private Fields
    //
    //-------------------------------------------------------------------------------

    //#region Private Fields
private:
    QSharedPointer<StrokeCollection>    _strokes;

    //#endregion Private Fields

};

INKCANVAS_END_NAMESPACE

#endif // ISFCLIPBOARDDATA_H
