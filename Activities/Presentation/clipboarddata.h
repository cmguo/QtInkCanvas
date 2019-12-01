#ifndef CLIPBOARDDATA_H
#define CLIPBOARDDATA_H

class QMimeData;
typedef QMimeData DataObject;

// namespace System.Activities.Presentation

class ClipboardData
{
    //-------------------------------------------------------------------------------
    //
    // Constructors
    //
    //-------------------------------------------------------------------------------

public:
    virtual ~ClipboardData() {};
    //-------------------------------------------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------------------------------------------

    //#region Methods
public:
    /// <summary>
    /// Copy the data to the IDataObject
    /// </summary>
    /// <param name="dataObject">The IDataObject instance</param>
    /// <returns>Returns true if the data is copied. Otherwise, returns false</returns>
    /// <SecurityNote>
    ///     Critical: This code copies ink content to the clipboard and accepts a dataobject which is
    ///     created under an elevation
    /// </SecurityNote>
    //[SecurityCritical]
    bool CopyToDataObject(DataObject* dataObject)
    {
        // Check if the data can be copied
        if ( CanCopy() )
        {
            // Do copy.
            DoCopy(dataObject);
            return true;
        }

        return false;
    }

    /// <summary>
    /// Paste the data from the IDataObject
    /// </summary>
    /// <param name="dataObject">The IDataObject instance</param>
    void PasteFromDataObject(DataObject const * dataObject)
    {
        // Check if we can paste.
        if ( CanPaste(dataObject) )
        {
            // Do Paste.
            DoPaste(dataObject);
        }
    }

    virtual bool CanPaste(DataObject const * dataObject) = 0;

    //#endregion Methods

    //-------------------------------------------------------------------------------
    //
    // Protected Methods
    //
    //-------------------------------------------------------------------------------

    //#region Protected Methods

    // Those are the abstract methods which need to be implemented in the derived classes.
protected:
    virtual bool CanCopy() = 0;
    virtual void DoCopy(DataObject* dataObject) = 0;
    virtual void DoPaste(DataObject const * dataObject) = 0;

    //#endregion Protected Methods

};

#endif // CLIPBOARDDATA_H
